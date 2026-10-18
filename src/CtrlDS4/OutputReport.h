/**
 * @file OutputReport.h
 * @brief Output report sent by the host to drive the rumble and the lightbar.
 */

#pragma once

#include <stdint.h>

#include "Types.h"

namespace CtrlDS4 {
namespace internal {

/// Report ID of the output report.
constexpr uint8_t OUTPUT_REPORT_ID = 0x05;

/**
 * @brief State of the rumble and the lightbar, as last requested by the host.
 *
 * A default-constructed state has the motors stopped and the lightbar off, without blinking.
 */
struct OutputState {
  /// Speed of the StrongMotor.
  uint8_t strongMotor = 0;
  /// Speed of the WeakMotor.
  uint8_t weakMotor = 0;
  /// Color of the lightbar.
  LightbarColor lightbarColor = {};
  /// Blinking of the lightbar.
  LightbarBlink lightbarBlink = {};
};

/**
 * @brief Applies an output report of the host to a state.
 *
 * The report tells which parts it changes (rumble, lightbar color, lightbar blinking), so the other
 * parts of the state are kept.
 *
 * @param report Content of the report, after its report ID.
 * @param size Size of the content in bytes.
 * @param state State to update.
 * @return true if the report was valid and applied, false if it was too short.
 */
bool applyOutputReport(const uint8_t *report, uint16_t size, OutputState &state);

}  // namespace internal
}  // namespace CtrlDS4
