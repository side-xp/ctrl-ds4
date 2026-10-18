/**
 * @file CtrlDS4.h
 * @brief Main header of the CtrlDS4 library.
 *
 * Emulates a DualShock 4 controller from boards with native USB, such as the RP2040 or the SAMD21.
 */

#pragma once

#include <Arduino.h>

#include "CtrlDS4/Types.h"
#include "CtrlDS4/ControlState.h"
#include "CtrlDS4/UsbDevice.h"

/// Everything the CtrlDS4 library declares: the Controller class and the types of its controls.
namespace CtrlDS4 {

/**
 * @brief Emulated DS4 controller.
 *
 * Call begin() once from setup(), then update() on every iteration of loop(). Set the state of the
 * controls at any time with the setters (setButton(), setStick(), setTrigger()): update() reports
 * the latest state to the host.
 *
 * Declare a single Controller in a sketch, since the board presents itself as one DS4.
 *
 * @code
 * CtrlDS4::Controller controller;
 *
 * void setup() {
 *   controller.begin();
 * }
 *
 * void loop() {
 *   controller.setButton(CtrlDS4::Cross, digitalRead(2) == LOW);
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
   * Applies the rumble and lightbar changes requested by the host (calling the callbacks set with
   * onRumble(), onLightbarColor() and onLightbarBlink()), then sends the state of the controls when
   * the host is ready for it.
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
   * @param button Button to change. Ignored if it's not a Button value.
   * @param isPressed true to press the button, false to release it.
   */
  void setButton(Button button, bool isPressed);

  /**
   * @brief Moves a stick.
   *
   * @param stick Stick to move. Ignored if it's not a Stick value.
   * @param x Horizontal position, from STICK_MIN (left) to STICK_MAX (right).
   * @param y Vertical position, from STICK_MIN (up) to STICK_MAX (down).
   */
  void setStick(Stick stick, uint8_t x, uint8_t y);

  /**
   * @brief Presses a trigger.
   *
   * The trigger is also reported as a pressed button as soon as its value is above TRIGGER_MIN, as
   * a real DS4 does.
   *
   * @param trigger Trigger to press. Ignored if it's not a Trigger value.
   * @param value How far the trigger is pressed, from TRIGGER_MIN (released) to TRIGGER_MAX (fully
   * pressed).
   */
  void setTrigger(Trigger trigger, uint8_t value);

  /**
   * @brief Puts all the controls at rest: buttons released, sticks centered and triggers released.
   */
  void releaseAll();

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
