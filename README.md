# Antkeeper

Antkeeper is an ant colony simulation game. This repository contains all of the source code to Antkeeper. The game data, however, is proprietary and resides in a closed-source Git submodule.

## Getting Started

### Prerequisites

* Git
* CMake
* SDL 2
* FreeType 2

### Download

Use Git to download the `antkeeper` repository and its submodules:

	git clone --recursive https://github.com/cjhoward/antkeeper.git antkeeper

### Configuration

Antkeeper uses the CMake build system for configuration.

	cd antkeeper
	cmake . -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DPLATFORM=win32 -DLANGUAGE=en-us

### Building

	cmake --build .

4. Testing

	cmake --build . --target run

5. Cleaning

First perform a dry run to check what will be deleted:

	git clean -d -f -x -n
	
If there are no issues, clean:

	git clean -d -f -x

## License

The source code for Antkeeper is licensed under the GNU General Public License, version 3. See [COPYING](./COPYING) for details.
