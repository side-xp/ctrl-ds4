/**
 * @file FeatureReports.h
 * @brief Feature reports the host reads from the controller, mostly when it connects.
 *
 * Linux and Android read them to set up the controller, and give up on it if they fail: the
 * hid-playstation driver reads CALIBRATION_REPORT_ID, PAIRING_INFO_REPORT_ID and
 * FIRMWARE_INFO_REPORT_ID, and the older hid-sony driver reads CALIBRATION_REPORT_ID and
 * MAC_ADDRESS_REPORT_ID. DEFINITION_REPORT_ID is answered too, as a real DS4 does, for the other
 * hosts that may read it.
 */

#pragma once

#include <stdint.h>

#include "Types.h"

namespace CtrlDS4 {
namespace internal {

/// Report ID of the definition of the controller (type and features).
constexpr uint8_t DEFINITION_REPORT_ID = 0x03;
/// Report ID of the calibration of the motion sensors.
constexpr uint8_t CALIBRATION_REPORT_ID = 0x02;
/// Report ID of the pairing info, which holds the MAC address of the controller.
constexpr uint8_t PAIRING_INFO_REPORT_ID = 0x12;
/// Report ID of the MAC address of the controller.
constexpr uint8_t MAC_ADDRESS_REPORT_ID = 0x81;
/// Report ID of the hardware and firmware versions.
constexpr uint8_t FIRMWARE_INFO_REPORT_ID = 0xA3;

/**
 * @brief Fills the content of a feature report requested by the host.
 *
 * Only the content is written: TinyUSB sends the report ID before it.
 *
 * @param reportId ID of the requested report.
 * @param macAddress MAC address of the controller.
 * @param buffer Buffer receiving the content of the report.
 * @param size Size of the buffer in bytes. A longer report is cut to fit.
 * @return Number of bytes written. The reports the library has no content for are filled with
 * zeros, on the whole buffer.
 */
uint16_t buildFeatureReport(uint8_t reportId, const MacAddress &macAddress, uint8_t *buffer, uint16_t size);

}  // namespace internal
}  // namespace CtrlDS4
