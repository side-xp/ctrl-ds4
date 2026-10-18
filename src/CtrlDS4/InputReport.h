/**
 * @file InputReport.h
 * @brief Input report sent to the host over USB, as a real DS4 does.
 *
 * The layout follows the one read by the Linux hid-playstation driver (also used by Android).
 */

#pragma once

#include <stdint.h>

#include "ControlState.h"

namespace CtrlDS4 {
namespace internal {

/// Report ID of the input report, its first byte.
constexpr uint8_t INPUT_REPORT_ID = 0x01;

/**
 * @brief Finger on the touchpad.
 */
struct __attribute__((packed)) TouchPoint {
  /// Bit 7 is set while no finger touches, bits 0 to 6 identify the touch.
  uint8_t contact;
  /// X (12 bits) then Y (12 bits) position of the finger, little-endian.
  uint8_t position[3];
};

/**
 * @brief Touchpad state at one point in time.
 */
struct __attribute__((packed)) TouchReport {
  /// Time of the touch report.
  uint8_t timestamp;
  /// The two fingers the touchpad tracks.
  TouchPoint points[2];
};

/**
 * @brief Input report of a DS4 connected over USB.
 *
 * The multi-byte fields are little-endian, as on the supported boards. Packed members must be
 * read and written directly, never through a pointer, since they may not be aligned.
 */
struct __attribute__((packed)) InputReport {
  /// Identifies the report for the host.
  uint8_t reportId;
  /// Axes of the sticks. See Stick for their range.
  uint8_t leftStickX;
  uint8_t leftStickY;
  uint8_t rightStickX;
  uint8_t rightStickY;
  /// D-pad position and face buttons, then shoulders and center buttons, then PS and Touchpad
  /// buttons with the report counter.
  uint8_t buttons[3];
  /// Analog values of the triggers. See Trigger for their range.
  uint8_t leftTrigger;
  uint8_t rightTrigger;
  /// Time of the motion data, in units of 16/3 microseconds.
  uint16_t sensorTimestamp;
  /// Not reported.
  uint8_t sensorTemperature;
  /// Angular velocity, around X, Y and Z.
  int16_t gyro[3];
  /// Acceleration, along X, Y and Z.
  int16_t accel[3];
  /// Unused.
  uint8_t reserved1[5];
  /// Battery level and cable state.
  uint8_t status[2];
  /// Unused.
  uint8_t reserved2;
  /// Number of valid entries in touchReports.
  uint8_t touchReportCount;
  /// Recent touchpad states, the first one being the latest.
  TouchReport touchReports[3];
  /// Unused.
  uint8_t reserved3[3];
};

static_assert(sizeof(InputReport) == 64, "A DS4 USB input report is 64 bytes, report ID included");

/**
 * @brief Fills an input report from the state of the controls.
 *
 * The report shows a controller plugged with a cable, with a full battery, no motion and no finger
 * on the touchpad.
 *
 * @param state State of the controls to report.
 * @param counter Number of the report. Only its 6 low bits are sent, so it can wrap freely.
 * @param sensorTimestamp Time of the report, in units of 16/3 microseconds. Wraps freely too.
 * @param report Report to fill. Its previous content is discarded.
 */
void buildInputReport(const ControlState &state, uint8_t counter, uint16_t sensorTimestamp, InputReport &report);

}  // namespace internal
}  // namespace CtrlDS4
