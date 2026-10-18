/**
 * @file TouchPads.ino
 * @brief Turns pieces of foil into buttons of the controller, with the touch controller of the SAMD21.
 *
 * Needs a SAMD21 board, such as the Seeed XIAO SAMD21, and the Adafruit FreeTouch library (install
 * it from the Library Manager).
 *
 * Wire a piece of aluminium foil or copper tape straight to each touch pin of pads (A0, A1, A6, A7,
 * A8, A9 and A10 on the XIAO SAMD21), with no resistor. Keep the fingers off the pads while the
 * board starts: it measures their untouched level then. If a pad reacts too easily, or not at all,
 * tune the pads with the calibration mode (see IS_CALIBRATING).
 *
 * The onboard LED lights up while the host requests rumble.
 */

#if !defined(__SAMD21__)
#error "TouchPads needs a SAMD21 board, whose touch controller reads the pads."
#endif

#include <CtrlDS4.h>

#include "TouchPad.h"

/// Set to true to tune the pads: keeps the USB serial port, and prints how far each pad is above
/// its untouched level for the Serial Plotter, next to the thresholds of TouchPad.h. Set it back to
/// false once done, since some hosts refuse a controller with a serial port.
const bool IS_CALIBRATING = false;
/// Time between two prints of the calibration mode, in milliseconds.
const unsigned long CALIBRATION_PRINT_INTERVAL_MS = 50;

/// Time between two readings of the pads, in milliseconds.
const unsigned long PAD_READ_INTERVAL_MS = 1;

/// Level of LED_BUILTIN that turns the LED on: the LED of the XIAO SAMD21 is wired active low.
/// Swap both levels for a board whose LED is wired active high.
const uint8_t LED_ON_LEVEL = LOW;
/// Level of LED_BUILTIN that turns the LED off.
const uint8_t LED_OFF_LEVEL = HIGH;

/// Touch pads and the buttons they drive.
TouchPad pads[] = {
  TouchPad(A0, CtrlDS4::Cross),
  TouchPad(A1, CtrlDS4::Circle),
  TouchPad(A6, CtrlDS4::DPadUp),
  TouchPad(A7, CtrlDS4::DPadDown),
};

/// Emulated controller driven by setup() and loop().
CtrlDS4::Controller controller;

/// Value of millis() at the last reading of the pads.
unsigned long lastReadMs = 0;
/// Value of millis() at the last print of the calibration mode.
unsigned long lastCalibrationPrintMs = 0;

/**
 * @brief Runs once at startup.
 *
 * Measures the untouched pads, then initializes the controller.
 */
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LED_OFF_LEVEL);

  if (IS_CALIBRATING) {
    Serial.begin(115200);
    controller.setUsbSerialEnabled(true);
  }

  for (TouchPad &pad : pads) {
    pad.begin();
  }

  controller.onRumble(showRumble);
  controller.begin();
}

/**
 * @brief Runs repeatedly after setup().
 *
 * Reads the pads at a steady rate, then lets the controller report their state to the host.
 */
void loop() {
  unsigned long nowMs = millis();
  if (nowMs - lastReadMs >= PAD_READ_INTERVAL_MS) {
    lastReadMs = nowMs;
    readPads();
  }

  if (IS_CALIBRATING && nowMs - lastCalibrationPrintMs >= CALIBRATION_PRINT_INTERVAL_MS) {
    lastCalibrationPrintMs = nowMs;
    printPadLevels();
  }

  controller.update();
}

/**
 * @brief Presses or releases the button of each pad, depending on whether it's touched.
 */
void readPads() {
  for (TouchPad &pad : pads) {
    controller.setButton(pad.getButton(), pad.read());
  }
}

/**
 * @brief Prints the level of each pad and the thresholds on one line, as the Serial Plotter expects.
 *
 * A pad is touched once its level rises above the press threshold, and released once it drops
 * under the release threshold.
 */
void printPadLevels() {
  Serial.print(F("Press:"));
  Serial.print(TOUCH_PRESS_DELTA);
  Serial.print(F(" Release:"));
  Serial.print(TOUCH_RELEASE_DELTA);

  uint8_t padNumber = 1;
  for (const TouchPad &pad : pads) {
    Serial.print(F(" Pad"));
    Serial.print(padNumber++);
    Serial.print(':');
    Serial.print(pad.getLevel());
  }
  Serial.println();
}

/**
 * @brief Lights up the onboard LED while the host requests rumble. Called by the controller.
 *
 * @param strongMotor Speed of the strong motor requested by the host.
 * @param weakMotor Speed of the weak motor requested by the host.
 */
void showRumble(uint8_t strongMotor, uint8_t weakMotor) {
  bool isRumbling = strongMotor > 0 || weakMotor > 0;
  digitalWrite(LED_BUILTIN, isRumbling ? LED_ON_LEVEL : LED_OFF_LEVEL);
}
