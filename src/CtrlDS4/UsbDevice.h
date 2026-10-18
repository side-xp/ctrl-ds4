/**
 * @file UsbDevice.h
 * @brief USB side of the emulated DS4, built on the Adafruit TinyUSB stack.
 */

#pragma once

// Also makes the build link the TinyUSB stack, which provides Serial when that stack is selected.
#include <Adafruit_TinyUSB.h>

#include "InputReport.h"
#include "OutputReport.h"
#include "Types.h"

namespace CtrlDS4 {
namespace internal {

/**
 * @brief Presents the board to the host as a DS4, and exchanges the reports with it.
 *
 * The USB serial port (Serial) can stay available next to the controller (see begin()).
 *
 * Only one instance can be used, since TinyUSB reports the requests of the host through functions
 * that have no access to an instance.
 */
class UsbDevice {
public:
  /**
   * @brief Adds the DS4 interface to the USB device, and reconnects to the host so it sees it.
   *
   * The cores connect to the host before setup() runs, as a plain serial device. Reconnecting
   * makes the host read the device again, this time with the identity and interface of a DS4.
   *
   * @param macAddress MAC address of the controller, which the host reads when it connects.
   * @param isSerialEnabled true to keep the USB serial port next to the controller, false to remove
   * it, as a real DS4 has none.
   * @return true if the DS4 interface could be added, false otherwise.
   */
  bool begin(const MacAddress &macAddress, bool isSerialEnabled);

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
   * The report is also kept for the hosts that request the latest one, rather than waiting for the
   * next one.
   *
   * @param report Report to send.
   * @return true if the report was queued, false if the host is not ready (see isReadyToSend()).
   */
  bool sendInputReport(const InputReport &report);

  /**
   * @brief Gets the rumble and lightbar state requested by the host, if it changed it.
   *
   * The host sends its requests while the USB stack runs, which can be during an interrupt: they
   * are kept until this function reads them, so they can be handled safely from loop().
   *
   * @param state Receives the requested state, if the host sent a request since the last call.
   * @return true if the host sent a request since the last call, false otherwise.
   */
  bool readOutputState(OutputState &state);

private:
  /// HID interface of the controller.
  Adafruit_USBD_HID _hid;
};

}  // namespace internal
}  // namespace CtrlDS4
