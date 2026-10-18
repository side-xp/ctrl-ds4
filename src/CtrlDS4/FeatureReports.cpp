#include "FeatureReports.h"

#include <string.h>

namespace CtrlDS4 {
namespace internal {

namespace {

/// Resolution of the gyroscope values of the input report, in units per degree per second (as on a real DS4).
constexpr int16_t GYRO_UNITS_PER_DEGREE_PER_SECOND = 16;
/// Rotation speed, in degrees per second, at which the gyroscope calibration is given (as on a real DS4).
constexpr int16_t GYRO_CALIBRATION_SPEED = 540;
/// Resolution of the accelerometer values of the input report, in units per g (as on a real DS4).
constexpr int16_t ACCEL_UNITS_PER_G = 8192;

/// Hardware version reported to the host.
constexpr uint16_t HARDWARE_VERSION = 0x0100;
/// Firmware version reported to the host.
constexpr uint16_t FIRMWARE_VERSION = 0x0100;

/**
 * @brief Calibration of the motion sensors, as sent over USB.
 *
 * Each sensor value is given at rest (bias), then at a known positive and negative motion. The
 * host derives the scale of the input report values from them.
 */
struct __attribute__((packed)) CalibrationReport {
  /// Gyroscope values at rest.
  int16_t gyroPitchBias;
  int16_t gyroYawBias;
  int16_t gyroRollBias;
  /// Gyroscope values at plus and minus gyroSpeedPlus and gyroSpeedMinus.
  int16_t gyroPitchPlus;
  int16_t gyroPitchMinus;
  int16_t gyroYawPlus;
  int16_t gyroYawMinus;
  int16_t gyroRollPlus;
  int16_t gyroRollMinus;
  /// Rotation speeds of the gyroscope calibration, in degrees per second.
  int16_t gyroSpeedPlus;
  int16_t gyroSpeedMinus;
  /// Accelerometer values at plus and minus 1 g.
  int16_t accelXPlus;
  int16_t accelXMinus;
  int16_t accelYPlus;
  int16_t accelYMinus;
  int16_t accelZPlus;
  int16_t accelZMinus;
  /// Unused.
  uint8_t reserved[2];
};

/**
 * @brief Pairing info, which holds the MAC address of the controller.
 */
struct __attribute__((packed)) PairingInfoReport {
  /// MAC address of the controller, last byte first.
  uint8_t macAddress[MAC_ADDRESS_SIZE];
  /// Unused.
  uint8_t reserved[3];
  /// MAC address of the Bluetooth host the controller is paired with, last byte first.
  uint8_t hostMacAddress[MAC_ADDRESS_SIZE];
};

/**
 * @brief MAC address of the controller.
 */
struct __attribute__((packed)) MacAddressReport {
  /// MAC address of the controller, last byte first.
  uint8_t macAddress[MAC_ADDRESS_SIZE];
};

/**
 * @brief Hardware and firmware versions, among other unused info.
 */
struct __attribute__((packed)) FirmwareInfoReport {
  /// Unused.
  uint8_t reserved1[34];
  /// Hardware version of the controller.
  uint16_t hardwareVersion;
  /// Unused.
  uint8_t reserved2[4];
  /// Firmware version of the controller.
  uint16_t firmwareVersion;
  /// Unused.
  uint8_t reserved3[6];
};

/**
 * @brief Definition of the controller: its type and the features it has.
 *
 * The layout is not documented, so the content of a real DS4 is sent as is. Taken from GP2040-CE
 * (https://gp2040-ce.info), Copyright (c) 2024 OpenStickCommunity, MIT License.
 */
const uint8_t DEFINITION_REPORT[] = {
  0x21, 0x27, 0x04, 0xCF, 0x00, 0x2C, 0x56,
  0x08, 0x00, 0x3D, 0x00, 0xE8, 0x03, 0x04, 0x00,
  0xFF, 0x7F, 0x0D, 0x0D, 0x00, 0x00, 0x00, 0x00,
  0x0D, 0x84, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static_assert(sizeof(DEFINITION_REPORT) == 47, "Must match the Report Count of report 3 in HID_REPORT_DESCRIPTOR");
static_assert(sizeof(CalibrationReport) == 36, "Must match the Report Count of report 2 in HID_REPORT_DESCRIPTOR");
static_assert(sizeof(PairingInfoReport) == 15, "Must match the Report Count of report 18 in HID_REPORT_DESCRIPTOR");
static_assert(sizeof(MacAddressReport) == 6, "Must match the Report Count of report 129 in HID_REPORT_DESCRIPTOR");
static_assert(sizeof(FirmwareInfoReport) == 48, "Must match the Report Count of report 163 in HID_REPORT_DESCRIPTOR");

/**
 * @brief Copies a MAC address in the order the DS4 sends it, last byte first.
 *
 * @param macAddress Address to copy.
 * @param output Where to copy the address.
 */
void copyMacAddressReversed(const MacAddress &macAddress, uint8_t (&output)[MAC_ADDRESS_SIZE]) {
  for (uint8_t i = 0; i < MAC_ADDRESS_SIZE; i++) {
    output[i] = macAddress.bytes[MAC_ADDRESS_SIZE - 1 - i];
  }
}

/**
 * @brief Copies a report to the buffer of the host request, cut to fit it.
 *
 * @tparam Report Type of the report.
 * @param report Report to copy.
 * @param buffer Buffer receiving the report.
 * @param bufferSize Size of the buffer in bytes.
 * @return Number of bytes copied.
 */
template<typename Report>
uint16_t copyReport(const Report &report, uint8_t *buffer, uint16_t bufferSize) {
  uint16_t copiedSize = sizeof(report) < bufferSize ? sizeof(report) : bufferSize;
  memcpy(buffer, &report, copiedSize);
  return copiedSize;
}

/**
 * @brief Builds the calibration of the motion sensors.
 *
 * The values define the resolution of the motion values of the input report as the one of a real
 * DS4 (see GYRO_UNITS_PER_DEGREE_PER_SECOND and ACCEL_UNITS_PER_G), with no bias.
 *
 * @return The calibration report.
 */
CalibrationReport makeCalibrationReport() {
  constexpr int16_t gyroCalibrationValue = GYRO_CALIBRATION_SPEED * GYRO_UNITS_PER_DEGREE_PER_SECOND;

  CalibrationReport report = {};
  report.gyroPitchPlus = gyroCalibrationValue;
  report.gyroPitchMinus = -gyroCalibrationValue;
  report.gyroYawPlus = gyroCalibrationValue;
  report.gyroYawMinus = -gyroCalibrationValue;
  report.gyroRollPlus = gyroCalibrationValue;
  report.gyroRollMinus = -gyroCalibrationValue;
  report.gyroSpeedPlus = GYRO_CALIBRATION_SPEED;
  report.gyroSpeedMinus = GYRO_CALIBRATION_SPEED;
  report.accelXPlus = ACCEL_UNITS_PER_G;
  report.accelXMinus = -ACCEL_UNITS_PER_G;
  report.accelYPlus = ACCEL_UNITS_PER_G;
  report.accelYMinus = -ACCEL_UNITS_PER_G;
  report.accelZPlus = ACCEL_UNITS_PER_G;
  report.accelZMinus = -ACCEL_UNITS_PER_G;
  return report;
}

/**
 * @brief Builds the pairing info.
 *
 * @param macAddress MAC address of the controller.
 * @return The pairing info report, not paired with any Bluetooth host.
 */
PairingInfoReport makePairingInfoReport(const MacAddress &macAddress) {
  PairingInfoReport report = {};
  copyMacAddressReversed(macAddress, report.macAddress);
  return report;
}

/**
 * @brief Builds the MAC address report.
 *
 * @param macAddress MAC address of the controller.
 * @return The MAC address report.
 */
MacAddressReport makeMacAddressReport(const MacAddress &macAddress) {
  MacAddressReport report = {};
  copyMacAddressReversed(macAddress, report.macAddress);
  return report;
}

/**
 * @brief Builds the firmware info.
 *
 * @return The firmware info report, with HARDWARE_VERSION and FIRMWARE_VERSION.
 */
FirmwareInfoReport makeFirmwareInfoReport() {
  FirmwareInfoReport report = {};
  report.hardwareVersion = HARDWARE_VERSION;
  report.firmwareVersion = FIRMWARE_VERSION;
  return report;
}

}  // namespace

// Fills the content of a feature report requested by the host.
uint16_t buildFeatureReport(uint8_t reportId, const MacAddress &macAddress, uint8_t *buffer, uint16_t size) {
  switch (reportId) {
    case DEFINITION_REPORT_ID:
      return copyReport(DEFINITION_REPORT, buffer, size);
    case CALIBRATION_REPORT_ID:
      return copyReport(makeCalibrationReport(), buffer, size);
    case PAIRING_INFO_REPORT_ID:
      return copyReport(makePairingInfoReport(macAddress), buffer, size);
    case MAC_ADDRESS_REPORT_ID:
      return copyReport(makeMacAddressReport(macAddress), buffer, size);
    case FIRMWARE_INFO_REPORT_ID:
      return copyReport(makeFirmwareInfoReport(), buffer, size);
    default:
      // The descriptor declares many more reports, as a real DS4 does. Hosts may read them, and
      // could give up on a controller that doesn't answer.
      memset(buffer, 0, size);
      return size;
  }
}

}  // namespace internal
}  // namespace CtrlDS4
