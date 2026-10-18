#include "OutputReport.h"

#include <string.h>

namespace CtrlDS4 {
namespace internal {

namespace {

/// Flag of OutputReport::validFlags telling the report changes the rumble.
constexpr uint8_t VALID_FLAG_RUMBLE = 0x01;
/// Flag of OutputReport::validFlags telling the report changes the lightbar color.
constexpr uint8_t VALID_FLAG_LIGHTBAR = 0x02;
/// Flag of OutputReport::validFlags telling the report changes the lightbar blinking.
constexpr uint8_t VALID_FLAG_LIGHTBAR_BLINK = 0x04;

/// Duration of one unit of OutputReport::lightbarBlinkOn and lightbarBlinkOff, in milliseconds.
constexpr uint16_t BLINK_UNIT_MS = 10;

/**
 * @brief Content of the output report of a DS4 connected over USB, after its report ID.
 *
 * The layout follows the one written by the Linux hid-playstation driver.
 */
struct __attribute__((packed)) OutputReport {
  /// Parts of the controller the report changes (see the VALID_FLAG_ constants).
  uint8_t validFlags;
  /// Unused.
  uint8_t reserved1[2];
  /// Speed of the WeakMotor.
  uint8_t weakMotor;
  /// Speed of the StrongMotor.
  uint8_t strongMotor;
  /// Color of the lightbar.
  uint8_t lightbarRed;
  uint8_t lightbarGreen;
  uint8_t lightbarBlue;
  /// Blinking of the lightbar, in units of BLINK_UNIT_MS.
  uint8_t lightbarBlinkOn;
  uint8_t lightbarBlinkOff;
  /// Unused.
  uint8_t reserved2[21];
};

static_assert(sizeof(OutputReport) == 31, "Must match the Report Count of report 5 in HID_REPORT_DESCRIPTOR");

}  // namespace

// Applies an output report of the host to a state.
bool applyOutputReport(const uint8_t *report, uint16_t size, OutputState &state) {
  OutputReport outputReport;
  if (size < sizeof(outputReport)) {
    return false;
  }
  memcpy(&outputReport, report, sizeof(outputReport));

  if (outputReport.validFlags & VALID_FLAG_RUMBLE) {
    state.strongMotor = outputReport.strongMotor;
    state.weakMotor = outputReport.weakMotor;
  }
  if (outputReport.validFlags & VALID_FLAG_LIGHTBAR) {
    state.lightbarColor.red = outputReport.lightbarRed;
    state.lightbarColor.green = outputReport.lightbarGreen;
    state.lightbarColor.blue = outputReport.lightbarBlue;
  }
  if (outputReport.validFlags & VALID_FLAG_LIGHTBAR_BLINK) {
    state.lightbarBlink.onMs = outputReport.lightbarBlinkOn * BLINK_UNIT_MS;
    state.lightbarBlink.offMs = outputReport.lightbarBlinkOff * BLINK_UNIT_MS;
  }
  return true;
}

}  // namespace internal
}  // namespace CtrlDS4
