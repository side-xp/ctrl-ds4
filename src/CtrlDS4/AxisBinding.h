/**
 * @file AxisBinding.h
 * @brief Stick axes and triggers driven by pins of the board.
 */

#pragma once

#include <stdint.h>

#include "ButtonBinding.h"
#include "ControlState.h"
#include "Types.h"

namespace CtrlDS4 {
namespace internal {

/**
 * @brief Drives a value from 0 to 1 from a pin: a trigger, or one half of a stick axis.
 *
 * A button wired to the pin drives the value to 0 or 1.
 */
class AxisHalfInput {
public:
  /**
   * @brief Starts reading a button wired to a pin, and sets the mode of the pin for the wiring.
   *
   * @param pin Pin the button is wired to.
   * @param wiring How the button is wired to the pin.
   */
  void attachButton(uint8_t pin, ButtonWiring wiring);

  /**
   * @brief Stops reading the pin. The value reads as 0.
   *
   * The mode of the pin is left as is.
   */
  void detach();

  /**
   * @brief Tells whether a pin is attached.
   *
   * @return true between an attach function and detach(), false otherwise.
   */
  bool isAttached() const;

  /**
   * @brief Reads the pin, and updates the value.
   *
   * @param debounceMs How long a button ignores its pin after a change, in milliseconds.
   * @return The value, from 0 to 1. 0 if no pin is attached.
   */
  float read(uint16_t debounceMs);

private:
  /// Button wired to the pin.
  ButtonBinding _button;
};

/**
 * @brief Drives a stick axis from pins, each half of the axis from its own input.
 *
 * The axis value is the positive half minus the negative half, so two buttons pressed together
 * cancel each other.
 */
class StickAxisBinding {
public:
  /**
   * @brief Gets the input of a half of the axis.
   *
   * @param half Half of the axis. Must be an AxisHalf value.
   * @return The input of that half.
   */
  AxisHalfInput &getHalf(AxisHalf half);

  /**
   * @brief Tells whether a pin is attached to any half of the axis.
   *
   * @return true if at least one half has a pin attached, false otherwise.
   */
  bool isAttached() const;

  /**
   * @brief Stops reading the pins of both halves.
   */
  void detach();

  /**
   * @brief Reads the pins of both halves, and updates the value of the axis.
   *
   * @param debounceMs How long a button ignores its pin after a change, in milliseconds.
   * @return The axis value, from -1 to 1.
   */
  float read(uint16_t debounceMs);

private:
  /// Inputs of the halves of the axis, indexed by the AxisHalf value.
  AxisHalfInput _halves[AXIS_HALF_COUNT];
};

}  // namespace internal
}  // namespace CtrlDS4
