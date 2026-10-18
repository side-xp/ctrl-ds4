#include "HidDescriptor.h"

namespace CtrlDS4 {
namespace internal {

const uint8_t HID_REPORT_DESCRIPTOR[] = {
  0x05, 0x01,  // Usage Page (Generic Desktop)
  0x09, 0x05,  // Usage (Game Pad)
  0xA1, 0x01,  // Collection (Application)

  // Input report: see InputReport for the layout.
  0x85, 0x01,        //   Report ID (1)
  0x09, 0x30,        //   Usage (X): left stick X
  0x09, 0x31,        //   Usage (Y): left stick Y
  0x09, 0x32,        //   Usage (Z): right stick X
  0x09, 0x35,        //   Usage (Rz): right stick Y
  0x15, 0x00,        //   Logical Minimum (0)
  0x26, 0xFF, 0x00,  //   Logical Maximum (255)
  0x75, 0x08,        //   Report Size (8)
  0x95, 0x04,        //   Report Count (4)
  0x81, 0x02,        //   Input (Data, Variable, Absolute)
  0x09, 0x39,        //   Usage (Hat Switch): D-pad
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x07,        //   Logical Maximum (7)
  0x35, 0x00,        //   Physical Minimum (0)
  0x46, 0x3B, 0x01,  //   Physical Maximum (315)
  0x65, 0x14,        //   Unit (Degrees)
  0x75, 0x04,        //   Report Size (4)
  0x95, 0x01,        //   Report Count (1)
  0x81, 0x42,        //   Input (Data, Variable, Absolute, Null State): out of range means released
  0x65, 0x00,        //   Unit (None)
  0x05, 0x09,        //   Usage Page (Button)
  0x19, 0x01,        //   Usage Minimum (1)
  0x29, 0x0E,        //   Usage Maximum (14)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x01,        //   Logical Maximum (1)
  0x75, 0x01,        //   Report Size (1)
  0x95, 0x0E,        //   Report Count (14)
  0x81, 0x02,        //   Input (Data, Variable, Absolute)
  0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
  0x09, 0x20,        //   Usage (0x20): report counter
  0x75, 0x06,        //   Report Size (6)
  0x95, 0x01,        //   Report Count (1)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x7F,        //   Logical Maximum (127)
  0x81, 0x02,        //   Input (Data, Variable, Absolute)
  0x05, 0x01,        //   Usage Page (Generic Desktop)
  0x09, 0x33,        //   Usage (Rx): L2
  0x09, 0x34,        //   Usage (Ry): R2
  0x15, 0x00,        //   Logical Minimum (0)
  0x26, 0xFF, 0x00,  //   Logical Maximum (255)
  0x75, 0x08,        //   Report Size (8)
  0x95, 0x02,        //   Report Count (2)
  0x81, 0x02,        //   Input (Data, Variable, Absolute)
  0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
  0x09, 0x21,        //   Usage (0x21): motion, battery and touchpad
  0x95, 0x36,        //   Report Count (54)
  0x81, 0x02,        //   Input (Data, Variable, Absolute)

  // Output report: rumble and lightbar.
  0x85, 0x05,  //   Report ID (5)
  0x09, 0x22,  //   Usage (0x22)
  0x95, 0x1F,  //   Report Count (31)
  0x91, 0x02,  //   Output (Data, Variable, Absolute)

  // Feature reports: see FeatureReports.h for their content.
  0x85, 0x02,        //   Report ID (2): motion sensors calibration
  0x09, 0x24,        //   Usage (0x24)
  0x95, 0x24,        //   Report Count (36)
  0xB1, 0x02,        //   Feature (Data, Variable, Absolute)
  0x06, 0x02, 0xFF,  //   Usage Page (Vendor Defined 0xFF02)
  0x85, 0x12,        //   Report ID (18): pairing info
  0x09, 0x21,        //   Usage (0x21)
  0x95, 0x0F,        //   Report Count (15)
  0xB1, 0x02,        //   Feature (Data, Variable, Absolute)
  0x06, 0x80, 0xFF,  //   Usage Page (Vendor Defined 0xFF80)
  0x85, 0x81,        //   Report ID (129): MAC address
  0x09, 0x21,        //   Usage (0x21)
  0x95, 0x06,        //   Report Count (6)
  0xB1, 0x02,        //   Feature (Data, Variable, Absolute)
  0x85, 0xA3,        //   Report ID (163): firmware info
  0x09, 0x45,        //   Usage (0x45)
  0x95, 0x30,        //   Report Count (48)
  0xB1, 0x02,        //   Feature (Data, Variable, Absolute)

  0xC0,  // End Collection
};

const uint16_t HID_REPORT_DESCRIPTOR_SIZE = sizeof(HID_REPORT_DESCRIPTOR);

}  // namespace internal
}  // namespace CtrlDS4
