/**
 * @file CtrlDS4.h
 * @brief Main header of the CtrlDS4 library.
 *
 * Emulates a DualShock 4 controller from boards with native USB, such as the RP2040 or the SAMD21.
 */

#pragma once

#include <Arduino.h>

#include "CtrlDS4/Types.h"
#include "CtrlDS4/AxisBinding.h"
#include "CtrlDS4/ButtonBinding.h"
#include "CtrlDS4/ControlState.h"
#include "CtrlDS4/UsbDevice.h"

/// Everything the CtrlDS4 library declares: the Controller class and the types of its controls.
namespace CtrlDS4 {

/**
 * @brief Emulated DS4 controller.
 *
 * Call begin() once from setup(), then update() on every iteration of loop(). Drive the controls
 * by binding buttons to pins with bind(), or set them from the sketch with the setters
 * (setButton(), setStick(), setTrigger()): update() reports the latest state to the host.
 *
 * Declare a single Controller in a sketch, since the board presents itself as one DS4.
 *
 * @code
 * CtrlDS4::Controller controller;
 *
 * void setup() {
 *   controller.bind(CtrlDS4::Cross, 2);
 *   controller.begin();
 * }
 *
 * void loop() {
 *   controller.update();
 * }
 * @endcode
 */
class Controller {
public:
  /**
   * @brief Function called when the host changes the rumble.
   *
   * @param strongMotor New speed of the StrongMotor.
   * @param weakMotor New speed of the WeakMotor.
   */
  using RumbleCallback = void (*)(uint8_t strongMotor, uint8_t weakMotor);

  /**
   * @brief Function called when the host changes the color of the lightbar.
   *
   * @param color New color of the lightbar.
   */
  using LightbarCallback = void (*)(const LightbarColor &color);

  /**
   * @brief Function called when the host changes the blinking of the lightbar.
   *
   * @param blink New blinking of the lightbar.
   */
  using LightbarBlinkCallback = void (*)(const LightbarBlink &blink);

  /* --- Lifecycle --- */

  /**
   * @brief Chooses the MAC address, then connects to the host as a DS4.
   *
   * Call once from setup(). It briefly disconnects the board from the host, so the host sees it
   * again as a DS4: open the Serial Monitor after begin(), or reopen it.
   *
   * @return true if the board could present itself as a DS4, false otherwise.
   */
  bool begin();

  /**
   * @brief Exchanges the latest state with the host.
   *
   * Reads the pins of the bound controls, applies the rumble and lightbar changes requested by the
   * host (calling the callbacks set with onRumble(), onLightbarColor() and onLightbarBlink()), then
   * sends the state of the controls when the host is ready for it.
   *
   * Call on every iteration of loop(), without blocking it for long: the host expects a new report
   * every few milliseconds. Does nothing until begin() has succeeded.
   */
  void update();

  /**
   * @brief Tells whether the library is ready to use.
   *
   * @return true once begin() has completed successfully, false otherwise.
   */
  bool isReady() const;

  /* --- Controls --- */

  /**
   * @brief Presses or releases a button.
   *
   * A button bound to a pin (see bind()) follows its pin instead: the next update() overrides the
   * value set here.
   *
   * @param button Button to change. Ignored if it's not a Button value.
   * @param isPressed true to press the button, false to release it.
   */
  void setButton(Button button, bool isPressed);

  /**
   * @brief Moves a stick, with the values of the DS4 report.
   *
   * See setAxis() to move a single axis with a value from -1 to 1. An axis bound to pins (see
   * bind()) follows its pins instead: the next update() overrides the value set here.
   *
   * @param stick Stick to move. Ignored if it's not a Stick value.
   * @param x Horizontal position, from STICK_MIN (left) to STICK_MAX (right).
   * @param y Vertical position, from STICK_MIN (up) to STICK_MAX (down), as in the DS4 report.
   */
  void setStick(Stick stick, uint8_t x, uint8_t y);

  /**
   * @brief Moves a single axis of a stick.
   *
   * An axis bound to pins (see bind()) follows its pins instead: the next update() overrides the
   * value set here.
   *
   * @code
   * controller.setAxis(CtrlDS4::LeftStickX, 1);     // Left stick fully right
   * controller.setAxis(CtrlDS4::LeftStickY, -0.5);  // Left stick halfway down
   * @endcode
   *
   * @param axis Axis to move. Ignored if it's not a StickAxis value.
   * @param value Position on the axis, from -1 to 1, 0 being at rest: X goes from left (-1) to
   * right (1), and Y from down (-1) to up (1). Clamped to that range.
   */
  void setAxis(StickAxis axis, float value);

