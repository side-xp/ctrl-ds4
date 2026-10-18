# @todo Project title

> @todo Short summary of what this project does.

## Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software) or [arduino-cli](https://arduino.github.io/arduino-cli/latest/)
- @todo Supported boards, required hardware and wiring

## Install

### From the Library Manager (recommended)

- **Arduino IDE:** open `Sketch > Include Library > Manage Libraries...`, search for `CtrlTemplate` and click **Install**.
- **arduino-cli:**

```bash
arduino-cli lib install CtrlTemplate
```

### From a ZIP file

Download the **Source code (zip)** of the [latest release](https://github.com/hubertgdev/ctrl-template/releases/latest), then:

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
#include <CtrlTemplate.h>
```

See the examples in `File > Examples > CtrlTemplate` in the Arduino IDE, or in the [`examples/`](./examples) folder.

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