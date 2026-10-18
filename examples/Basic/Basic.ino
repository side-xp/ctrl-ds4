/**
 * @file Basic.ino
 * @brief Minimal usage of CtrlDS4: initialize the controller once, then update it continuously.
 */

#include <CtrlDS4.h>

/// Emulated controller driven by setup() and loop().
CtrlDS4::Controller controller;

/**
 * @brief Runs once at startup.
 *
 * Opens the serial port to report errors, then initializes the controller.
 */
void setup() {
  Serial.begin(9600);

  if (!controller.begin()) {
    Serial.println(F("CtrlDS4 failed to initialize."));
  }
}

/**
 * @brief Runs repeatedly after setup().
 */
void loop() {
  controller.update();
}