  /**
   * @brief Presses a trigger, with the values of the DS4 report.
   *
   * The trigger is also reported as a pressed button as soon as its value is above TRIGGER_MIN, as
   * a real DS4 does. See setTriggerValue() to press it with a value from 0 to 1. A trigger bound to
   * a pin (see bind()) follows its pin instead: the next update() overrides the value set here.
   *
   * @param trigger Trigger to press. Ignored if it's not a Trigger value.
   * @param value How far the trigger is pressed, from TRIGGER_MIN (released) to TRIGGER_MAX (fully
   * pressed).
   */
  void setTrigger(Trigger trigger, uint8_t value);

  /**
   * @brief Presses a trigger.
   *
   * The trigger is also reported as a pressed button as soon as its value is above 0, as a real
   * DS4 does. A trigger bound to a pin (see bind()) follows its pin instead: the next update()
   * overrides the value set here.
   *
   * @code
   * controller.setTriggerValue(CtrlDS4::R2, 0.5);  // R2 pressed halfway
   * @endcode
   *
   * @param trigger Trigger to press. Ignored if it's not a Trigger value.
   * @param value How far the trigger is pressed, from 0 (released) to 1 (fully pressed). Clamped
   * to that range.
   */
  void setTriggerValue(Trigger trigger, float value);

  /**
   * @brief Puts all the controls at rest: buttons released, sticks centered and triggers released.
   */
  void releaseAll();

  /* --- Pin bindings --- */

  /**
   * @brief Drives a button from a pin of the board.
   *
   * Sets the mode of the pin for the wiring, then update() reads it and presses or releases the
   * button, filtering the bounces of the contacts (see setDebounceTime()).
   *
   * @code
   * controller.bind(CtrlDS4::Cross, 2);                       // Button between pin 2 and GND
   * controller.bind(CtrlDS4::Circle, 3, CtrlDS4::ActiveHigh);  // Button between pin 3 and 3.3V
   * @endcode
   *
   * @param button Button to drive. Ignored if it's not a Button value. Binding it again replaces
   * its previous pin.
   * @param pin Pin the button is wired to.
   * @param wiring How the button is wired to the pin.
   */
  void bind(Button button, uint8_t pin, ButtonWiring wiring = ActiveLow);

  /**
   * @brief Stops driving a button from its pin, and releases it.
   *
   * The mode of the pin is left as is.
   *
   * @param button Button to stop driving. Ignored if it's not a Button value, or not bound.
   */
  void unbind(Button button);

  /**
   * @brief Drives a trigger from a button wired to a pin of the board.
   *
   * The trigger is fully pressed while the button is pressed, and released otherwise.
   *
   * @code
   * controller.bind(CtrlDS4::R2, 4);  // Button between pin 4 and GND
   * @endcode
   *
   * @param trigger Trigger to drive. Ignored if it's not a Trigger value. Binding it again
   * replaces its previous pin.
   * @param pin Pin the button is wired to.
   * @param wiring How the button is wired to the pin.
   */
  void bind(Trigger trigger, uint8_t pin, ButtonWiring wiring = ActiveLow);

  /**
   * @brief Drives a half of a stick axis from a button wired to a pin of the board.
   *
   * The axis is pushed to the end of that half while the button is pressed. Bind a button to each
   * half to drive the whole axis: pressed together, they cancel each other.
   *
   * @code
   * controller.bind(CtrlDS4::LeftStickY, CtrlDS4::Positive, 5);  // Up while pressed
   * controller.bind(CtrlDS4::LeftStickY, CtrlDS4::Negative, 6);  // Down while pressed
   * @endcode
   *
   * @param axis Axis to drive. Ignored if it's not a StickAxis value. Replaces the analog pin of
   * the whole axis, if any (see bindAnalog()).
   * @param half Half of the axis the button pushes to. Ignored if it's not an AxisHalf value.
   * Binding it again replaces its previous pin.
   * @param pin Pin the button is wired to.
   * @param wiring How the button is wired to the pin.
   */
  void bind(StickAxis axis, AxisHalf half, uint8_t pin, ButtonWiring wiring = ActiveLow);

