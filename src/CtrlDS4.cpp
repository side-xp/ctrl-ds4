#include "CtrlDS4.h"

namespace CtrlDS4 {

// Initializes the library. Call once from setup().
bool Controller::begin() {
  // @todo Initialize pins, peripherals and state here.
  _isReady = true;
  return _isReady;
}

// Runs the library logic. Call on every iteration of loop().
void Controller::update() {
  if (!_isReady) {
    return;
  }

  // @todo Implement the library logic. Keep it non-blocking (no delay()).
}

// Returns true once begin() has completed successfully.
bool Controller::isReady() const {
  return _isReady;
}

// Presses or releases a button.
void Controller::setButton(Button button, bool isPressed) {
  if (button >= internal::BUTTON_COUNT) {
    return;
  }

  uint16_t mask = 1u << button;
  if (isPressed) {
    _state.buttons |= mask;
  } else {
    _state.buttons &= ~mask;
  }
}

// Moves a stick.
void Controller::setStick(Stick stick, uint8_t x, uint8_t y) {
  if (stick >= internal::STICK_COUNT) {
    return;
  }

  _state.stickX[stick] = x;
  _state.stickY[stick] = y;
}

// Presses a trigger.
void Controller::setTrigger(Trigger trigger, uint8_t value) {
  if (trigger >= internal::TRIGGER_COUNT) {
    return;
  }

  _state.triggers[trigger] = value;
}

// Puts all the controls at rest.
void Controller::releaseAll() {
  _state = internal::ControlState();
}

}  // namespace CtrlDS4
