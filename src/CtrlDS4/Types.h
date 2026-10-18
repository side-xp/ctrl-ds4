/**
 * @file Types.h
 * @brief Public types of the CtrlDS4 library.
 *
 * Don't include this file directly: include CtrlDS4.h, which includes it.
 */

#pragma once

#include <Arduino.h>

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
 * @brief How a button is wired to its pin, for Controller::bind().
 */
enum ButtonWiring : uint8_t {
  /// Button between the pin and GND, the most common wiring. Uses the internal pull-up resistor of
  /// the pin, so it needs no other component: the pin reads LOW while the button is pressed.
  ActiveLow,
  /// Button between the pin and 3.3V. Uses the internal pull-down resistor of the pin: the pin
  /// reads HIGH while the button is pressed.
  ActiveHigh
};

/// Default debounce time of the bound buttons, in milliseconds: longer than the bounces of most
/// buttons (see Controller::setDebounceTime()).
constexpr uint16_t DEFAULT_DEBOUNCE_MS = 10;

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
 * @brief Single axes of the analog sticks, for the controls driven one axis at a time.
 *
 * Their values go from -1 to 1, 0 being at rest: X goes from left (-1) to right (1), and Y from
 * down (-1) to up (1).
 */
enum StickAxis : uint8_t {
  LeftStickX,   ///< Horizontal axis of the left stick.
  LeftStickY,   ///< Vertical axis of the left stick.
  RightStickX,  ///< Horizontal axis of the right stick.
  RightStickY   ///< Vertical axis of the right stick.
};

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

/// Number of bytes of a MAC address.
constexpr uint8_t MAC_ADDRESS_SIZE = 6;

/**
 * @brief MAC address the controller reports to the host.
 *
 * Hosts use it to tell controllers apart, so each controller connected to the same host needs its
 * own address.
 *
 * It can be printed directly, as six hexadecimal bytes separated by colons:
 *
 * @code
 * Serial.println(controller.getMacAddress());  // Prints e.g. 02:1A:2B:3C:4D:5E
 * @endcode
 */
struct MacAddress : public Printable {
  /// Bytes of the address, in the order they are printed.
  uint8_t bytes[MAC_ADDRESS_SIZE] = {};

  /**
   * @brief Builds the address 00:00:00:00:00:00.
   */
  MacAddress() = default;

  /**
   * @brief Builds an address from its bytes, in the order they are printed.
   *
   * @code
   * CtrlDS4::MacAddress address(0x02, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E);  // 02:1A:2B:3C:4D:5E
   * @endcode
   *
   * To avoid using the address of a real device, set bit 1 of the first byte (locally
   * administered address). Keep bit 0 cleared, since it marks multicast addresses, which a device
   * can't use.
   *
   * @param byte0 First byte.
   * @param byte1 Second byte.
   * @param byte2 Third byte.
   * @param byte3 Fourth byte.
   * @param byte4 Fifth byte.
   * @param byte5 Last byte.
   */
  MacAddress(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5);

  /**
   * @brief Prints the address as six hexadecimal bytes separated by colons.
   *
   * @param output Where to print the address (e.g. Serial).
   * @return Number of characters printed.
   */
  size_t printTo(Print &output) const override;
};

/**
 * @brief Where the MAC address of a controller comes from.
 */
enum MacAddressSource : uint8_t {
  MacFromUniqueId,  ///< Derived from the factory unique ID of the board.
  MacFromSketch,    ///< Set by the sketch with Controller::setMacAddress().
  MacFallback       ///< Fixed address, used when the board has no usable unique ID.
};

/**
 * @brief Rumble motors of the controller, driven by the host.
 *
 * Each motor speed goes from 0 (stopped) to 255 (full speed).
 */
enum Motor : uint8_t {
  StrongMotor,  ///< Heavy motor in the left grip, for strong and slow rumble.
  WeakMotor     ///< Light motor in the right grip, for light and fast rumble.
};

/**
 * @brief Color of the lightbar, set by the host.
 *
 * Each component goes from 0 (off) to 255 (full brightness).
 */
struct LightbarColor {
  uint8_t red;    ///< Red component.
  uint8_t green;  ///< Green component.
  uint8_t blue;   ///< Blue component.
};

/**
 * @brief Blinking of the lightbar, set by the host.
 *
 * The lightbar shows its color for onMs, then turns off for offMs, and repeats. When both are 0,
 * the lightbar doesn't blink: it always shows its color.
 */
struct LightbarBlink {
  uint16_t onMs;   ///< How long the lightbar stays on, in milliseconds.
  uint16_t offMs;  ///< How long the lightbar stays off, in milliseconds.
};

}  // namespace CtrlDS4
