# SPDX-FileCopyrightText: 2023 C. J. Howard
# SPDX-License-Identifier: GPL-3.0-or-later

# Prevent in-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "In-source builds prohibited.")
endif()

cmake_minimum_required(VERSION 3.27)
include(FetchContent)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

project(Antkeeper
	VERSION 0.0.1
	DESCRIPTION "ant colony simulation game"
	HOMEPAGE_URL "https://antkeeper.com/"
	LANGUAGES CXX C
)

option(ANTKEEPER_ASAN "Enable address sanitizer" OFF)

set(APPLICATION_NAME ${PROJECT_NAME})
string(TOLOWER "${APPLICATION_NAME}" APPLICATION_SLUG)
set(APPLICATION_VERSION ${PROJECT_VERSION})
set(APPLICATION_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(APPLICATION_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(APPLICATION_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(APPLICATION_AUTHOR "C. J. Howard")

if(MSVC)
	# Use static multithreaded runtime on MSVC
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

if (MSVC)
	# Enable parallel building on MSVC
	add_compile_options(/MP)
	
	# Optionally enable address sanitizer
	if (ANTKEEPER_ASAN)
		add_compile_options(/fsanitize=address)
	endif()
endif()

# Find Python3 interpreter
find_package(Python3
	COMPONENTS
		Interpreter
	REQUIRED
)

# Find packages
find_package(OpenGL REQUIRED)

# Fetch dependencies
FetchContent_Declare(cxxopts
	GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
	GIT_TAG eb787304d67ec22f7c3a184ee8b4c481d04357fd # v3.1.1
)
FetchContent_Declare(dr_libs
	GIT_REPOSITORY https://github.com/mackron/dr_libs.git
	GIT_TAG 9eed1be421749ba68a87e5b4c3b10858f8580689 # v0.13.13
	GIT_SUBMODULES ""
)
FetchContent_Declare(entt
	GIT_REPOSITORY https://github.com/skypjack/entt.git
	GIT_TAG e4ccb878f47245a319704912435d3c89f34ad6be # v3.10.3
)
FetchContent_Declare(freetype
	GIT_REPOSITORY https://github.com/freetype/freetype.git
	GIT_TAG 920c5502cc3ddda88f6c7d85ee834ac611bb11cc # v2.13.2
)
FetchContent_Declare(glad
	GIT_REPOSITORY https://github.com/Dav1dde/glad.git
	GIT_TAG d08b1aa01f8fe57498f04d47b5fa8c48725be877 # v2.0.4
	SOURCE_SUBDIR cmake
)
FetchContent_Declare(nlohmann-json
	GIT_REPOSITORY https://github.com/nlohmann/json.git
	GIT_TAG 9cca280a4d0ccf0c08f47a99aa71d1b0e52f8d03 # v3.11.3
)
FetchContent_Declare(openal-soft
	GIT_REPOSITORY https://github.com/kcat/openal-soft.git
	GIT_TAG d3875f333fb6abe2f39d82caca329414871ae53b # v1.23.1
)
FetchContent_Declare(physfs
	GIT_REPOSITORY https://github.com/icculus/physfs.git
	GIT_TAG eb3383b532c5f74bfeb42ec306ba2cf80eed988c # v3.2.0
)
FetchContent_Declare(SDL2
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG 15ead9a40d09a1eb9972215cceac2bf29c9b77f6 # v2.28.5
)
FetchContent_Declare(stb
	GIT_REPOSITORY https://github.com/nothings/stb.git
	GIT_TAG 03f50e343d796e492e6579a11143a085429d7f5d # stb_image v2.28, stb_image_write v1.16
)
FetchContent_Declare(tinyexr
	GIT_REPOSITORY https://github.com/syoyo/tinyexr.git
	GIT_TAG 6e8cac308cdf4d717078f3f37c4aa39bf3b356b4 # v1.0.7
)

# Configure cxxopts
set(CXXOPTS_BUILD_EXAMPLES OFF)
set(CXXOPTS_BUILD_TESTS OFF)

# Configure FreeType
set(BUILD_SHARED_LIBS "false")
set(FT_DISABLE_ZLIB ON)
set(FT_DISABLE_BZIP2 ON)
set(FT_DISABLE_PNG ON)
set(FT_DISABLE_HARFBUZZ ON)
set(FT_DISABLE_BROTLI ON)

# Configure OpenAL Soft
set(LIBTYPE "STATIC")
set(FORCE_STATIC_VCRT ON)
set(ALSOFT_UTILS OFF)
set(ALSOFT_EXAMPLES OFF)

# Configure PhysicsFS
set(PHYSFS_ARCHIVE_ZIP ON)
set(PHYSFS_ARCHIVE_7Z OFF)
set(PHYSFS_ARCHIVE_GRP OFF)
set(PHYSFS_ARCHIVE_WAD OFF)
set(PHYSFS_ARCHIVE_HOG OFF)
set(PHYSFS_ARCHIVE_MVL OFF)
set(PHYSFS_ARCHIVE_QPAK OFF)
set(PHYSFS_ARCHIVE_SLB OFF)
set(PHYSFS_ARCHIVE_ISO9660 OFF)
set(PHYSFS_ARCHIVE_VDF OFF)
set(PHYSFS_BUILD_STATIC ON)
set(PHYSFS_BUILD_SHARED OFF)
set(PHYSFS_BUILD_TEST OFF)
set(PHYSFS_BUILD_DOCS OFF)

# Configure SDL
set(SDL_ASSERTIONS "disabled")
set(SDL_ATOMIC OFF)
set(SDL_AUDIO OFF)
set(SDL_CPUINFO OFF)
set(SDL_DIRECTX OFF)
set(SDL_EVENTS ON)
set(SDL_FILE OFF)
set(SDL_FILESYSTEM OFF)
set(SDL_FORCE_STATIC_VCRT ON)
set(SDL_HAPTIC OFF)
set(SDL_HIDAPI ON)
set(SDL_JOYSTICK ON)
set(SDL_LIBC ON)
set(SDL_LOADSO ON)
set(SDL_LOCALE OFF)
set(SDL_MISC OFF)
set(SDL_OPENGL ON)
set(SDL_OPENGLES OFF)
set(SDL_POWER OFF)
set(SDL_RENDER OFF)
set(SDL_SENSOR OFF)
set(SDL_SHARED OFF)
set(SDL_STATIC ON)
set(SDL_TEST OFF)
set(SDL_THREADS ON)
set(SDL_TIMERS OFF)
set(SDL_VIDEO ON)
set(SDL_VULKAN OFF)
set(SDL_WASAPI OFF)

# Configure TinyEXR
set(TINYEXR_BUILD_SAMPLE OFF)
set(TINYEXR_USE_MINIZ OFF)
add_compile_definitions(
	TINYEXR_USE_MINIZ=0
	TINYEXR_USE_STB_ZLIB=1
)

# Make dependencies available
FetchContent_MakeAvailable(
	cxxopts
	dr_libs
	entt
	freetype
	glad
	nlohmann-json
	openal-soft
	physfs
	SDL2
	stb
	tinyexr
)

# Build dr_wav static library
configure_file(${dr_libs_SOURCE_DIR}/dr_wav.h ${dr_libs_BINARY_DIR}/src/dr_wav.c COPYONLY)
file(COPY ${dr_libs_SOURCE_DIR}/dr_wav.h DESTINATION ${dr_libs_BINARY_DIR}/include)
add_library(dr_wav STATIC ${dr_libs_BINARY_DIR}/src/dr_wav.c)
target_compile_definitions(dr_wav
	PRIVATE
		DR_WAV_IMPLEMENTATION
		DR_WAV_NO_CONVERSION_API
		DR_WAV_NO_STDIO
)
target_include_directories(dr_wav
	PUBLIC
		$<BUILD_INTERFACE:${dr_libs_BINARY_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

# Build stb static library
configure_file(${stb_SOURCE_DIR}/stb_image.h ${stb_BINARY_DIR}/src/stb_image.c COPYONLY)
configure_file(${stb_SOURCE_DIR}/stb_image_write.h ${stb_BINARY_DIR}/src/stb_image_write.c COPYONLY)
file(COPY
	${stb_SOURCE_DIR}/stb_image.h
	${stb_SOURCE_DIR}/stb_image_write.h
	DESTINATION ${stb_BINARY_DIR}/include/stb
)
add_library(stb STATIC
	${stb_BINARY_DIR}/src/stb_image.c
	${stb_BINARY_DIR}/src/stb_image_write.c
)
target_compile_definitions(stb
	PRIVATE
		STB_IMAGE_IMPLEMENTATION
		STBI_NO_BMP
		STBI_NO_PSD
		STBI_NO_GIF
		STBI_NO_HDR
		STBI_NO_PIC
		STBI_NO_PNM
		STBI_NO_LINEAR
		STBI_NO_STDIO
		STB_IMAGE_WRITE_IMPLEMENTATION
)
target_include_directories(stb
	PUBLIC
		$<BUILD_INTERFACE:${stb_BINARY_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

# Generate OpenGL loader with glad
glad_add_library(glad STATIC
	API gl:core=4.6
	EXTENSIONS
		GL_EXT_texture_filter_anisotropic
)

# Make tinyexr.h available
file(COPY ${tinyexr_SOURCE_DIR}/tinyexr.h DESTINATION ${tinyexr_BINARY_DIR}/include)

# Build antkeeper-data module (if exists)
# if(EXISTS ${PROJECT_SOURCE_DIR}/modules/antkeeper-data/CMakeLists.txt)
	# ExternalProject_Add(antkeeper-data
		# SOURCE_DIR ${MODULE_SOURCE_DIR}/antkeeper-data
		# BINARY_DIR ${MODULE_BUILD_DIR}/antkeeper-data
		# INSTALL_DIR ${PACKAGE_INSTALL_DIR}
		# CMAKE_ARGS
			# "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
			# "-DCMAKE_INSTALL_PREFIX=${PACKAGE_INSTALL_DIR}"
			# "-DCMAKE_PREFIX_PATH=${MODULE_INSTALL_DIR}"
			# "-DAPPLICATION_NAME=${APPLICATION_NAME}"
			# "-DAPPLICATION_VERSION=${APPLICATION_VERSION}"
			# "-DAPPLICATION_AUTHOR=${APPLICATION_AUTHOR}"
			# "-DCMAKE_TOOLCHAIN_FILE='${CMAKE_TOOLCHAIN_FILE}'"
			# "-DBLENDER=${BLENDER}"
		# BUILD_ALWAYS 1)
# endif()

# Generate configuration header file
configure_file(${PROJECT_SOURCE_DIR}/src/engine/config.hpp.in ${PROJECT_BINARY_DIR}/src/engine/config.hpp)

# Collect platform-independent source files
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
	${PROJECT_SOURCE_DIR}/src/*.cpp
	EXCLUDE_DIR "${PROJECT_SOURCE_DIR}/src/game/platform"
)

# Collect module files
# file(GLOB_RECURSE MODULE_FILES CONFIGURE_DEPENDS
	# ${PROJECT_SOURCE_DIR}/src/*.ixx
	# ${PROJECT_SOURCE_DIR}/src/*.cxx
# )

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	
	# Collect Windows-specific source files
	file(GLOB_RECURSE WINDOWS_SOURCE_FILES CONFIGURE_DEPENDS
		${PROJECT_SOURCE_DIR}/src/game/platform/windows/*.cpp
	)
	list(APPEND SOURCE_FILES ${WINDOWS_SOURCE_FILES})
	
	# Generate Windows icon resource file
	set(ICON_FILE "${PROJECT_SOURCE_DIR}/res/data/src/icons/antkeeper.ico")
	if(EXISTS "${ICON_FILE}")
		configure_file(${PROJECT_SOURCE_DIR}/res/windows/icon.rc.in ${PROJECT_BINARY_DIR}/res/windows/icon.rc)
		list(APPEND SOURCE_FILES "${PROJECT_BINARY_DIR}/res/windows/icon.rc")
	endif()
	
	# Generate Windows version-information resource file
	configure_file(${PROJECT_SOURCE_DIR}/res/windows/version.rc.in ${PROJECT_BINARY_DIR}/res/windows/version.rc)
	list(APPEND SOURCE_FILES "${PROJECT_BINARY_DIR}/res/windows/version.rc")
	
	# Make executable DPI-aware on Windows
	list(APPEND SOURCE_FILES "${PROJECT_SOURCE_DIR}/res/windows/dpi-aware.manifest")
	
endif()

# Add executable target
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Set executable module files
# target_sources(${PROJECT_NAME}
	# PUBLIC
		# FILE_SET CXX_MODULES FILES ${MODULE_FILES}
# )

# Set target properties
set_target_properties(${PROJECT_NAME}
	PROPERTIES
		OUTPUT_NAME $<LOWER_CASE:${PROJECT_NAME}>
		RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<LOWER_CASE:${PROJECT_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_GENERATOR_PLATFORM}-$<CONFIG>>
		COMPILE_WARNING_AS_ERROR ON
		CXX_STANDARD 23
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

# Set compile definitions
target_compile_definitions(${PROJECT_NAME}
	PRIVATE
		$<$<NOT:$<CONFIG:Debug>>:N>DEBUG
		_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
		_SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING
		
		# Set minimum Windows version (Windows XP)
		$<$<PLATFORM_ID:Windows>:
			WINVER=0x0501
			_WIN32_WINNT=0x0501
		>
)

# Set compile options
target_compile_options(${PROJECT_NAME}
	PRIVATE
		$<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -pedantic>
		$<$<CXX_COMPILER_ID:MSVC>:
			/W4 /we4265 /we5204 /we5263 /we4946 /we4822 /we4355 /we4061 /we4062 /EHsc /GR-
			$<$<NOT:$<CONFIG:Debug>>:/GL>
			$<$<CONFIG:Release>:/O2 /Ob3 /Qfast_transcendentals /GS->
		>
)

# Set link options
target_link_options(${PROJECT_NAME}
	PRIVATE
		$<$<PLATFORM_ID:Windows>:
			$<$<CXX_COMPILER_ID:MSVC>:
				$<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>
				$<$<NOT:$<CONFIG:Debug>>:/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /OPT:REF /OPT:ICF /INCREMENTAL:NO /LTCG>
			>
			$<$<CXX_COMPILER_ID:GNU,Clang>:
				$<$<CONFIG:Debug>:-mconsole>
				$<$<NOT:$<CONFIG:Debug>>:-mwindows --entry=mainCRTStartup>
			>
		>
		$<$<CXX_COMPILER_ID:GNU,Clang>:
			-static
			$<$<NOT:$<CONFIG:Debug>>:-flto>
		>
)

# Set include directories
target_include_directories(${PROJECT_NAME}
	PRIVATE
		${PROJECT_SOURCE_DIR}/src
		${PROJECT_BINARY_DIR}/src
		${tinyexr_BINARY_DIR}/include
)

# Link to dependencies
target_link_libraries(${PROJECT_NAME}
	PRIVATE
		dr_wav
		stb
		tinyexr
		glad
		EnTT
		cxxopts
		nlohmann_json
		SDL2::SDL2-static
		SDL2::SDL2main
		OpenAL
		physfs-static
		freetype
		${OPENGL_gl_LIBRARY}
)

# Determine data output directory
get_target_property(RUNTIME_OUTPUT_DIRECTORY ${PROJECT_NAME} RUNTIME_OUTPUT_DIRECTORY)
set(DATA_OUTPUT_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}/data")

# Add localization CMakeLists
add_subdirectory(${PROJECT_SOURCE_DIR}/res/localization)

# Add documentation CMakeLists
add_subdirectory(${PROJECT_SOURCE_DIR}/docs)
