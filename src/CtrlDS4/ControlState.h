/**
 * @file ControlState.h
 * @brief State of the controls of an emulated controller.
 */

#pragma once

#include <stdint.h>

#include "Types.h"

namespace CtrlDS4 {
namespace internal {

/// Number of Button values (DPadRight is the last one).
constexpr uint8_t BUTTON_COUNT = DPadRight + 1;
/// Number of Stick values (RightStick is the last one).
constexpr uint8_t STICK_COUNT = RightStick + 1;
/// Number of Trigger values (R2 is the last one).
constexpr uint8_t TRIGGER_COUNT = R2 + 1;

/**
 * @brief State of all the controls, as set by the sketch.
 *
 * A default-constructed state is the controller at rest: no button pressed, sticks centered and
 * triggers released.
 */
struct ControlState {
  /// One bit per button, indexed by the Button value. Set when the button is pressed.
  uint16_t buttons = 0;
  /// X axis of each stick, indexed by the Stick value.
  uint8_t stickX[STICK_COUNT] = { STICK_CENTER, STICK_CENTER };
  /// Y axis of each stick, indexed by the Stick value.
  uint8_t stickY[STICK_COUNT] = { STICK_CENTER, STICK_CENTER };
  /// Value of each trigger, indexed by the Trigger value.
  uint8_t triggers[TRIGGER_COUNT] = { TRIGGER_MIN, TRIGGER_MIN };
};

static_assert(BUTTON_COUNT <= sizeof(ControlState::buttons) * 8, "ControlState::buttons needs one bit per button");

}  // namespace internal
}  // namespace CtrlDS4
