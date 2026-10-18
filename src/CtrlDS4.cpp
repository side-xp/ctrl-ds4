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

}  // namespace CtrlDS4
