# Arduino library protocols

This document lists the operations to run, implementations to write and tools to use for specific cases that may happen during the development of an Arduino library.

> The goal of this document is to be as exhaustive as possible, leaving no doubt when it comes to add/use something in a project. Please report any missing part or need for more details.

## Register the library in the Library Manager

Makes the library installable from the Arduino IDE Library Manager and with `arduino-cli lib install`, as described in the install instructions of the [`README.md`](./README.md). This is done once per library.

### Before submitting

- The repository is public.
- The [template setup](./TEMPLATE.md) is complete, so the library has its final `name`: it can't be changed once registered.
- `task lint` passes. Its `submit` mode also checks that no registered library already uses the same `name`.
- The latest tag passes `task lint` too: the Library Manager only reads tagged versions, never the development branches. If the fixes needed for the previous point came after the latest tag, release them first (see [below](#fix-a-rejected-submission)).

### Submit

Follow the [registry instructions](https://github.com/arduino/library-registry#adding-a-library-to-library-manager): fork [arduino/library-registry](https://github.com/arduino/library-registry), add the URL of the repository home page (e.g. `https://github.com/<your-username>/<your-repo>`) to `repositories.txt`, and open a pull request.

A bot checks the submission within a few minutes and comments on the pull request. If everything is fine, it merges the pull request, and the library is available in the Library Manager within a day.

### Fix a rejected submission

If the bot reports a problem with the library:

1. Fix it, and commit the fix with a `fix` type (other types like `chore` don't trigger a release).
1. Push it to the main branch, so the CI releases a new version with a new tag.
1. Comment on the pull request, mentioning `@ArduinoBot`, to run the checks again.

### After the registration

- Switch `LIBRARY_MANAGER_MODE` to `update` in [`Taskfile.yml`](./Taskfile.yml): `submit` mode would now fail, as the library name is already registered (by this library).
- Nothing else to do for new versions: the Library Manager checks for new tags every hour, and each release of the CI creates one.

> If a release doesn't show up in the Library Manager, check the indexer logs at `http://downloads.arduino.cc/libraries/logs/github.com/<your-username>/<your-repo>/`. Releases that don't pass the checks are rejected there.

## Generate the documentation and publish it on GitHub Pages

Generates an HTML reference of the library from its Doxygen comments with [Doxygen](https://www.doxygen.nl), and publishes it on GitHub Pages after each release on the main branch.

### 1. Install Doxygen

Add it to the requirements table of the [`README.md`](./README.md):

```markdown
| [Doxygen](https://www.doxygen.nl) | Generating the documentation. | [Downloads](https://www.doxygen.nl/download.html) (`winget install DimitriVanHeesch.Doxygen` on Windows) |
```

### 2. Configure Doxygen

Create a `Doxyfile` at the root of the project, with only the options that differ from the defaults:

```ini
# Only the options that differ from the defaults, see https://www.doxygen.nl/manual/config.html
PROJECT_NAME           = <library name>
# Set by the docs task of Taskfile.yml
PROJECT_NUMBER         = $(LIBRARY_VERSION)
PROJECT_BRIEF          = "<sentence of library.properties>"
# Doxygen only creates the last folder of the path, so build/docs is split in two
OUTPUT_DIRECTORY       = build
HTML_OUTPUT            = docs
INPUT                  = README.md src examples
RECURSIVE              = YES
FILE_PATTERNS          = *.h *.hpp *.c *.cpp *.ino
# Doxygen doesn't know .ino files, which are C++
EXTENSION_MAPPING      = ino=C++
USE_MDFILE_AS_MAINPAGE = README.md
GENERATE_LATEX         = NO
```

> The `@todo` comments of the code are listed in a "Todo List" page. The ones left in the `README.md` produce warnings, as Doxygen doesn't expect them in Markdown files.

### 3. Add a task

Add this task to [`Taskfile.yml`](./Taskfile.yml):

```yaml
  docs:
    desc: Generate the documentation in build/docs
    env:
      # Latest version tag, followed by the number of commits since, if any (e.g. 0.2.0-3-g1a2b3c4)
      LIBRARY_VERSION:
        sh: git describe --tags --always
    cmds:
      - doxygen
```

Run `task docs`, then open `build/docs/index.html` to check the result.

### 4. Publish from the CI

In the repository settings on GitHub, open **Pages** and set **Source** to **GitHub Actions**.

Then add this job to [`.github/workflows/ci.yml`](./.github/workflows/ci.yml). It runs after the `release` job, so only on the main branch:

```yaml
  docs:
    needs: release
    runs-on: ubuntu-latest
    permissions:
      pages: write
      id-token: write
    environment:
      name: github-pages
      url: ${{ steps.deploy.outputs.page_url }}
    steps:
      - name: Checkout
        uses: actions/checkout@v7
        with:
          # Latest commit of the branch, so it includes the version commit of the release job
          ref: ${{ github.ref }}
          # The whole history and the tags, to find the version
          fetch-depth: 0

      - name: Install Task
        uses: arduino/setup-task@v3
        with:
          repo-token: ${{ secrets.GITHUB_TOKEN }}

      - name: Install Doxygen
        run: sudo apt-get update && sudo apt-get install -y doxygen

      - name: Generate the documentation
        run: task docs

      - name: Upload the documentation
        uses: actions/upload-pages-artifact@v5
        with:
          path: build/docs

      - name: Deploy the documentation
        id: deploy
        uses: actions/deploy-pages@v5
```

### 5. Link the documentation

Add the link to the documentation in the [`README.md`](./README.md), for example in the usage section:

```markdown
See the [documentation](https://<your-username>.github.io/<your-repo>/) for the complete reference.
```
