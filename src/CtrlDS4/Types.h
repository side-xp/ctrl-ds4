/**
 * @file Types.h
 * @brief Public types of the CtrlDS4 library.
 *
 * Don't include this file directly: include CtrlDS4.h, which includes it.
 */

#pragma once

#include <stdint.h>

namespace CtrlDS4 {

/**
 * @brief Digital controls of the controller.
 *
 * The D-pad directions are buttons too, so they can be bound to pins like any other button.
 * The library converts them to the D-pad position the DS4 reports, ignoring opposite directions
 * pressed together.
 *
 * L2 and R2 are not listed: they are analog triggers (see Trigger), and the library reports them
 * as pressed from their analog value.
 *
 * The enums of the library are unscoped on purpose, so their values read as CtrlDS4::Cross rather
 * than CtrlDS4::Button::Cross. They share the namespace, so their value names must stay unique
 * across all of them.
 */
enum Button : uint8_t {
  Cross,     ///< Bottom face button.
  Circle,    ///< Right face button.
  Square,    ///< Left face button.
  Triangle,  ///< Top face button.
  L1,        ///< Left shoulder button.
  R1,        ///< Right shoulder button.
  L3,        ///< Left stick click.
  R3,        ///< Right stick click.
  Share,     ///< Share button, left of the touchpad.
  Options,   ///< Options button, right of the touchpad.
  PS,        ///< PlayStation button, below the touchpad.
  Touchpad,  ///< Touchpad click.
  DPadUp,    ///< D-pad up direction.
  DPadDown,  ///< D-pad down direction.
  DPadLeft,  ///< D-pad left direction.
  DPadRight  ///< D-pad right direction.
};

/**
 * @brief Analog sticks of the controller.
 *
 * Each axis goes from STICK_MIN to STICK_MAX, like on a real DS4: X goes from left to right, and
 * Y from up to down.
 */
enum Stick : uint8_t {
  LeftStick,  ///< Left analog stick.
  RightStick  ///< Right analog stick.
};

/// Value of a stick axis pushed fully left (X) or up (Y).
constexpr uint8_t STICK_MIN = 0;
/// Value of a stick axis at rest.
constexpr uint8_t STICK_CENTER = 128;
/// Value of a stick axis pushed fully right (X) or down (Y).
constexpr uint8_t STICK_MAX = 255;

/**
 * @brief Analog triggers of the controller.
 *
 * Each trigger goes from TRIGGER_MIN (released) to TRIGGER_MAX (fully pressed).
 */
enum Trigger : uint8_t {
  L2,  ///< Left trigger.
  R2   ///< Right trigger.
};

/// Value of a released trigger.
constexpr uint8_t TRIGGER_MIN = 0;
/// Value of a fully pressed trigger.
constexpr uint8_t TRIGGER_MAX = 255;

}  // namespace CtrlDS4
