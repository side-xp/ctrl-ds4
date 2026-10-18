# CtrlDS4

Arduino library to emulate a Sony DualShock 4 (DS4) controller over USB, on boards with native USB such as the *RP2040* and the *SAMD21*.

---
> 🤖 This package has mostly been implemented using AI coding assistant. You can expect some questionable implementation or use cases that are not covered yet.
> We will refine this package and its usage over time.
---

---
⚠️ This library is still WIP.
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

Include the library in your sketch:

```cpp
#include <CtrlDS4.h>
```

See the examples in `File > Examples > CtrlDS4` in the Arduino IDE, or in the [`examples/`](./examples) folder.

## Development

### Requirements

| Tool | Used for | Install |
| --- | --- | --- |
| [Git](https://git-scm.com) | Version control, listing the sources to check. | [Downloads](https://git-scm.com/downloads) |
| [Task](https://taskfile.dev) | Running the dev commands of [`Taskfile.yml`](./Taskfile.yml). | [Installation guide](https://taskfile.dev/installation/) (`winget install Task.Task` on Windows) |
| [arduino-cli](https://arduino.github.io/arduino-cli/latest/) | Compiling the examples. | [Installation guide](https://arduino.github.io/arduino-cli/latest/installation/) |
| [clang-format](https://clang.llvm.org/docs/ClangFormat.html) 14 or later | Formatting the code. | `pip install clang-format`, or with [LLVM](https://github.com/llvm/llvm-project/releases) |
| [arduino-lint](https://arduino.github.io/arduino-lint/latest/) | Checking the library structure and metadata. | Download the archive for your system from the [releases](https://github.com/arduino/arduino-lint/releases), extract it outside the project and add that folder to your `PATH` |
| [Python](https://www.python.org) 3.8 or later | Optional, VS Code setup only. | [Downloads](https://www.python.org/downloads/) |

> The [Arduino IDE](https://www.arduino.cc/en/software) is not required for development, but it's the best way to test the library the way users will install it.

### Setup

```bash
# Install the board cores used to compile the examples
task setup:cores

# Optional, for VS Code: let the C/C++ extension resolve the Arduino includes
# (run again after adding an example or a new #include)
task setup:editor
```

### Checks

Run all the checks (formatting, lint, compilation) before pushing:

```bash
task check
```

Use `task format` to fix the formatting, and `task --list` to see all the available commands.

## Commit conventions

This project uses [Conventional Commits](https://www.conventionalcommits.org):

```
<type>(<optional scope>): <short description>
```

Common types: `feat`, `fix`, `chore`, `docs`, `test`, `refactor`, `perf`, `ci`.

> **Why this matters:** `semantic-release` reads the commit history to determine the next version number and generate the changelog. A `feat` triggers a minor bump, a `fix` triggers a patch, and a `BREAKING CHANGE` footer triggers a major bump.

## CI/CD

The CI runs on GitHub Actions ([`.github/workflows/ci.yml`](./.github/workflows/ci.yml)), with the same tools as for local development:

- **Push on `dev` or `develop`:** runs `task check`.
- **Push on `main` or `master`:** runs `task check`, then [semantic-release](https://semantic-release.gitbook.io) ([`.releaserc.json`](./.releaserc.json)). If the commits since the last release contain a `feat`, `fix`, `perf` or breaking change, it:
  - sets the new version in [`library.properties`](./library.properties), commits it and tags that commit with the version
  - creates a GitHub release, with notes generated from the commits

> The release adds a commit on the main branch: pull it before merging into the main branch again.

> Once the library is registered in the Library Manager, each new tag is picked up automatically as a new version.

## License

This project is licensed under the [MIT License](./LICENSE.md).

---

Crafted and maintained with love by [Sideways Experiments](https://sideways-experiments.com)

(c) 2022-2026 Sideways Experiments