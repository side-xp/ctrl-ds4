/**
 * @file AxisValue.h
 * @brief Conversions from the -1 to 1 values of the library to the values of the DS4 report.
 */

#pragma once

#include <stdint.h>

namespace CtrlDS4 {
namespace internal {

/**
 * @brief Limits a value to a range.
 *
 * @param value Value to limit. NaN reads as 0.
 * @param min Lowest value of the range.
 * @param max Highest value of the range.
 * @return The value, limited to the range.
 */
float clampValue(float value, float min, float max);

/**
 * @brief Converts a stick axis value to the format of the DS4 report.
 *
 * @param value Axis value, from -1 to 1, 0 being at rest. Clamped to that range, and NaN reads
 * as 0.
 * @return The axis value, from STICK_MIN (-1) to STICK_MAX (1), STICK_CENTER being at rest.
 */
uint8_t toRawStickAxis(float value);

/**
 * @brief Converts a trigger value to the format of the DS4 report.
 *
 * @param value Trigger value, from 0 (released) to 1 (fully pressed). Clamped to that range, and
 * NaN reads as 0.
 * @return The trigger value, from TRIGGER_MIN to TRIGGER_MAX.
 */
uint8_t toRawTrigger(float value);

}  // namespace internal
}  // namespace CtrlDS4
