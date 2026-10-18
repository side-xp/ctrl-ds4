#include "CtrlDS4.h"

// Initializes the library. Call once from setup().
bool CtrlDS4::begin() {
  // @todo Initialize pins, peripherals and state here.
  _isReady = true;
  return _isReady;
}

// Runs the library logic. Call on every iteration of loop().
void CtrlDS4::update() {
  if (!_isReady) {
    return;
  }

  // @todo Implement the library logic. Keep it non-blocking (no delay()).
}

// Returns true once begin() has completed successfully.
bool CtrlDS4::isReady() const {
  return _isReady;
}
