#include "CtrlTemplate.h"

// Initializes the library. Call once from setup().
bool CtrlTemplate::begin() {
  // @todo Initialize pins, peripherals and state here.
  _isReady = true;
  return _isReady;
}

// Runs the library logic. Call on every iteration of loop().
void CtrlTemplate::update() {
  if (!_isReady) {
    return;
  }

  // @todo Implement the library logic. Keep it non-blocking (no delay()).
}

// Returns true once begin() has completed successfully.
bool CtrlTemplate::isReady() const {
  return _isReady;
}
