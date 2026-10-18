/**
 * @file CtrlDS4.h
 * @brief Main header of the CtrlDS4 library.
 *
 * Emulates a DualShock 4 controller from boards with native USB, such as the RP2040 or the SAMD21.
 */

#pragma once

#include <Arduino.h>
// Also makes the build link the TinyUSB stack, which provides Serial when that stack is selected.
#include <Adafruit_TinyUSB.h>

#include "CtrlDS4/Types.h"
#include "CtrlDS4/ControlState.h"

/// Everything the CtrlDS4 library declares: the Controller class and the types of its controls.
namespace CtrlDS4 {

/**
 * @brief Emulated DS4 controller.
 *
 * Call begin() once from setup(), then update() on every iteration of loop(). Set the state of the
 * controls at any time with the setters (setButton(), setStick(), setTrigger()): update() reports
 * the latest state to the host.
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
  /* --- Lifecycle --- */

  /**
   * @brief Initializes the library.
   *
   * Call once from setup(), before any other method.
   *
   * @return true if initialization succeeded, false otherwise.
   */
  bool begin();

  /**
   * @brief Runs the library logic.
   *
   * Call on every iteration of loop(). Does nothing until begin() has succeeded.
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

private:
  /// Set by begin() once initialization has succeeded.
  bool _isReady = false;

  /// State of the controls, as set by the setters.
  internal::ControlState _state;
};

}  // namespace CtrlDS4
