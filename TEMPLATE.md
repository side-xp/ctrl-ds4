# Template setup guide

> **Delete this file once you've completed the steps below.**

## 1. Create your repository

Click **Use this template** on GitHub to create a new repository, then clone it locally:

```bash
git clone https://github.com/<your-username>/<your-repo>.git
cd <your-repo>
```

## 2. Initialize the project

- Replace `@todo` in [`LICENSE.md`](./LICENSE.md) by the project name
- Replace the `@todo` values in [`README.md`](./README.md), and the repository URL of the latest release link
- Replace the properties in [`library.properties`](./library.properties):
  - the `@todo` values
  - `category`, using one of the [allowed values](https://arduino.github.io/arduino-cli/latest/library-specification/#libraryproperties-file-format)
  - `url`, with the URL of your repository
- Rename `CtrlTemplate` to the library name everywhere:
  - the `name` in [`library.properties`](./library.properties)
  - the class in [`src/CtrlTemplate.h`](./src/CtrlTemplate.h) and [`src/CtrlTemplate.cpp`](./src/CtrlTemplate.cpp)
  - the file names themselves
  - the `#include` and usage in [`examples/Basic/Basic.ino`](./examples/Basic/Basic.ino)
  - the install and usage instructions in [`README.md`](./README.md)

- Adjust `BOARDS` in [`Taskfile.yml`](./Taskfile.yml) to the architectures supported by the library (see `architectures` in [`library.properties`](./library.properties))

> The library name must not contain "library" or "Arduino": the Library Manager rejects it as superfluous.

> The example folder and the `.ino` file must keep the same name, or the Arduino IDE won't open it.

## 3. Tag the first version

Once the steps above are committed, tag that commit with the version of [`library.properties`](./library.properties) and push the tag:

```bash
git tag 0.1.0
git push origin 0.1.0
```

> semantic-release computes the next version from the latest version tag. Without that tag, the first release would be 1.0.0.

## 4. Add what the project needs

The template only contains the base setup. See [`protocols.md`](protocols.md) for recipes.
