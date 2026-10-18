/**
 * @file TouchPad.h
 * @brief Capacitive touch pad driving a button, read by the touch controller of the SAMD21.
 *
 * Declared outside TouchPads.ino on purpose: the Arduino IDE generates prototypes for the functions
 * of a sketch, and mistakes the constructors declared there for functions, which breaks the build.
 */

#pragma once

#include <Adafruit_FreeTouch.h>
#include <CtrlDS4.h>

/// Readings of an untouched pad averaged at startup to find its baseline.
const uint8_t TOUCH_BASELINE_SAMPLE_COUNT = 32;
/// Level above the baseline from which a released pad is touched. Pick it with the calibration
/// mode of TouchPads.ino (see IS_CALIBRATING).
const int32_t TOUCH_PRESS_DELTA = 150;
/// Level above the baseline under which a touched pad is released. The gap with TOUCH_PRESS_DELTA
/// keeps a borderline touch from flickering.
const int32_t TOUCH_RELEASE_DELTA = 100;
/// Each reading weighs 1 / 2^TOUCH_SMOOTHING_SHIFT in the level of a pad, which smooths out the
/// noise of the readings.
const uint8_t TOUCH_SMOOTHING_SHIFT = 2;

/**
 * @brief Piece of foil wired to a touch pin, driving a button of the controller.
 *
 * A finger near the foil adds capacitance, which the touch controller measures as a higher
 * reading: the pad is touched while its level is far enough above its untouched baseline.
 */
class TouchPad {
public:
  /**
   * @brief Binds a pad to a button.
   *
   * @param pin Touch pin the foil is wired to.
   * @param button Button the pad drives.
   */
  TouchPad(uint8_t pin, CtrlDS4::Button button)
    : _sensor(pin, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE), _button(button) {}

  /**
   * @brief Starts the sensor of the pad, and measures the baseline of the untouched pad.
   *
   * The baseline depends on the size of the foil and the length of its wire, so keep the fingers
   * off the pad meanwhile.
   *
   * @return true if the sensor started, false if the pin has no touch support. The pad then always
   * reads as released.
   */
  bool begin() {
    _isStarted = _sensor.begin();
    if (!_isStarted) {
      return false;
    }

    int32_t sum = 0;
    for (uint8_t i = 0; i < TOUCH_BASELINE_SAMPLE_COUNT; i++) {
      sum += _sensor.measure();
    }
    _baseline = sum / TOUCH_BASELINE_SAMPLE_COUNT;
    _level = _baseline;
    return true;
  }

  /**
   * @brief Takes a new reading of the pad, and tells whether it's touched.
   *
   * @return true while the pad is touched, false otherwise.
   */
  bool read() {
    if (!_isStarted) {
      return false;
    }

    int32_t reading = _sensor.measure();
    _level += (reading - _level) >> TOUCH_SMOOTHING_SHIFT;

    // A touched pad needs to drop lower than a released one needs to rise, so it doesn't flicker.
    int32_t threshold = _isTouched ? TOUCH_RELEASE_DELTA : TOUCH_PRESS_DELTA;
    _isTouched = getLevel() > threshold;
    return _isTouched;
  }

  /**
   * @brief Gets the button the pad drives.
   *
   * @return The button given to the constructor.
   */
  CtrlDS4::Button getButton() const {
    return _button;
  }

  /**
   * @brief Gets how far the pad is above its untouched baseline, as of the last reading.
   *
   * @return The level of the pad, compared to TOUCH_PRESS_DELTA and TOUCH_RELEASE_DELTA. 0 if the
   * sensor didn't start.
   */
  int32_t getLevel() const {
    return _level - _baseline;
  }

private:
  /// Touch controller channel of the pin.
  Adafruit_FreeTouch _sensor;
  /// Button the pad drives.
  CtrlDS4::Button _button;
  /// Set by begin() if the sensor started.
  bool _isStarted = false;
  /// Reading of the untouched pad.
  int32_t _baseline = 0;
  /// Running average of the readings.
  int32_t _level = 0;
  /// Whether the pad was touched as of the last reading.
  bool _isTouched = false;
};
