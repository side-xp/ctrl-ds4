# Contributing

First of all, thank you for considering contributing to a *Sideways Experiments* project!

At Sideways Experiments, we hate doing the same thing more than once, and we love to share knowledge. Whether because you "just want to help" or share that generous laziness philosophy, you're welcome!

## Developer setup

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

# Install the libraries some examples need besides CtrlDS4
task setup:libraries

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

The compilation covers the [`examples/`](./examples) and the test sketches of [`extras/tests/`](./extras/tests). The test sketches are hardware checks for the development of the library: the Arduino IDE doesn't list them as examples.

A sketch that needs hardware only SAMD boards have goes in `SAMD_ONLY_SKETCHES` in [`Taskfile.yml`](./Taskfile.yml), so it's skipped for the other boards. A library a sketch needs besides CtrlDS4 goes in `SKETCH_LIBRARIES`.

## Get involved!

There are many ways to be involved in our open source projects, and there's absolutely no pressure to give more or less of your time. So whether you want to develop the core of a whole package or just post a comment in an issue, any help is appreciated!

So what can you do to help?

- **Discuss with the community**: join our [Discord server](https://discord.gg/G49RUZ9F2N), and start chatting with the community or the core team
- **Report bugs**: found a bug when using one of our packages? You can report it in the *Issues* tab on GitHub
- **Suggest improvements**: whether from the [Discord server](https://discord.gg/G49RUZ9F2N) or by creating an *Issue*, feel free to talk about your needs or current usage of our solutions, highlight what's missing or what could be better
- **Request new features**: again, you can use the [Discord server](https://discord.gg/G49RUZ9F2N) or create a new *Issue* to ask for something new, see if others may need it too, so we can consider modifying an existing package or even start a new project just for it
- **Address issues**: you can contribute directly to the codebase by resolving an issue, creating the required assets, or just implementing changes and create a Pull Request on *GitHub*

> By the way, we love to know how you use our tools, so we can make them better!

## Code of Conduct

*Sideways Experiments* has adopted the [*Contributor Covenant*](https://www.contributor-covenant.org/) as its Code of Conduct for its open source projects, and we expect contributors to adhere to it.

If you observe any unacceptable behavior or need to report a violation, please contact the *Sideways Experiments* core team directly to [contact@sideways-experiments.com](mailto:contact@sideways-experiments.com).

## Code syntax

Please refer to our [general Coding Style specification](https://github.com/side-xp/.github/blob/main/docs/coding-style/README.md) to learn more about the conventions used in this project.

## Submitting a Pull Request

To contribute code:

1. **Fork** this repository
2. Create a **new branch** off `develop`
3. Make your changes and commit them
   - **Signed commits** are required
   - Take care to follow our [commit message guidelines](https://github.com/side-xp/.github/blob/main/docs/coding-style/commit-messages.md)
4. Test your changes locally
5. Submit a **Pull Request targeting `develop` or `dev`**, not `master` or `main`
6. Describe your changes and reference any related issues
7. A member of the ***Sideways Experiments* core team** will review it

As mentioned in the [*Suggesting enhancements or features*](https://github.com/side-xp/.github/blob/main/SUPPORT.md#suggesting-enhancements-or-features) section of our support guidelines, please don't create Pull Requests for unsolicited work.

### Commit messages

This project uses [Conventional Commits](https://www.conventionalcommits.org):

```
<type>(<optional scope>): <short description>
```

Common types: `feat`, `fix`, `chore`, `docs`, `test`, `refactor`, `perf`, `ci`.

> **Why this matters:** `semantic-release` reads the commit history to determine the next version number and generate the changelog. A `feat` triggers a minor bump, a `fix` triggers a patch, and a `BREAKING CHANGE` footer triggers a major bump.

### Pull Request Requirements

- One PR per logical change (fix, feature, etc.)
- Must pass basic tests (if applicable)
- Must use **signed commits**
- All PRs must be reviewed by a *Sideways Experiments* core team member
- Assign the PR to the core team if not automatically assigned

## CI/CD and releases

The CI runs on GitHub Actions ([`.github/workflows/ci.yml`](./.github/workflows/ci.yml)), with the same tools as for local development:

- **Push on `dev` or `develop`:** runs `task check`.
- **Push on `main` or `master`:** runs `task check`, then [semantic-release](https://semantic-release.gitbook.io) ([`.releaserc.json`](./.releaserc.json)). If the commits since the last release contain a `feat`, `fix`, `perf` or breaking change, it:
  - sets the new version in [`library.properties`](./library.properties), commits it and tags that commit with the version (e.g. `0.2.0`, without a `v` prefix)
  - creates a GitHub release, with notes generated from the commits

> The release adds a commit on the main branch: pull it before merging into the main branch again.

> Once the library is registered in the Library Manager, each new tag is picked up automatically as a new version.

We currently do **not** require contributors to handle releases: this is done internally.

## License and Contributor Agreement

Most of our projects are licensed under the [MIT License](https://mit-license.org).

A `LICENSE.md` file is always included in our projects' repository root, and all contributions to a specific project will be licensed under the same terms as that file.

We will evaluate whether a Contributor License Agreement (CLA) is required in the future, as the projects go. For now, you can contribute freely under MIT.

## Maintainers

This project is maintained by the team at *Sideways Experiments*.

If you're unsure about any part of the contribution process, feel free to open a discussion on our [Discord server](https://discord.gg/G49RUZ9F2N), or by sending an email directly to [contact@sideways-experiments.com](mailto:contact@sideways-experiments.com).

---

Thank you again for being part of the project!