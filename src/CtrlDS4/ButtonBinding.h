/**
 * @file ButtonBinding.h
 * @brief Button of the controller driven by a pin of the board.
 */

#pragma once

#include <stdint.h>

#include "Types.h"

namespace CtrlDS4 {
namespace internal {

/**
 * @brief Reads a button wired to a pin, without the bounces of its contacts.
 *
 * Mechanical contacts bounce for a few milliseconds when pressed or released, which would read as
 * several presses. A change of the pin is taken at once, then the pin is ignored for the debounce
 * time: this filters the bounces without delaying the presses.
 */
class ButtonBinding {
public:
  /**
   * @brief Starts reading a pin, and sets its mode for the wiring.
   *
   * @param pin Pin the button is wired to.
   * @param wiring How the button is wired to the pin.
   */
  void attach(uint8_t pin, ButtonWiring wiring);

  /**
   * @brief Stops reading the pin. The button reads as released.
   *
   * The mode of the pin is left as is.
   */
  void detach();

  /**
   * @brief Tells whether a pin is attached.
   *
   * @return true between attach() and detach(), false otherwise.
   */
  bool isAttached() const;

  /**
   * @brief Reads the pin, and updates the state of the button.
   *
   * @param debounceMs How long changes of the pin are ignored after a change, in milliseconds.
   * @return true if the button is pressed, false otherwise.
   */
  bool read(uint16_t debounceMs);

private:
  /// Pin the button is wired to.
  uint8_t _pin = 0;
  /// Set while the pin reads LOW when the button is pressed.
  bool _isActiveLow = true;
  /// Set between attach() and detach().
  bool _isAttached = false;
  /// State of the button after debouncing.
  bool _isPressed = false;
  /// Value of millis() when _isPressed last changed.
  uint32_t _lastChangeMs = 0;
};

}  // namespace internal
}  // namespace CtrlDS4
