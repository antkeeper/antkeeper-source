# Antkeeper

Antkeeper is an ant colony simulation game. This repository contains all of the source code to Antkeeper. The game data, however, is proprietary and resides in a closed-source Git submodule.

## Download

Use Git to download the `antkeeper` repository and its submodules:

	git clone --recursive https://github.com/cjhoward/antkeeper.git antkeeper

## Configuration & Building

CMake is required to configure and build the application. Depending on the target build platform, CMake should be invoked from one of the following directories:

	build/linux32 // 32-bit GNU/Linux application
	build/linux64 // 64-bit GNU/Linux application
	build/win32   // 32-bit Windows application
	build/win64   // 64-bit Windows application

The following arguments may be passed to CMake during configuration:

	-DCMAKE_BUILD_TYPE // [Debug, Release]
	-DPLATFORM         // [linux32, linux64, win32, win64]
	-DLANGUAGE         // [en-us, zh-cn]
	-DSTANDALONE       // [OFF, ON]

### GNU/Linux

Building on GNU/Linux requires CMake, GCC, G++, and GNU Make. Open a terminal in the project root directory and run the following commands:

	cd build/linux64
	cmake ../.. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=...
	cmake --build .

### Windows

Building on Windows requires CMake and Visual Studio 2017. Additionally, [NSIS](http://nsis.sourceforge.net/) is required if you want to build a distributable installer program. In order to correctly build for your target architecture, you must use the `x86 Native Tools Command Prompt` or the `x64 Native Tools Command Prompt` for 32-bit and 64-bit applications, respectively. Then navigate to the project root directory and run the following commands:

	cd build/win64
	cmake ..\.. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=...
	cmake --build .

## Testing

After building, a standalone version of the application will be located somewhere in the `bin` directory, according to the build type, build platform, and version string. This application can be executed with the following command:

	cmake --build . --target run

## Distribution

The built application can be packaged into a distributable format with the following command:

	cmake --build . --target package

The resulting package will be located in the `dist` directory.

## Contributing

If any changes have been made to the submodules, commit those first. Each submodule can then be updated to their latest commits with the following command:

	git submodule update --recursive --remote

## License

The source code for Antkeeper is licensed under the GNU General Public License, version 3. See [COPYING](./COPYING) for details.
