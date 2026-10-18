#include "AxisValue.h"

#include <math.h>

#include "Types.h"

namespace CtrlDS4 {
namespace internal {

namespace {

/**
 * @brief Limits a value to a range.
 *
 * @param value Value to limit. NaN reads as 0.
 * @param min Lowest value of the range.
 * @param max Highest value of the range.
 * @return The value, limited to the range.
 */
float clampValue(float value, float min, float max) {
  if (isnan(value)) {
    return 0;
  }
  if (value < min) {
    return min;
  }
  if (value > max) {
    return max;
  }
  return value;
}

}  // namespace

// Converts a stick axis value to the format of the DS4 report.
uint8_t toRawStickAxis(float value) {
  value = clampValue(value, -1, 1);

  // STICK_CENTER is not exactly halfway between STICK_MIN and STICK_MAX, so each half of the axis
  // has its own scale, to reach both ends exactly.
  float halfRange = value >= 0
                      ? STICK_MAX - STICK_CENTER
                      : STICK_CENTER - STICK_MIN;
  return static_cast<uint8_t>(roundf(STICK_CENTER + value * halfRange));
}

// Converts a trigger value to the format of the DS4 report.
uint8_t toRawTrigger(float value) {
  value = clampValue(value, 0, 1);
  return static_cast<uint8_t>(roundf(TRIGGER_MIN + value * (TRIGGER_MAX - TRIGGER_MIN)));
}

}  // namespace internal
}  // namespace CtrlDS4
