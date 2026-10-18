#include "AxisBinding.h"

namespace CtrlDS4 {
namespace internal {

// Starts reading a button wired to a pin.
void AxisHalfInput::attachButton(uint8_t pin, ButtonWiring wiring) {
  _button.attach(pin, wiring);
}

// Stops reading the pin.
void AxisHalfInput::detach() {
  _button.detach();
}

// Tells whether a pin is attached.
bool AxisHalfInput::isAttached() const {
  return _button.isAttached();
}

// Reads the pin, and updates the value.
float AxisHalfInput::read(uint16_t debounceMs) {
  return _button.read(debounceMs) ? 1 : 0;
}

// Gets the input of a half of the axis.
AxisHalfInput &StickAxisBinding::getHalf(AxisHalf half) {
  return _halves[half];
}

// Tells whether a pin is attached to any half of the axis.
bool StickAxisBinding::isAttached() const {
  return _halves[Positive].isAttached() || _halves[Negative].isAttached();
}

// Stops reading the pins of both halves.
void StickAxisBinding::detach() {
  _halves[Positive].detach();
  _halves[Negative].detach();
}

// Reads the pins of both halves, and updates the value of the axis.
float StickAxisBinding::read(uint16_t debounceMs) {
  return _halves[Positive].read(debounceMs) - _halves[Negative].read(debounceMs);
}

}  // namespace internal
}  // namespace CtrlDS4
