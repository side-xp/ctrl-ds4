/**
 * @file HidDescriptor.h
 * @brief HID report descriptor of the emulated DS4.
 */

#pragma once

#include <stdint.h>

namespace CtrlDS4 {
namespace internal {

/**
 * @brief Describes the reports of the controller to the host.
 *
 * The input and output reports are declared as on a real DS4, so hosts that only rely on the
 * descriptor (e.g. the generic HID gamepad support of Windows) read them the same way.
 */
extern const uint8_t HID_REPORT_DESCRIPTOR[];

/// Size of HID_REPORT_DESCRIPTOR in bytes.
extern const uint16_t HID_REPORT_DESCRIPTOR_SIZE;

}  // namespace internal
}  // namespace CtrlDS4
