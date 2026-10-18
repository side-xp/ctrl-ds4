/**
 * @file Buttons.ino
 * @brief Drives Cross and Circle from two push buttons.
 *
 * Wiring: one button between pin CROSS_PIN and GND, another between pin CIRCLE_PIN and GND. No
 * resistor is needed: the library enables the internal pull-up resistors of the pins.
 */

#include <CtrlDS4.h>

/// Pin of the button driving Cross.
const uint8_t CROSS_PIN = 2;
/// Pin of the button driving Circle.
const uint8_t CIRCLE_PIN = 3;

/// Emulated controller driven by setup() and loop().
CtrlDS4::Controller controller;

/**
 * @brief Runs once at startup.
 *
 * Binds the buttons to their pins, then initializes the controller.
 */
void setup() {
  controller.bind(CtrlDS4::Cross, CROSS_PIN);
  controller.bind(CtrlDS4::Circle, CIRCLE_PIN);
  controller.begin();
}

/**
 * @brief Runs repeatedly after setup().
 *
 * Lets the controller read the buttons and report them to the host.
 */
void loop() {
  controller.update();
}