  /**
   * @brief Drives a trigger from an analog pin of the board.
   *
   * The trigger follows the reading of the pin: released at AnalogCalibration::minReading, fully
   * pressed at AnalogCalibration::maxReading. The pin is read on the update() calls that send a
   * report to the host.
   *
   * @code
   * controller.bindAnalog(CtrlDS4::R2, A0);                                     // Whole range
   * controller.bindAnalog(CtrlDS4::L2, A1, CtrlDS4::AnalogCalibration(80, 950));  // Calibrated
   * @endcode
   *
   * @param trigger Trigger to drive. Ignored if it's not a Trigger value. Binding it again
   * replaces its previous pin.
   * @param pin Analog pin to read.
   * @param calibration Readings that match the released and fully pressed trigger, and deadzone
   * near the released trigger. The whole analogRead() range by default.
   */
  void bindAnalog(Trigger trigger, uint8_t pin, const AnalogCalibration &calibration = AnalogCalibration());

  /**
   * @brief Drives a whole stick axis from an analog pin of the board, e.g. a potentiometer.
   *
   * The axis follows the reading of the pin: left (X) or down (Y) at AnalogCalibration::minReading,
   * right (X) or up (Y) at AnalogCalibration::maxReading, and centered halfway. The pin is read on
   * the update() calls that send a report to the host.
   *
   * @code
   * controller.bindAnalog(CtrlDS4::LeftStickX, A0, CtrlDS4::AnalogCalibration(0, 1023, 0.05));
   * controller.bindAnalog(CtrlDS4::LeftStickY, A1, CtrlDS4::AnalogCalibration(1023, 0, 0.05));  // Inverted
   * @endcode
   *
   * @param axis Axis to drive. Ignored if it's not a StickAxis value. Replaces the previous pins of
   * the axis, including the ones of its halves.
   * @param pin Analog pin to read.
   * @param calibration Readings that match both ends of the axis, and deadzone around the center.
   * The whole analogRead() range by default.
   */
  void bindAnalog(StickAxis axis, uint8_t pin, const AnalogCalibration &calibration = AnalogCalibration());

  /**
   * @brief Drives a half of a stick axis from an analog pin of the board, e.g. a pressure sensor.
   *
   * The axis follows the reading of the pin, from rest at AnalogCalibration::minReading to the end
   * of the half at AnalogCalibration::maxReading. Bind the other half to drive the whole axis: the
   * axis value is the positive half minus the negative half. The pin is read on the update() calls
   * that send a report to the host.
   *
   * @code
   * controller.bindAnalog(CtrlDS4::RightStickX, CtrlDS4::Positive, A0);  // Pushes right
   * controller.bindAnalog(CtrlDS4::RightStickX, CtrlDS4::Negative, A1);  // Pushes left
   * @endcode
   *
   * @param axis Axis to drive. Ignored if it's not a StickAxis value. Replaces the analog pin of
   * the whole axis, if any.
   * @param half Half of the axis the pin drives. Ignored if it's not an AxisHalf value. Binding it
   * again replaces its previous pin.
   * @param pin Analog pin to read.
   * @param calibration Readings that match rest and the end of the half, and deadzone near rest.
   * The whole analogRead() range by default.
   */
  void bindAnalog(StickAxis axis, AxisHalf half, uint8_t pin, const AnalogCalibration &calibration = AnalogCalibration());

  /**
   * @brief Stops driving a trigger from its pin, and releases it.
   *
   * The mode of the pin is left as is.
   *
   * @param trigger Trigger to stop driving. Ignored if it's not a Trigger value, or not bound.
   */
  void unbind(Trigger trigger);

  /**
   * @brief Stops driving a stick axis from its pins (whole axis and both halves), and puts the axis
   * at rest.
   *
   * The mode of the pins is left as is.
   *
   * @param axis Axis to stop driving. Ignored if it's not a StickAxis value, or not bound.
   */
  void unbind(StickAxis axis);

  /**
   * @brief Sets how long the bound buttons ignore their pin after a press or a release.
   *
   * Applies to all the buttons wired to pins, including the ones driving a trigger or a stick axis.
   *
   * Mechanical buttons bounce for a few milliseconds, which would read as several presses. Presses
   * are still reported at once: this only limits how fast a button can be pressed again.
   *
   * @param debounceMs Duration in milliseconds, DEFAULT_DEBOUNCE_MS until set. Raise it if a button
   * reports double presses, lower it for very fast repeated presses. 0 disables the filter.
   */
  void setDebounceTime(uint16_t debounceMs);

  /* --- MAC address --- */

  /**
   * @brief Sets the MAC address reported to the host, instead of the one begin() chooses.
   *
   * Call it before begin(), since the host reads the address when the controller connects.
   *
   * @param macAddress Address to report. See MacAddress for the bits to set and clear.
   */
  void setMacAddress(const MacAddress &macAddress);

