/**
 * @file AxisBinding.h
 * @brief Stick axes and triggers driven by pins of the board.
 */

#pragma once

#include <stdint.h>

#include "ButtonBinding.h"
#include "ControlState.h"
#include "Types.h"

namespace CtrlDS4 {
namespace internal {

/**
 * @brief Reads an analog pin, scaled from 0 to 1 by a calibration.
 */
class AnalogInput {
public:
  /**
   * @brief Starts reading an analog pin.
   *
   * Sets the pin as a plain input, so a pull-up or pull-down resistor set before doesn't skew the
   * readings.
   *
   * @param pin Analog pin to read.
   * @param calibration Readings that match the ends of the range.
   */
  void attach(uint8_t pin, const AnalogCalibration &calibration);

  /**
   * @brief Reads the pin.
   *
   * @return The reading, scaled from 0 (AnalogCalibration::minReading) to 1
   * (AnalogCalibration::maxReading), and clamped to that range.
   */
  float read() const;

  /**
   * @brief Gets the deadzone of the calibration.
   *
   * @return The deadzone, from 0 to 1.
   */
  float getDeadzone() const;

private:
  /// Analog pin to read.
  uint8_t _pin = 0;
  /// Readings that match the ends of the range.
  AnalogCalibration _calibration;
};

/**
 * @brief Drives a value from 0 to 1 from a pin: a trigger, or one half of a stick axis.
 *
 * A button wired to the pin drives the value to 0 or 1. An analog pin drives it anywhere from 0 to
 * 1.
 */
class AxisHalfInput {
public:
  /**
   * @brief Starts reading a button wired to a pin, and sets the mode of the pin for the wiring.
   *
   * Replaces the pin attached before, if any.
   *
   * @param pin Pin the button is wired to.
   * @param wiring How the button is wired to the pin.
   */
  void attachButton(uint8_t pin, ButtonWiring wiring);

  /**
   * @brief Starts reading an analog pin.
   *
   * Replaces the pin attached before, if any.
   *
   * @param pin Analog pin to read.
   * @param calibration Readings that match 0 and 1, and deadzone near 0.
   */
  void attachAnalog(uint8_t pin, const AnalogCalibration &calibration);

  /**
   * @brief Stops reading the pin. The value reads as 0.
   *
   * The mode of the pin is left as is.
   */
  void detach();

  /**
   * @brief Tells whether a pin is attached.
   *
   * @return true between an attach function and detach(), false otherwise.
   */
  bool isAttached() const;

  /**
   * @brief Reads the pin, and updates the value.
   *
   * @param debounceMs How long a button ignores its pin after a change, in milliseconds.
   * @param shouldReadAnalog true to read an analog pin, false to reuse its last value: reading an
   * analog pin is slow, so it's only worth it when a report is about to be sent.
   * @return The value, from 0 to 1. 0 if no pin is attached.
   */
  float read(uint16_t debounceMs, bool shouldReadAnalog);

private:
  /// Kinds of input the pin can be.
  enum Source : uint8_t {
    NoSource,      ///< No pin attached.
    ButtonSource,  ///< Button wired to the pin.
    AnalogSource   ///< Analog pin.
  };

  /// Kind of input attached.
  Source _source = NoSource;
  /// Button wired to the pin, when _source is ButtonSource.
  ButtonBinding _button;
  /// Analog pin, when _source is AnalogSource.
  AnalogInput _analog;
  /// Last value read from the analog pin, deadzone applied.
  float _analogValue = 0;
};

/**
 * @brief Drives a stick axis from pins: each half of the axis from its own input, or the whole axis
 * from a single analog pin.
 *
 * With an input per half, the axis value is the positive half minus the negative half, so two
 * buttons pressed together cancel each other.
 */
class StickAxisBinding {
public:
  /**
   * @brief Drives a half of the axis from a button wired to a pin.
   *
   * Replaces the input of that half, or the analog pin of the whole axis.
   *
   * @param half Half of the axis. Must be an AxisHalf value.
   * @param pin Pin the button is wired to.
   * @param wiring How the button is wired to the pin.
   */
  void attachButton(AxisHalf half, uint8_t pin, ButtonWiring wiring);

  /**
   * @brief Drives a half of the axis from an analog pin.
   *
   * Replaces the input of that half, or the analog pin of the whole axis.
   *
   * @param half Half of the axis. Must be an AxisHalf value.
   * @param pin Analog pin to read.
   * @param calibration Readings that match the start and the end of the half, and deadzone near
   * the start.
   */
  void attachAnalog(AxisHalf half, uint8_t pin, const AnalogCalibration &calibration);

  /**
   * @brief Drives the whole axis from an analog pin.
   *
   * Replaces the inputs of both halves.
   *
   * @param pin Analog pin to read.
   * @param calibration Readings that match both ends of the axis, and deadzone around the center.
   */
  void attachAnalog(uint8_t pin, const AnalogCalibration &calibration);

  /**
   * @brief Tells whether a pin drives the axis.
   *
   * @return true if the whole axis or at least one half has a pin attached, false otherwise.
   */
  bool isAttached() const;

  /**
   * @brief Stops reading all the pins of the axis.
   */
  void detach();

  /**
   * @brief Reads the pins of the axis, and updates its value.
   *
   * @param debounceMs How long a button ignores its pin after a change, in milliseconds.
   * @param shouldReadAnalog true to read the analog pins, false to reuse their last value.
   * @return The axis value, from -1 to 1.
   */
  float read(uint16_t debounceMs, bool shouldReadAnalog);

private:
  /// Inputs of the halves of the axis, indexed by the AxisHalf value.
  AxisHalfInput _halves[AXIS_HALF_COUNT];
  /// Analog pin driving the whole axis, when _isFullAxisAttached is set.
  AnalogInput _fullAxis;
  /// Set while an analog pin drives the whole axis.
  bool _isFullAxisAttached = false;
  /// Last value read from the analog pin of the whole axis, deadzone applied.
  float _fullAxisValue = 0;
};

}  // namespace internal
}  // namespace CtrlDS4
