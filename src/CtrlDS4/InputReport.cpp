#include "InputReport.h"

#include <string.h>

namespace CtrlDS4 {
namespace internal {

namespace {

/// Position of the report counter in buttons[2], above the PS and Touchpad bits.
constexpr uint8_t COUNTER_SHIFT = 2;
/// Bits of the counter kept in the report.
constexpr uint8_t COUNTER_MASK = 0x3F;

/// Flag of status[0] telling the host a cable is connected.
constexpr uint8_t STATUS_CABLE_CONNECTED = 0x10;
/// Battery level of status[0] meaning "fully charged", while a cable is connected.
constexpr uint8_t STATUS_BATTERY_FULL = 11;

/// Flag of TouchPoint::contact telling no finger touches.
constexpr uint8_t TOUCH_POINT_INACTIVE = 0x80;

/// Values of the D-pad position, in the low half of buttons[0].
enum DPadPosition : uint8_t {
  DPadNorth,      ///< Up.
  DPadNorthEast,  ///< Up and right.
  DPadEast,       ///< Right.
  DPadSouthEast,  ///< Down and right.
  DPadSouth,      ///< Down.
  DPadSouthWest,  ///< Down and left.
  DPadWest,       ///< Left.
  DPadNorthWest,  ///< Up and left.
  DPadReleased    ///< No direction.
};

/// Where a button is stored in InputReport::buttons.
struct ButtonBit {
  Button button;      ///< Button stored at that location.
  uint8_t byteIndex;  ///< Index in InputReport::buttons.
  uint8_t bitIndex;   ///< Index of the bit in that byte.
};

/// Location of each button in the report. The D-pad directions are reported as a DPadPosition instead.
const ButtonBit BUTTON_BITS[] = {
  { Square, 0, 4 },
  { Cross, 0, 5 },
  { Circle, 0, 6 },
  { Triangle, 0, 7 },
  { L1, 1, 0 },
  { R1, 1, 1 },
  { Share, 1, 4 },
  { Options, 1, 5 },
  { L3, 1, 6 },
  { R3, 1, 7 },
  { PS, 2, 0 },
  { Touchpad, 2, 1 },
};

/// Location of the digital bit of each trigger in InputReport::buttons[1], indexed by the Trigger value.
const uint8_t TRIGGER_BIT_INDEXES[TRIGGER_COUNT] = { 2, 3 };

/**
 * @brief Tells whether a button is pressed in the given state.
 *
 * @param state State of the controls to read.
 * @param button Button to check.
 * @return true if the button is pressed, false otherwise.
 */
bool isPressed(const ControlState &state, Button button) {
  return (state.buttons & (1u << button)) != 0;
}

/**
 * @brief Converts the D-pad directions of the state to the position reported by the DS4.
 *
 * Opposite directions pressed together cancel each other.
 *
 * @param state State of the controls to read.
 * @return The D-pad position, as a DPadPosition value.
 */
uint8_t getDPadPosition(const ControlState &state) {
  // Indexed by [vertical + 1][horizontal + 1]: -1 is down or left, 0 is neither, 1 is up or right.
  static const uint8_t POSITIONS[3][3] = {
    { DPadSouthWest, DPadSouth, DPadSouthEast },
    { DPadWest, DPadReleased, DPadEast },
    { DPadNorthWest, DPadNorth, DPadNorthEast },
  };

  int8_t vertical = static_cast<int8_t>(isPressed(state, DPadUp)) - static_cast<int8_t>(isPressed(state, DPadDown));
  int8_t horizontal = static_cast<int8_t>(isPressed(state, DPadRight)) - static_cast<int8_t>(isPressed(state, DPadLeft));
  return POSITIONS[vertical + 1][horizontal + 1];
}

}  // namespace

// Fills an input report from the state of the controls.
void buildInputReport(const ControlState &state, uint8_t counter, uint16_t sensorTimestamp, InputReport &report) {
  memset(&report, 0, sizeof(report));
  report.reportId = INPUT_REPORT_ID;

  report.leftStickX = state.stickX[LeftStick];
  report.leftStickY = state.stickY[LeftStick];
  report.rightStickX = state.stickX[RightStick];
  report.rightStickY = state.stickY[RightStick];

  report.buttons[0] = getDPadPosition(state);
  for (const ButtonBit &buttonBit : BUTTON_BITS) {
    if (isPressed(state, buttonBit.button)) {
      report.buttons[buttonBit.byteIndex] |= 1u << buttonBit.bitIndex;
    }
  }

  // The DS4 reports L2 and R2 both as analog values and as buttons, pressed as soon as they move.
  for (uint8_t trigger = 0; trigger < TRIGGER_COUNT; trigger++) {
    if (state.triggers[trigger] > TRIGGER_MIN) {
      report.buttons[1] |= 1u << TRIGGER_BIT_INDEXES[trigger];
    }
  }
  report.leftTrigger = state.triggers[L2];
  report.rightTrigger = state.triggers[R2];

  report.buttons[2] |= (counter & COUNTER_MASK) << COUNTER_SHIFT;
  report.sensorTimestamp = sensorTimestamp;

  report.status[0] = STATUS_CABLE_CONNECTED | STATUS_BATTERY_FULL;

  // A zeroed touch point would read as a finger in the top-left corner, so mark them all as
  // inactive, including the unused touch reports.
  report.touchReportCount = 1;
  for (TouchReport &touchReport : report.touchReports) {
    for (TouchPoint &point : touchReport.points) {
      point.contact = TOUCH_POINT_INACTIVE;
    }
  }
}

}  // namespace internal
}  // namespace CtrlDS4
