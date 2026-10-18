/**
 * @file UsbDevice.h
 * @brief USB side of the emulated DS4, built on the Adafruit TinyUSB stack.
 */

#pragma once

// Also makes the build link the TinyUSB stack, which provides Serial when that stack is selected.
#include <Adafruit_TinyUSB.h>

#include "InputReport.h"

namespace CtrlDS4 {
namespace internal {

/**
 * @brief Presents the board to the host as a DS4, and sends it the reports.
 *
 * The USB serial port (Serial) stays available next to the controller.
 */
class UsbDevice {
public:
  /**
   * @brief Adds the DS4 interface to the USB device, and reconnects to the host so it sees it.
   *
   * The cores connect to the host before setup() runs, as a plain serial device. Reconnecting
   * makes the host read the device again, this time with the identity and interface of a DS4.
   *
   * @return true if the DS4 interface could be added, false otherwise.
   */
  bool begin();

  /**
   * @brief Tells whether the host is ready to receive a new input report.
   *
   * The host asks for reports at a fixed rate, so it's ready again once it has read the previous
   * one.
   *
   * @return true if sendInputReport() can be called, false otherwise.
   */
  bool isReadyToSend();

  /**
   * @brief Sends an input report to the host.
   *
   * @param report Report to send.
   * @return true if the report was queued, false if the host is not ready (see isReadyToSend()).
   */
  bool sendInputReport(const InputReport &report);

private:
  /// HID interface of the controller.
  Adafruit_USBD_HID _hid;
};

}  // namespace internal
}  // namespace CtrlDS4
