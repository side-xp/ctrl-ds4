# CtrlDS4

Arduino library to emulate a Sony DualShock 4 (DS4) controller over USB, on boards with native USB such as the *RP2040* and the *SAMD21*.

---
> 🤖 This package has mostly been implemented using AI coding assistant. You can expect some questionable implementation or use cases that are not covered yet.
> We will refine this package and its usage over time.
---

## Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software) or [arduino-cli](https://arduino.github.io/arduino-cli/latest/)
- A supported board, with its core installed and the **TinyUSB** USB stack selected

### Supported boards

| Board | Core | Board manager URL | FQBN |
| --- | --- | --- | --- |
| Seeed XIAO SAMD21 | [Seeed SAMD Boards](https://github.com/Seeed-Studio/ArduinoCore-samd) | `https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json` | `Seeeduino:samd:seeed_XIAO_m0:usbstack=tinyusb` |
| Seeed XIAO RP2040 | [Raspberry Pi Pico/RP2040](https://github.com/earlephilhower/arduino-pico) (Earle Philhower) | `https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json` | `rp2040:rp2040:seeed_xiao_rp2040:usbstack=tinyusb` |

> Other boards of these cores should work too, as long as they use the TinyUSB stack, but they are not tested.

> The official Arduino SAMD and Arduino Mbed RP2040 cores are not supported: their USB stack doesn't let the library present itself as a DS4 controller.

### Board setup

- **Arduino IDE:**
  1. Open `File > Preferences...` and add the board manager URL of your board to **Additional boards manager URLs**.
  2. Open `Tools > Board > Boards Manager...`, search for the core and click **Install**.
  3. Select your board in `Tools > Board`, then select `Tools > USB Stack > TinyUSB` (SAMD21) or `Tools > USB Stack > Adafruit TinyUSB` (RP2040).
- **arduino-cli:** install the core, then compile and upload with the FQBN of your board, which includes the USB stack option:

```bash
arduino-cli core install Seeeduino:samd --additional-urls <board manager URL>
arduino-cli compile --fqbn Seeeduino:samd:seeed_XIAO_m0:usbstack=tinyusb <sketch>
```

> The Adafruit TinyUSB library is bundled with both cores, in a version that matches the core. Don't install it from the Library Manager: that copy would be used instead, and may not match.

## Install

### From the Library Manager (recommended)

- **Arduino IDE:** open `Sketch > Include Library > Manage Libraries...`, search for `CtrlDS4` and click **Install**.
- **arduino-cli:**

```bash
arduino-cli lib install CtrlDS4
```

### From a ZIP file

Download the **Source code (zip)** of the [latest release](https://github.com/side-xp/ctrl-ds4/releases/latest), then:

- **Arduino IDE:** open `Sketch > Include Library > Add .ZIP Library...` and select the downloaded file.
- **arduino-cli:**

```bash
# Installing from a ZIP file is disabled by default
arduino-cli config set library.enable_unsafe_install true
arduino-cli lib install --zip-path <downloaded file>.zip
```

## Usage

### Quick start

Wire a push button between pin `D2` and `GND`, and another between pin `D3` and `GND`. No resistor is needed: the library enables the internal pull-up resistors of the pins.

```cpp
#include <CtrlDS4.h>

CtrlDS4::Controller controller;

void setup() {
  controller.bind(CtrlDS4::Cross, 2);
  controller.bind(CtrlDS4::Circle, 3);
  controller.begin();
}

void loop() {
  controller.update();
}
```

Upload the sketch: the board now shows up on the host as a *Wireless Controller*, and the buttons drive Cross and Circle.

Call `update()` on every iteration of `loop()`, and avoid blocking `loop()` for long (e.g. with `delay()`): the host expects a new report every few milliseconds.

See the examples in `File > Examples > CtrlDS4` in the Arduino IDE, or in the [`examples/`](./examples) folder.

> Declare a single `Controller` per sketch: the board presents itself as one DS4.

### Controls

The controls are named after the DS4 ones:

| Type | Values |
| --- | --- |
| `CtrlDS4::Button` | `Cross`, `Circle`, `Square`, `Triangle`, `L1`, `R1`, `L3`, `R3`, `Share`, `Options`, `PS`, `Touchpad`, `DPadUp`, `DPadDown`, `DPadLeft`, `DPadRight` |
| `CtrlDS4::Stick` | `LeftStick`, `RightStick` |
| `CtrlDS4::Trigger` | `L2`, `R2` |

The D-pad directions are buttons, so they can be bound to pins like the others. Opposite directions pressed together cancel each other.

#### Bind buttons to pins

```cpp
controller.bind(CtrlDS4::Cross, 2);                        // Button between pin 2 and GND
controller.bind(CtrlDS4::Circle, 3, CtrlDS4::ActiveHigh);  // Button between pin 3 and 3.3V
controller.unbind(CtrlDS4::Circle);                        // Stop driving Circle from its pin
```

The bound buttons are read on each `update()`, and their contact bounces are filtered: a press is reported at once, then the pin is ignored for 10 ms. If a button reports double presses, increase that time with `controller.setDebounceTime(20)`.

#### Set the controls from the sketch

For anything that is not a button wired to a pin (an analog stick, a sensor, a value computed by the sketch...), set the controls directly:

```cpp
controller.setButton(CtrlDS4::Square, true);        // Press Square
controller.setAxis(CtrlDS4::LeftStickX, 1);         // Push the left stick fully right
controller.setAxis(CtrlDS4::LeftStickY, -0.5);      // Push the left stick halfway down
controller.setTriggerValue(CtrlDS4::R2, 0.5);       // Press R2 halfway
controller.releaseAll();                            // Put all the controls at rest
```

The stick axes (`CtrlDS4::StickAxis`: `LeftStickX`, `LeftStickY`, `RightStickX`, `RightStickY`) go from `-1` to `1`, `0` being at rest: X goes from left to right, and Y from down to up. The triggers go from `0` (released) to `1` (fully pressed).

> `setStick()` and `setTrigger()` set the same controls with the values of the DS4 report instead, from `0` to `255`. In that format, a stick at rest is at `CtrlDS4::STICK_CENTER` (`128`), and Y goes from up to down.

> A bound button follows its pin: calling `setButton()` on it has no lasting effect.

### Rumble and lightbar

The host can drive the rumble motors and the lightbar of the controller. Read their latest state at any time, or register functions to call when they change:

```cpp
void onRumble(uint8_t strongMotor, uint8_t weakMotor) {
  analogWrite(MOTOR_PIN, strongMotor);
}

void setup() {
  controller.onRumble(onRumble);
  controller.begin();
}

void loop() {
  controller.update();
  CtrlDS4::LightbarColor color = controller.getLightbarColor();
  // ...
}
```

The callbacks are called from `update()`, so they can safely drive pins or print to `Serial`. See the [`RumbleAndLightbar`](./examples/RumbleAndLightbar/RumbleAndLightbar.ino) example for all of them.

### MAC address

Hosts tell controllers apart with their MAC address. The library derives a unique one from the factory ID of the board, so each board gets its own address with nothing to set up.

If a board has no usable factory ID (some RP2040 clone boards), `controller.getMacAddressSource()` returns `CtrlDS4::MacFallback`, and all such boards share the same address. Set one per board before `begin()` in that case:

```cpp
controller.setMacAddress(CtrlDS4::MacAddress(0x02, 0x00, 0x00, 0x00, 0x00, 0x2A));
```

### Compatibility

| Host | Status |
| --- | --- |
| Windows (generic gamepad, browsers) | Tested |
| Linux, Android | Expected to work, not tested yet |
| Steam | Expected to work, not tested yet |
| PS4, PS5 | Not supported: the consoles authenticate genuine controllers |

Only the USB connection is supported, not Bluetooth.

### Troubleshooting

- **The upload fails, or the port of the board is gone:** once the sketch runs, the board presents itself with the identity of a DS4, so its serial port changes. Put the board in bootloader mode, then upload again: on the XIAO SAMD21, short the two `RST` pads twice quickly. On the XIAO RP2040, hold `BOOT` while plugging it in.
- **The Serial Monitor shows nothing after a reset:** `begin()` briefly disconnects the board, so the host sees it again as a DS4. Open the Serial Monitor after `begin()`, or reopen it.

## Contributing

Bug reports, ideas and contributions are welcome: see [`CONTRIBUTING.md`](./CONTRIBUTING.md) for how to get involved, and for the developer setup.

## License

This project is licensed under the [MIT License](./LICENSE.md).

---

Crafted and maintained with love by [Sideways Experiments](https://sideways-experiments.com)

(c) 2022-2026 Sideways Experiments