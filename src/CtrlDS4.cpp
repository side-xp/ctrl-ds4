#include "CtrlDS4.h"

#include "CtrlDS4/AxisValue.h"
#include "CtrlDS4/UniqueId.h"

namespace CtrlDS4 {

namespace {

/**
 * @brief Builds the MAC address used when the board has no usable unique ID.
 *
 * It's locally administered, like the addresses derived from a unique ID, so it can't match the
 * address of a real device.
 *
 * @return The fallback MAC address.
 */
MacAddress makeFallbackMacAddress() {
  return MacAddress(0x02, 0x00, 0x00, 0x00, 0x00, 0x01);
}

/// The sensor timestamp of the input report counts SENSOR_TICKS_PER_PERIOD every SENSOR_PERIOD_MICROS.
constexpr uint64_t SENSOR_TICKS_PER_PERIOD = 3;
/// See SENSOR_TICKS_PER_PERIOD.
constexpr uint64_t SENSOR_PERIOD_MICROS = 16;

}  // namespace

// Chooses the MAC address, then connects to the host as a DS4.
bool Controller::begin() {
  if (_macAddressSource != MacFromSketch) {
    uint8_t uniqueId[internal::UNIQUE_ID_MAX_SIZE];
    uint8_t uniqueIdSize = internal::readUniqueId(uniqueId);
    if (uniqueIdSize > 0) {
      _macAddress = internal::makeMacAddress(uniqueId, uniqueIdSize);
      _macAddressSource = MacFromUniqueId;
    } else {
      _macAddress = makeFallbackMacAddress();
      _macAddressSource = MacFallback;
    }
  }

  _isReady = _usbDevice.begin(_macAddress, _isUsbSerialEnabled);
  _lastReportMicros = micros();
  return _isReady;
}

// Exchanges the latest state with the host.
void Controller::update() {
  if (!_isReady) {
    return;
  }

  // Reading analog pins is slow, so they're only read when their values are about to be sent.
  bool shouldSendReport = isReportDue();
  readBindings(shouldSendReport);
  applyHostOutput();
  if (shouldSendReport) {
    sendInputReport();
  }
}

// Sets the bound buttons, triggers and stick axes from their pins.
void Controller::readBindings(bool shouldReadAnalog) {
  for (uint8_t button = 0; button < internal::BUTTON_COUNT; button++) {
    internal::ButtonBinding &binding = _buttonBindings[button];
    if (binding.isAttached()) {
      setButton(static_cast<Button>(button), binding.read(_debounceMs));
    }
  }

  for (uint8_t trigger = 0; trigger < internal::TRIGGER_COUNT; trigger++) {
    internal::AxisHalfInput &binding = _triggerBindings[trigger];
    if (binding.isAttached()) {
      setTriggerValue(static_cast<Trigger>(trigger), binding.read(_debounceMs, shouldReadAnalog));
    }
  }

  for (uint8_t axis = 0; axis < internal::STICK_AXIS_COUNT; axis++) {
    internal::StickAxisBinding &binding = _stickAxisBindings[axis];
    if (binding.isAttached()) {
      setAxis(static_cast<StickAxis>(axis), binding.read(_debounceMs, shouldReadAnalog));
    }
  }
}

// Applies the rumble and lightbar changes requested by the host, if any.
void Controller::applyHostOutput() {
  internal::OutputState output;
  if (!_usbDevice.readOutputState(output)) {
    return;
  }

  bool hasRumbleChanged = output.strongMotor != _output.strongMotor
                          || output.weakMotor != _output.weakMotor;
  bool hasLightbarChanged = output.lightbarColor.red != _output.lightbarColor.red
                            || output.lightbarColor.green != _output.lightbarColor.green
                            || output.lightbarColor.blue != _output.lightbarColor.blue;
  bool hasLightbarBlinkChanged = output.lightbarBlink.onMs != _output.lightbarBlink.onMs
                                 || output.lightbarBlink.offMs != _output.lightbarBlink.offMs;
  _output = output;

  if (hasRumbleChanged && _rumbleCallback != nullptr) {
    _rumbleCallback(_output.strongMotor, _output.weakMotor);
  }
  if (hasLightbarChanged && _lightbarCallback != nullptr) {
    _lightbarCallback(_output.lightbarColor);
  }
  if (hasLightbarBlinkChanged && _lightbarBlinkCallback != nullptr) {
    _lightbarBlinkCallback(_output.lightbarBlink);
  }
}

// Tells whether update() should send the state of the controls now.
bool Controller::isReportDue() {
  // The unsigned difference stays correct when micros() wraps around.
  uint32_t elapsedMicros = micros() - _lastReportMicros;
  uint32_t intervalMicros = static_cast<uint32_t>(_reportIntervalMs) * 1000;
  return elapsedMicros >= intervalMicros && _usbDevice.isReadyToSend();
}

// Sends the state of the controls to the host.
void Controller::sendInputReport() {
  // The unsigned difference stays correct when micros() wraps around.
  uint32_t nowMicros = micros();
  _elapsedMicros += nowMicros - _lastReportMicros;
  _lastReportMicros = nowMicros;
  uint16_t sensorTimestamp = static_cast<uint16_t>(_elapsedMicros * SENSOR_TICKS_PER_PERIOD / SENSOR_PERIOD_MICROS);

  internal::InputReport report;
  internal::buildInputReport(_state, _reportCounter, sensorTimestamp, report);
  if (_usbDevice.sendInputReport(report)) {
    _reportCounter++;
  }
}

// Returns true once begin() has completed successfully.
bool Controller::isReady() const {
  return _isReady;
}

// Sets how often update() sends the state of the controls to the host.
void Controller::setReportInterval(uint16_t intervalMs) {
  _reportIntervalMs = intervalMs;
}

// Keeps or removes the USB serial port of the board next to the controller.
void Controller::setUsbSerialEnabled(bool isEnabled) {
  _isUsbSerialEnabled = isEnabled;
}

// Presses or releases a button.
void Controller::setButton(Button button, bool isPressed) {
  if (button >= internal::BUTTON_COUNT) {
    return;
  }

  uint16_t mask = 1u << button;
  if (isPressed) {
    _state.buttons |= mask;
  } else {
    _state.buttons &= ~mask;
  }
}

// Moves a stick.
void Controller::setStick(Stick stick, uint8_t x, uint8_t y) {
  if (stick >= internal::STICK_COUNT) {
    return;
  }

  _state.stickX[stick] = x;
  _state.stickY[stick] = y;
}

// Moves a single axis of a stick.
void Controller::setAxis(StickAxis axis, float value) {
  // In the DS4 report, Y goes from up to down: the opposite of the axis values of the library.
  switch (axis) {
    case LeftStickX:
      _state.stickX[LeftStick] = internal::toRawStickAxis(value);
      break;
    case LeftStickY:
      _state.stickY[LeftStick] = internal::toRawStickAxis(-value);
      break;
    case RightStickX:
      _state.stickX[RightStick] = internal::toRawStickAxis(value);
      break;
    case RightStickY:
      _state.stickY[RightStick] = internal::toRawStickAxis(-value);
      break;
    default:
      break;
  }
}

// Presses a trigger.
void Controller::setTrigger(Trigger trigger, uint8_t value) {
  if (trigger >= internal::TRIGGER_COUNT) {
    return;
  }

  _state.triggers[trigger] = value;
}

// Presses a trigger.
void Controller::setTriggerValue(Trigger trigger, float value) {
  setTrigger(trigger, internal::toRawTrigger(value));
}

// Puts all the controls at rest.
void Controller::releaseAll() {
  _state = internal::ControlState();
}

// Drives a button from a pin of the board.
void Controller::bind(Button button, uint8_t pin, ButtonWiring wiring) {
  if (button >= internal::BUTTON_COUNT) {
    return;
  }

  _buttonBindings[button].attach(pin, wiring);
}

// Stops driving a button from its pin, and releases it.
void Controller::unbind(Button button) {
  if (button >= internal::BUTTON_COUNT || !_buttonBindings[button].isAttached()) {
    return;
  }

  _buttonBindings[button].detach();
  setButton(button, false);
}

// Drives a trigger from a button wired to a pin of the board.
void Controller::bind(Trigger trigger, uint8_t pin, ButtonWiring wiring) {
  if (trigger >= internal::TRIGGER_COUNT) {
    return;
  }

  _triggerBindings[trigger].attachButton(pin, wiring);
}

// Drives a half of a stick axis from a button wired to a pin of the board.
void Controller::bind(StickAxis axis, AxisHalf half, uint8_t pin, ButtonWiring wiring) {
  if (axis >= internal::STICK_AXIS_COUNT || half >= internal::AXIS_HALF_COUNT) {
    return;
  }

  _stickAxisBindings[axis].attachButton(half, pin, wiring);
}

// Drives a trigger from an analog pin of the board.
void Controller::bindAnalog(Trigger trigger, uint8_t pin, const AnalogCalibration &calibration) {
  if (trigger >= internal::TRIGGER_COUNT) {
    return;
  }

  _triggerBindings[trigger].attachAnalog(pin, calibration);
}

// Drives a whole stick axis from an analog pin of the board.
void Controller::bindAnalog(StickAxis axis, uint8_t pin, const AnalogCalibration &calibration) {
  if (axis >= internal::STICK_AXIS_COUNT) {
    return;
  }

  _stickAxisBindings[axis].attachAnalog(pin, calibration);
}

// Drives a half of a stick axis from an analog pin of the board.
void Controller::bindAnalog(StickAxis axis, AxisHalf half, uint8_t pin, const AnalogCalibration &calibration) {
  if (axis >= internal::STICK_AXIS_COUNT || half >= internal::AXIS_HALF_COUNT) {
    return;
  }

  _stickAxisBindings[axis].attachAnalog(half, pin, calibration);
}

// Stops driving a trigger from its pin, and releases it.
void Controller::unbind(Trigger trigger) {
  if (trigger >= internal::TRIGGER_COUNT || !_triggerBindings[trigger].isAttached()) {
    return;
  }

  _triggerBindings[trigger].detach();
  setTrigger(trigger, TRIGGER_MIN);
}

// Stops driving a stick axis from its pins, and puts the axis at rest.
void Controller::unbind(StickAxis axis) {
  if (axis >= internal::STICK_AXIS_COUNT || !_stickAxisBindings[axis].isAttached()) {
    return;
  }

  _stickAxisBindings[axis].detach();
  setAxis(axis, 0);
}

// Sets how long the bound buttons ignore their pin after a press or a release.
void Controller::setDebounceTime(uint16_t debounceMs) {
  _debounceMs = debounceMs;
}

// Sets the MAC address reported to the host.
void Controller::setMacAddress(const MacAddress &macAddress) {
  _macAddress = macAddress;
  _macAddressSource = MacFromSketch;
}

// Gets the MAC address reported to the host.
const MacAddress &Controller::getMacAddress() const {
  return _macAddress;
}

// Tells where the MAC address comes from.
MacAddressSource Controller::getMacAddressSource() const {
  return _macAddressSource;
}

// Gets the speed of a rumble motor, as last requested by the host.
uint8_t Controller::getRumble(Motor motor) const {
  switch (motor) {
    case StrongMotor:
      return _output.strongMotor;
    case WeakMotor:
      return _output.weakMotor;
    default:
      return 0;
  }
}

// Gets the color of the lightbar, as last requested by the host.
LightbarColor Controller::getLightbarColor() const {
  return _output.lightbarColor;
}

// Gets the blinking of the lightbar, as last requested by the host.
LightbarBlink Controller::getLightbarBlink() const {
  return _output.lightbarBlink;
}

// Sets the function to call when the host changes the rumble.
void Controller::onRumble(RumbleCallback callback) {
  _rumbleCallback = callback;
}

// Sets the function to call when the host changes the color of the lightbar.
void Controller::onLightbarColor(LightbarCallback callback) {
  _lightbarCallback = callback;
}

// Sets the function to call when the host changes the blinking of the lightbar.
void Controller::onLightbarBlink(LightbarBlinkCallback callback) {
  _lightbarBlinkCallback = callback;
}

}  // namespace CtrlDS4
