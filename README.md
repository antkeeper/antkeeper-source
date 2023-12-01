<!--
SPDX-FileCopyrightText: 2023 C. J. Howard
SPDX-License-Identifier: CC-BY-SA-4.0
-->

<div align="center">

# Antkeeper

[![build status](https://github.com/antkeeper/antkeeper/actions/workflows/build.yml/badge.svg)](https://github.com/antkeeper/antkeeper/actions/workflows/build.yml)
[![code quality](https://app.codacy.com/project/badge/Grade/ec1d9f614fdf4d5b8effa6b7b72b3d5e)](https://app.codacy.com/gh/antkeeper/antkeeper/dashboard)
[![documentation](https://img.shields.io/badge/docs-latest-brightgreen.svg?style=flat)](https://antkeeper.github.io/antkeeper/)
[![chat](https://img.shields.io/discord/547138509610156036?logo=discord)](https://discord.gg/ptwHV4T)
[![REUSE compliance](https://github.com/antkeeper/antkeeper/actions/workflows/reuse.yml/badge.svg)](https://github.com/antkeeper/antkeeper/actions/workflows/reuse.yml)

</div>

Antkeeper is a 3D ant colony simulation game currently in development for Windows, Mac, and Linux.

Head over to [antkeeper.com](https://antkeeper.com/) if you're interested in following the development of the game or purchasing a copy when it's released. Antkeeper is an indie game with a single developer, so feel free to reach out to me personally with any questions, comments, or feedback you may have.

## Table of Contents

* [Building](#building)
* [Maintainers](#maintainers)
* [Contributing](#contributing)
* [License](#license)

## Building

Requirements:

* [Git](https://git-scm.com/downloads)
* [CMake](https://cmake.org/download/)>=3.27
* [Build Tools for Visual Studio](https://visualstudio.microsoft.com/downloads/)>=2022
* [Python](https://www.python.org/downloads/)>=3.11.0
* [Jinja2](https://pypi.org/project/Jinja2/)>=2.7,<4.0

Configure and build a 64-bit Windows release:

```bash
git clone https://github.com/antkeeper/antkeeper.git && cd antkeeper
cmake --preset win64-release
cmake --build --preset win64-release
```

## Maintainers

* [C. J. Howard](https://github.com/cjhoward)

## Contributing

Antkeeper is an open-source project, and contributions are welcome.

See [CONTRIBUTING.md](./CONTRIBUTING.md) for details.

## License

* Antkeeper source code is licensed under [GPL-3.0-or-later](./LICENSES/GPL-3.0-or-later.txt).
* Antkeeper translation texts are licensed under [CC-BY-SA-4.0](./LICENSES/CC-BY-SA-4.0.txt).
* Antkeeper data package is proprietary.
