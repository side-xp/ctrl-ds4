/**
 * @file Basic.ino
 * @brief Minimal usage of CtrlDS4: initialize it once, then update it continuously.
 */

#include <CtrlDS4.h>

/// Library instance driven by setup() and loop().
CtrlDS4 ctrlDS4;

/**
 * @brief Runs once at startup.
 *
 * Opens the serial port to report errors, then initializes the library.
 */
void setup() {
  Serial.begin(9600);

  if (!ctrlDS4.begin()) {
    Serial.println(F("CtrlDS4 failed to initialize."));
  }
}

/**
 * @brief Runs repeatedly after setup().
 */
void loop() {
  ctrlDS4.update();
}