  /**
   * @brief Gets the MAC address reported to the host.
   *
   * Unless set with setMacAddress(), it's chosen by begin(): derived from the factory unique ID of
   * the board, or a fixed fallback address if the board has none.
   *
   * @return The MAC address. Only meaningful once begin() has been called, or setMacAddress().
   */
  const MacAddress &getMacAddress() const;

  /**
   * @brief Tells where the MAC address comes from.
   *
   * Use it to warn when the board has no usable unique ID, since the fallback address is the same
   * on all those boards.
   *
   * @return Where the address returned by getMacAddress() comes from.
   */
  MacAddressSource getMacAddressSource() const;

  /* --- Rumble and lightbar --- */

  /**
   * @brief Gets the speed of a rumble motor, as last requested by the host.
   *
   * @param motor Motor to read.
   * @return Speed of the motor, from 0 (stopped) to 255 (full speed). 0 if it's not a Motor value.
   */
  uint8_t getRumble(Motor motor) const;

  /**
   * @brief Gets the color of the lightbar, as last requested by the host.
   *
   * @return Color of the lightbar. Off until the host sets it.
   */
  LightbarColor getLightbarColor() const;

  /**
   * @brief Gets the blinking of the lightbar, as last requested by the host.
   *
   * @return Blinking of the lightbar. Not blinking until the host sets it.
   */
  LightbarBlink getLightbarBlink() const;

  /**
   * @brief Sets the function to call when the host changes the rumble.
   *
   * The function is called from update(), so it can safely drive pins or print to Serial.
   *
   * @param callback Function to call, or nullptr to stop calling one.
   */
  void onRumble(RumbleCallback callback);

  /**
   * @brief Sets the function to call when the host changes the color of the lightbar.
   *
   * The function is called from update(), so it can safely drive pins or print to Serial.
   *
   * @param callback Function to call, or nullptr to stop calling one.
   */
  void onLightbarColor(LightbarCallback callback);

  /**
   * @brief Sets the function to call when the host changes the blinking of the lightbar.
   *
   * The function is called from update(), so it can safely drive pins or print to Serial.
   *
   * @param callback Function to call, or nullptr to stop calling one.
   */
  void onLightbarBlink(LightbarBlinkCallback callback);

private:
  /**
   * @brief Sets the bound buttons, triggers and stick axes from their pins.
   *
   * @param shouldReadAnalog true to read the analog pins, false to reuse their last values.
   */
  void readBindings(bool shouldReadAnalog);

  /**
   * @brief Applies the rumble and lightbar changes requested by the host, if any.
   *
   * Calls the callbacks of the parts that changed.
   */
  void applyHostOutput();

  /**
   * @brief Sends the state of the controls to the host, if it's ready for it.
   */
  void sendInputReport();

  /// Set by begin() once initialization has succeeded.
  bool _isReady = false;

  /// State of the controls, as set by the setters.
  internal::ControlState _state;

  /// Pin binding of each button, indexed by the Button value.
  internal::ButtonBinding _buttonBindings[internal::BUTTON_COUNT];
  /// Pin binding of each trigger, indexed by the Trigger value.
  internal::AxisHalfInput _triggerBindings[internal::TRIGGER_COUNT];
  /// Pin bindings of each stick axis, indexed by the StickAxis value.
  internal::StickAxisBinding _stickAxisBindings[internal::STICK_AXIS_COUNT];
  /// How long a bound button ignores its pin after a change, in milliseconds.
  uint16_t _debounceMs = DEFAULT_DEBOUNCE_MS;

  /// MAC address reported to the host.
  MacAddress _macAddress;
  /// Where _macAddress comes from.
  MacAddressSource _macAddressSource = MacFallback;

  /// USB connection to the host.
  internal::UsbDevice _usbDevice;
  /// Number of the next input report, which the host uses to detect lost reports.
  uint8_t _reportCounter = 0;
  /// Value of micros() when the last input report was built.
  uint32_t _lastReportMicros = 0;
  /// Time since begin(), in microseconds, as of the last input report. Never wraps in practice.
  uint64_t _elapsedMicros = 0;

  /// Rumble and lightbar state, as last requested by the host.
  internal::OutputState _output;
  /// Called when the host changes the rumble, if set.
  RumbleCallback _rumbleCallback = nullptr;
  /// Called when the host changes the color of the lightbar, if set.
  LightbarCallback _lightbarCallback = nullptr;
  /// Called when the host changes the blinking of the lightbar, if set.
  LightbarBlinkCallback _lightbarBlinkCallback = nullptr;
};

}  // namespace CtrlDS4
