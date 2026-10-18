# Third-party notices

CtrlDS4 includes data from the projects below, under their own license.

## GP2040-CE

[GP2040-CE](https://github.com/OpenStickCommunity/GP2040-CE) is a gamepad firmware that can present itself as a DS4. CtrlDS4 uses some of the DS4 data it holds, captured from a real controller, so hosts recognize the board as a DS4:

| Data | In CtrlDS4 | In GP2040-CE |
| --- | --- | --- |
| HID report descriptor | `HID_REPORT_DESCRIPTOR`, in [`src/CtrlDS4/HidDescriptor.cpp`](./src/CtrlDS4/HidDescriptor.cpp) | `ps4_report_descriptor`, in `headers/drivers/ps4/PS4Descriptors.h` |
| Controller definition | `DEFINITION_REPORT`, in [`src/CtrlDS4/FeatureReports.cpp`](./src/CtrlDS4/FeatureReports.cpp) | The "normal controller" variant of `output_0x03`, in `src/drivers/ps4/PS4Driver.cpp` |
| Firmware info | `FIRMWARE_INFO_REPORT`, in [`src/CtrlDS4/FeatureReports.cpp`](./src/CtrlDS4/FeatureReports.cpp) | `output_0xa3`, in `src/drivers/ps4/PS4Driver.cpp` |

GP2040-CE is licensed under the MIT License:

```text
MIT License

Copyright (c) 2024 OpenStickCommunity (gp2040-ce.info)
Copyright (c) 2021 Jason Skuby (mytechtoybox.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
