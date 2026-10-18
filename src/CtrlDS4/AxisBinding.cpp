#include "AxisBinding.h"

#include <Arduino.h>
#include <math.h>

#include "AxisValue.h"

namespace CtrlDS4 {
namespace internal {

namespace {

/**
 * @brief Makes the values near rest read as exactly at rest.
 *
 * The values beyond the deadzone are scaled, so the ends of the range are still reached.
 *
 * @param value Value to filter, from -1 to 1, 0 being at rest.
 * @param deadzone Part of the range near rest that reads as 0, from 0 to 1.
 * @return The filtered value, from -1 to 1.
 */
float applyDeadzone(float value, float deadzone) {
  if (deadzone <= 0) {
    return value;
  }
  if (deadzone >= 1) {
    return 0;
  }

  float magnitude = fabsf(value);
  if (magnitude <= deadzone) {
    return 0;
  }

  float scaledMagnitude = (magnitude - deadzone) / (1 - deadzone);
  return value < 0 ? -scaledMagnitude : scaledMagnitude;
}

}  // namespace

// Starts reading an analog pin.
void AnalogInput::attach(uint8_t pin, const AnalogCalibration &calibration) {
  _pin = pin;
  _calibration = calibration;
  pinMode(_pin, INPUT);
}

// Reads the pin.
float AnalogInput::read() const {
  int32_t range = static_cast<int32_t>(_calibration.maxReading) - _calibration.minReading;
  if (range == 0) {
    return 0;
  }

  // A negative range (minReading above maxReading) inverts the reading.
  int32_t offset = static_cast<int32_t>(analogRead(_pin)) - _calibration.minReading;
  return clampValue(static_cast<float>(offset) / range, 0, 1);
}

// Gets the deadzone of the calibration.
float AnalogInput::getDeadzone() const {
  return _calibration.deadzone;
}

// Starts reading a button wired to a pin.
void AxisHalfInput::attachButton(uint8_t pin, ButtonWiring wiring) {
  detach();
  _button.attach(pin, wiring);
  _source = ButtonSource;
}

// Starts reading an analog pin.
void AxisHalfInput::attachAnalog(uint8_t pin, const AnalogCalibration &calibration) {
  detach();
  _analog.attach(pin, calibration);
  _source = AnalogSource;
}

// Stops reading the pin.
void AxisHalfInput::detach() {
  _button.detach();
  _source = NoSource;
  _analogValue = 0;
}

// Tells whether a pin is attached.
bool AxisHalfInput::isAttached() const {
  return _source != NoSource;
}

// Reads the pin, and updates the value.
float AxisHalfInput::read(uint16_t debounceMs, bool shouldReadAnalog) {
  switch (_source) {
    case ButtonSource:
      return _button.read(debounceMs) ? 1 : 0;
    case AnalogSource:
      if (shouldReadAnalog) {
        _analogValue = applyDeadzone(_analog.read(), _analog.getDeadzone());
      }
      return _analogValue;
    default:
      return 0;
  }
}

// Drives a half of the axis from a button wired to a pin.
void StickAxisBinding::attachButton(AxisHalf half, uint8_t pin, ButtonWiring wiring) {
  _isFullAxisAttached = false;
  _halves[half].attachButton(pin, wiring);
}

// Drives a half of the axis from an analog pin.
void StickAxisBinding::attachAnalog(AxisHalf half, uint8_t pin, const AnalogCalibration &calibration) {
  _isFullAxisAttached = false;
  _halves[half].attachAnalog(pin, calibration);
}

// Drives the whole axis from an analog pin.
void StickAxisBinding::attachAnalog(uint8_t pin, const AnalogCalibration &calibration) {
  detach();
  _fullAxis.attach(pin, calibration);
  _isFullAxisAttached = true;
}

// Tells whether a pin drives the axis.
bool StickAxisBinding::isAttached() const {
  return _isFullAxisAttached || _halves[Positive].isAttached() || _halves[Negative].isAttached();
}

// Stops reading all the pins of the axis.
void StickAxisBinding::detach() {
  _halves[Positive].detach();
  _halves[Negative].detach();
  _isFullAxisAttached = false;
  _fullAxisValue = 0;
}

// Reads the pins of the axis, and updates its value.
float StickAxisBinding::read(uint16_t debounceMs, bool shouldReadAnalog) {
  if (_isFullAxisAttached) {
    if (shouldReadAnalog) {
      // The reading goes from 0 to 1 across the axis: center it on 0 before applying the deadzone.
      float axisValue = _fullAxis.read() * 2 - 1;
      _fullAxisValue = applyDeadzone(axisValue, _fullAxis.getDeadzone());
    }
    return _fullAxisValue;
  }

  return _halves[Positive].read(debounceMs, shouldReadAnalog) - _halves[Negative].read(debounceMs, shouldReadAnalog);
}

}  // namespace internal
}  // namespace CtrlDS4
