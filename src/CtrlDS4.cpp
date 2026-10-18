#include "CtrlDS4.h"

#include "CtrlDS4/UniqueId.h"

namespace CtrlDS4 {

namespace {

/**
 * @brief Builds the MAC address used when the board has no usable unique ID.
 *
 * It's locally administered, like the addresses derived from a unique ID, so it can't match the
 * address of a real device.
 *
 * @return The fallback MAC address.
 */
MacAddress makeFallbackMacAddress() {
  return MacAddress(0x02, 0x00, 0x00, 0x00, 0x00, 0x01);
}

}  // namespace

// Initializes the library. Call once from setup().
bool Controller::begin() {
  if (_macAddressSource != MacFromSketch) {
    uint8_t uniqueId[internal::UNIQUE_ID_MAX_SIZE];
    uint8_t uniqueIdSize = internal::readUniqueId(uniqueId);
    if (uniqueIdSize > 0) {
      _macAddress = internal::makeMacAddress(uniqueId, uniqueIdSize);
      _macAddressSource = MacFromUniqueId;
    } else {
      _macAddress = makeFallbackMacAddress();
      _macAddressSource = MacFallback;
    }
  }

  // @todo Initialize the USB device here.
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

// Sets the MAC address reported to the host.
void Controller::setMacAddress(const MacAddress &macAddress) {
  _macAddress = macAddress;
  _macAddressSource = MacFromSketch;
}

// Gets the MAC address reported to the host.
const MacAddress &Controller::getMacAddress() const {
  return _macAddress;
}

// Tells where the MAC address comes from.
MacAddressSource Controller::getMacAddressSource() const {
  return _macAddressSource;
}

}  // namespace CtrlDS4
