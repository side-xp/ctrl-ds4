#include "ButtonBinding.h"

#include <Arduino.h>

namespace CtrlDS4 {
namespace internal {

// Starts reading a pin, and sets its mode for the wiring.
void ButtonBinding::attach(uint8_t pin, ButtonWiring wiring) {
  _pin = pin;
  _isActiveLow = wiring != ActiveHigh;
  _isAttached = true;
  _isPressed = false;
  _lastChangeMs = millis();

  pinMode(_pin, _isActiveLow ? INPUT_PULLUP : INPUT_PULLDOWN);
}

// Stops reading the pin.
void ButtonBinding::detach() {
  _isAttached = false;
  _isPressed = false;
}

// Tells whether a pin is attached.
bool ButtonBinding::isAttached() const {
  return _isAttached;
}

// Reads the pin, and updates the state of the button.
bool ButtonBinding::read(uint16_t debounceMs) {
  if (!_isAttached) {
    return false;
  }

  // The unsigned difference stays correct when millis() wraps around.
  uint32_t nowMs = millis();
  if (nowMs - _lastChangeMs < debounceMs) {
    return _isPressed;
  }

  bool isPinLow = digitalRead(_pin) == LOW;
  bool isPressed = isPinLow == _isActiveLow;
  if (isPressed != _isPressed) {
    _isPressed = isPressed;
    _lastChangeMs = nowMs;
  }
  return _isPressed;
}

}  // namespace internal
}  // namespace CtrlDS4
