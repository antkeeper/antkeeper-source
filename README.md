<!--
SPDX-FileCopyrightText: 2023 C. J. Howard
SPDX-License-Identifier: CC-BY-SA-4.0
-->

# Antkeeper Source

[![build status](https://github.com/antkeeper/antkeeper-source/actions/workflows/build.yml/badge.svg)](https://github.com/antkeeper/antkeeper-source/actions/workflows/build.yml)
[![code quality](https://app.codacy.com/project/badge/Grade/ec1d9f614fdf4d5b8effa6b7b72b3d5e)](https://app.codacy.com/gh/antkeeper/antkeeper-source/dashboard)
[![chat](https://img.shields.io/discord/547138509610156036?logo=discord)](https://discord.gg/ptwHV4T)

Antkeeper is an open-source 3D ant colony simulation game currently in development for Windows and Linux. Antkeeper is the passion project of solo indie game developer and myrmecophile [C. J. Howard](https://github.com/cjhoward).

Get notified when the game is released by joining the mailing list at <https://antkeeper.com/> or watching this repository. 

> [!NOTE]
> Antkeeper source code is licensed under [GPL-3.0-or-later](./LICENSES/GPL-3.0-or-later.txt), but the game data required to play Antkeeper is proprietary, and can be obtained by purchasing a copy of the game when officially released.

## Table of Contents

-   [Building](#building)
    -   [Windows](#windows)
-   [Documentation](#documentation)
-   [Contributing](#contributing)
-   [Authors](#authors)
-   [License](#license)

## Building

### Windows

Requirements:

-   [Git](https://git-scm.com/downloads)
-   [CMake](https://cmake.org/download/)>=3.27
-   [Build Tools for Visual Studio](https://visualstudio.microsoft.com/downloads/)>=2022
-   [Python](https://www.python.org/downloads/)>=3.11.0
-   [Jinja2](https://pypi.org/project/Jinja2/)>=2.7,<4.0

Configure and build a Windows 64-bit release:

```bash
git clone https://github.com/antkeeper/antkeeper-source.git && cd antkeeper-source
cmake --preset windows-x64-release
cmake --build --preset windows-x64-release
```

## Testing

Configure and build with `-DANTKEEPER_TEST=ON`, then run the following command to perform tests for a Windows 64-bit release:

```bash
ctest --test-dir build\windows-x64 -C Release
```

## Documentation

Source code documentation can be generated with [Doxygen](https://www.doxygen.nl/download.html). [Graphviz](https://graphviz.org/download/) can optionally be used to generate dependency graphs.

The following commands configure and build the source code documentation:

```bash
cmake --preset windows-x64-release -DANTKEEPER_BUILD_DOCS=ON
cmake --build --preset windows-x64-release --target docs
```

Generated HTML documentation will be located at `build/<platform>/docs/doxygen/html/index.html`.

## Contributing

Contributions are welcome! Feel free to [open an issue](https://github.com/antkeeper/antkeeper-source/issues) or [submit a pull request](https://github.com/antkeeper/antkeeper-source/pulls).

See the [contribution guide](./docs/CONTRIBUTING.md) for more details on how to contribute.

## Authors

-   [C. J. Howard](https://github.com/cjhoward)

## License

[![REUSE compliance](https://github.com/antkeeper/antkeeper-source/actions/workflows/reuse.yml/badge.svg)](https://github.com/antkeeper/antkeeper-source/actions/workflows/reuse.yml)

-   Antkeeper source code is licensed under [GPL-3.0-or-later](./LICENSES/GPL-3.0-or-later.txt).
-   Antkeeper translations are licensed under [CC-BY-SA-4.0](./LICENSES/CC-BY-SA-4.0.txt).
-   Antkeeper game data is proprietary.
