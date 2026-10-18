/**
 * @file RumbleAndLightbar.ino
 * @brief Prints the rumble and lightbar changes requested by the host.
 *
 * Open the Serial Monitor, then trigger a rumble or a lightbar change from the host: for example,
 * the vibration test of a gamepad tester website in Chrome, or the controller settings of Steam.
 *
 * Replace the prints with your own outputs, such as a vibration motor driven with analogWrite(), or
 * an RGB LED.
 */

#include <CtrlDS4.h>

/// Emulated controller driven by setup() and loop().
CtrlDS4::Controller controller;

/**
 * @brief Runs once at startup.
 *
 * Registers the functions to call on changes, then initializes the controller.
 */
void setup() {
  Serial.begin(115200);

  controller.onRumble(printRumble);
  controller.onLightbarColor(printLightbarColor);
  controller.onLightbarBlink(printLightbarBlink);
  controller.begin();
}

/**
 * @brief Runs repeatedly after setup().
 *
 * Lets the controller exchange with the host, which calls the registered functions on changes.
 */
void loop() {
  controller.update();
}

/**
 * @brief Prints the new speeds of the rumble motors.
 *
 * @param strongMotor Speed of the strong motor, from 0 (stopped) to 255 (full speed).
 * @param weakMotor Speed of the weak motor, from 0 (stopped) to 255 (full speed).
 */
void printRumble(uint8_t strongMotor, uint8_t weakMotor) {
  Serial.print(F("Rumble: strong "));
  Serial.print(strongMotor);
  Serial.print(F(", weak "));
  Serial.println(weakMotor);
}

/**
 * @brief Prints the new color of the lightbar.
 *
 * @param color New color, each component from 0 (off) to 255 (full brightness).
 */
void printLightbarColor(const CtrlDS4::LightbarColor &color) {
  Serial.print(F("Lightbar: red "));
  Serial.print(color.red);
  Serial.print(F(", green "));
  Serial.print(color.green);
  Serial.print(F(", blue "));
  Serial.println(color.blue);
}

/**
 * @brief Prints the new blinking of the lightbar.
 *
 * @param blink New blinking, both durations at 0 when the lightbar doesn't blink.
 */
void printLightbarBlink(const CtrlDS4::LightbarBlink &blink) {
  Serial.print(F("Lightbar blink: on "));
  Serial.print(blink.onMs);
  Serial.print(F(" ms, off "));
  Serial.print(blink.offMs);
  Serial.println(F(" ms"));
}
