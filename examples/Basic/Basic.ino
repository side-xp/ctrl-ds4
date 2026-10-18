/**
 * @file Basic.ino
 * @brief Minimal usage of CtrlTemplate: initialize it once, then update it continuously.
 */

#include <CtrlTemplate.h>

/// Library instance driven by setup() and loop().
CtrlTemplate ctrlTemplate;

/**
 * @brief Runs once at startup.
 *
 * Opens the serial port to report errors, then initializes the library.
 */
void setup() {
  Serial.begin(9600);

  if (!ctrlTemplate.begin()) {
    Serial.println(F("CtrlTemplate failed to initialize."));
  }
}

/**
 * @brief Runs repeatedly after setup().
 */
void loop() {
  ctrlTemplate.update();
}
