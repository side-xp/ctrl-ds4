/**
 * @file Basic.ino
 * @brief Minimal usage of CtrlDS4: presses Cross briefly every second, without any wiring.
 */

#include <CtrlDS4.h>

/// Time between two presses of Cross, in milliseconds.
const unsigned long PRESS_INTERVAL_MS = 1000;
/// How long Cross stays pressed, in milliseconds.
const unsigned long PRESS_DURATION_MS = 100;

/// Emulated controller driven by setup() and loop().
CtrlDS4::Controller controller;

/**
 * @brief Runs once at startup.
 *
 * Initializes the controller.
 */
void setup() {
  controller.begin();
}

/**
 * @brief Runs repeatedly after setup().
 *
 * Sets the state of the controls, then lets the controller report it to the host.
 */
void loop() {
  // Pressed during the first PRESS_DURATION_MS of every PRESS_INTERVAL_MS, without blocking loop().
  bool isCrossPressed = millis() % PRESS_INTERVAL_MS < PRESS_DURATION_MS;
  controller.setButton(CtrlDS4::Cross, isCrossPressed);

  controller.update();
}
