#include "UsbDevice.h"

#include "FeatureReports.h"
#include "HidDescriptor.h"

namespace CtrlDS4 {
namespace internal {

namespace {

/// USB vendor ID of Sony, which hosts use to recognize the controller.
constexpr uint16_t SONY_VENDOR_ID = 0x054C;
/// USB product ID of the first DS4 model (CUH-ZCT1), the most widely supported one.
constexpr uint16_t DS4_PRODUCT_ID = 0x05C4;

/// Manufacturer name reported by a real DS4.
const char MANUFACTURER_NAME[] = "Sony Computer Entertainment";
/// Product name reported by a real DS4.
const char PRODUCT_NAME[] = "Wireless Controller";

/// Time between two input reports requested from the host: 250 Hz, the rate of a real DS4 over USB.
constexpr uint8_t POLL_INTERVAL_MS = 4;

/// How long the board stays disconnected while reconnecting, so the host notices it.
constexpr uint32_t RECONNECTION_DELAY_MS = 10;

/// MAC address reported in the feature reports, set by UsbDevice::begin().
MacAddress reportedMacAddress;

/**
 * @brief Answers a report request of the host. Called by TinyUSB.
 *
 * @param reportId ID of the requested report.
 * @param reportType Type of the requested report. Only feature reports are answered.
 * @param buffer Buffer receiving the content of the report, after the report ID.
 * @param requestedSize Size of the buffer in bytes.
 * @return Number of bytes written, or 0 if the controller has no such report.
 */
uint16_t onGetReport(uint8_t reportId, hid_report_type_t reportType, uint8_t *buffer, uint16_t requestedSize) {
  if (reportType != HID_REPORT_TYPE_FEATURE) {
    return 0;
  }

  return buildFeatureReport(reportId, reportedMacAddress, buffer, requestedSize);
}

}  // namespace

// Adds the DS4 interface to the USB device, and reconnects to the host so it sees it.
bool UsbDevice::begin(const MacAddress &macAddress) {
  reportedMacAddress = macAddress;

  TinyUSBDevice.setID(SONY_VENDOR_ID, DS4_PRODUCT_ID);
  TinyUSBDevice.setManufacturerDescriptor(MANUFACTURER_NAME);
  TinyUSBDevice.setProductDescriptor(PRODUCT_NAME);

  _hid.setReportDescriptor(HID_REPORT_DESCRIPTOR, HID_REPORT_DESCRIPTOR_SIZE);
  _hid.setReportCallback(onGetReport, nullptr);
  _hid.setPollInterval(POLL_INTERVAL_MS);
  // Hosts send the output report (rumble and lightbar) through this endpoint.
  _hid.enableOutEndpoint(true);
  if (!_hid.begin()) {
    return false;
  }

  // Reconnect even if the host has not finished reading the device yet, since it may already have
  // read its previous identity.
  TinyUSBDevice.detach();
  delay(RECONNECTION_DELAY_MS);
  TinyUSBDevice.attach();
  return true;
}

// Tells whether the host is ready to receive a new input report.
bool UsbDevice::isReadyToSend() {
  return _hid.ready();
}

// Sends an input report to the host.
bool UsbDevice::sendInputReport(const InputReport &report) {
  // The report already starts with its ID, so it's sent as is (report ID 0 adds nothing before it).
  return _hid.sendReport(0, &report, sizeof(report));
}

}  // namespace internal
}  // namespace CtrlDS4
