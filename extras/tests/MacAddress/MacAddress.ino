/**
 * @file MacAddress.ino
 * @brief Test sketch: prints the MAC address of the controller, and where it comes from.
 *
 * Open the Serial Monitor: the address is printed each time it opens. Upload this sketch to several
 * boards to check that each one gets its own address, which stays the same across resets.
 *
 * This is a hardware check for the development of the library, not an example: sketches don't need
 * to handle the MAC address, since Controller::begin() chooses it.
 */

#include <CtrlDS4.h>

/// Emulated controller driven by setup() and loop().
CtrlDS4::Controller controller;

/// Whether the Serial Monitor was open on the previous loop(), to detect when it opens.
bool wasSerialOpen = false;

/**
 * @brief Runs once at startup.
 *
 * Opens the serial port, then initializes the controller, which chooses its MAC address.
 */
void setup() {
  Serial.begin(115200);
  controller.begin();
}

/**
 * @brief Runs repeatedly after setup().
 *
 * Prints the MAC address when the Serial Monitor opens.
 */
void loop() {
  // Over USB, Serial is true while a serial monitor has the port open.
  bool isSerialOpen = static_cast<bool>(Serial);
  if (isSerialOpen && !wasSerialOpen) {
    printMacAddress();
  }
  wasSerialOpen = isSerialOpen;

  controller.update();
}

/**
 * @brief Prints the MAC address of the controller and where it comes from.
 */
void printMacAddress() {
  Serial.print(F("MAC address: "));
  Serial.println(controller.getMacAddress());

  Serial.print(F("Source: "));
  switch (controller.getMacAddressSource()) {
    case CtrlDS4::MacFromUniqueId:
      Serial.println(F("unique ID of the board"));
      break;
    case CtrlDS4::MacFromSketch:
      Serial.println(F("set by the sketch"));
      break;
    case CtrlDS4::MacFallback:
      Serial.println(F("fallback (this board has no usable unique ID)"));
      break;
  }
}
