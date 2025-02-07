# SPDX-FileCopyrightText: 2025 C. J. Howard
# SPDX-License-Identifier: GPL-3.0-or-later

# Prevent in-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "In-source builds prohibited.")
endif()

cmake_minimum_required(VERSION 3.25)
include(FetchContent)

# Suppress CMP0077 policy warning
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# Enable compile_commands.json generation
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Restrict configuration types to Debug and Release
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

project(Antkeeper
	VERSION 0.0.1
	DESCRIPTION "ant colony simulation game"
	HOMEPAGE_URL "https://antkeeper.com/"
	LANGUAGES CXX C
)
set(PROJECT_AUTHOR "C. J. Howard")
set(PROJECT_IDENTIFIER "com.cjhoward.antkeeper")
set(PROJECT_COPYRIGHT "Copyright (c) 2025 ${PROJECT_AUTHOR}")

option(ANTKEEPER_ASAN "Enable address sanitizer" OFF)
option(ANTKEEPER_TEST "Enable building tests" OFF)

if(MSVC)
	# Use static multithreaded runtime on MSVC
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

	# Enable parallel building on MSVC
	add_compile_options(/MP)

	# Optionally enable address sanitizer
	if(ANTKEEPER_ASAN)
		add_compile_options(/fsanitize=address)
	endif()
endif()

# Find Python3 interpreter
find_package(Python3 COMPONENTS Interpreter REQUIRED)

# Find packages
find_package(OpenGL REQUIRED)

# Fetch dependencies
set(FETCHCONTENT_QUIET TRUE)

FetchContent_Declare(cxxopts
	GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
	GIT_TAG eb787304d67ec22f7c3a184ee8b4c481d04357fd # v3.1.1
	GIT_PROGRESS TRUE
)
FetchContent_Declare(dr_libs
	GIT_REPOSITORY https://github.com/mackron/dr_libs.git
	GIT_TAG 9eed1be421749ba68a87e5b4c3b10858f8580689 # v0.13.13
	GIT_PROGRESS TRUE
	GIT_SUBMODULES ""
)
FetchContent_Declare(entt
	GIT_REPOSITORY https://github.com/skypjack/entt.git
	GIT_TAG e4ccb878f47245a319704912435d3c89f34ad6be # v3.10.3
	GIT_PROGRESS TRUE
)
FetchContent_Declare(freetype
	GIT_REPOSITORY https://github.com/freetype/freetype.git
	GIT_TAG 920c5502cc3ddda88f6c7d85ee834ac611bb11cc # v2.13.2
	GIT_PROGRESS TRUE
)
FetchContent_Declare(glad
	GIT_REPOSITORY https://github.com/Dav1dde/glad.git
	GIT_TAG d08b1aa01f8fe57498f04d47b5fa8c48725be877 # v2.0.4
	GIT_PROGRESS TRUE
	SOURCE_SUBDIR cmake
)
FetchContent_Declare(nlohmann-json
	GIT_REPOSITORY https://github.com/nlohmann/json.git
	GIT_TAG 9cca280a4d0ccf0c08f47a99aa71d1b0e52f8d03 # v3.11.3
	GIT_PROGRESS TRUE
)
FetchContent_Declare(openal-soft
	GIT_REPOSITORY https://github.com/kcat/openal-soft.git
	GIT_TAG b621b5fce739525418f36e0474a46f9e1cab5e64 # v1.24.2
	GIT_PROGRESS TRUE
)
FetchContent_Declare(physfs
	GIT_REPOSITORY https://github.com/icculus/physfs.git
	GIT_TAG eb3383b532c5f74bfeb42ec306ba2cf80eed988c # v3.2.0
	GIT_PROGRESS TRUE
)
FetchContent_Declare(SDL3
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG b5c3eab6b447111d3c7879bb547b80fb4abd9063 # v3.2.4
	GIT_PROGRESS TRUE
)
FetchContent_Declare(stb
	GIT_REPOSITORY https://github.com/nothings/stb.git
	GIT_TAG 5c205738c191bcb0abc65c4febfa9bd25ff35234 # stb_image v2.30, stb_image_write v1.16
	GIT_PROGRESS TRUE
)
FetchContent_Declare(tinyexr
	GIT_REPOSITORY https://github.com/syoyo/tinyexr.git
	GIT_TAG 6e8cac308cdf4d717078f3f37c4aa39bf3b356b4 # v1.0.7
	GIT_PROGRESS TRUE
)
FetchContent_Declare(ogg
	GIT_REPOSITORY https://github.com/xiph/ogg.git
	GIT_TAG e1774cd77f471443541596e09078e78fdc342e4f # v1.3.5
	GIT_PROGRESS TRUE
	OVERRIDE_FIND_PACKAGE
)
FetchContent_Declare(vorbis
	GIT_REPOSITORY https://github.com/xiph/vorbis.git
	GIT_TAG 0657aee69dec8508a0011f47f3b69d7538e9d262 # v1.3.7
	GIT_PROGRESS TRUE
)
FetchContent_Declare(lua
	GIT_REPOSITORY https://github.com/lua/lua.git
	GIT_TAG 1ab3208a1fceb12fca8f24ba57d6e13c5bff15e3 # v5.4.7
	GIT_PROGRESS TRUE
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
set(SDL_SHARED OFF)
set(SDL_STATIC ON)
set(SDL_TEST_LIBRARY OFF)
set(SDL_ASSERTIONS "disabled")

# SDL subsystems
set(SDL_AUDIO OFF)
set(SDL_VIDEO ON)
set(SDL_GPU OFF)
set(SDL_RENDER OFF)
set(SDL_CAMERA OFF)
set(SDL_JOYSTICK ON)
set(SDL_HAPTIC ON)
set(SDL_HIDAPI ON)
set(SDL_POWER OFF)
set(SDL_SENSOR ON)
set(SDL_DIALOG OFF)

# SDL options
set(SDL_ATOMIC OFF)
set(SDL_CPUINFO OFF)
set(SDL_DIRECTX OFF)
set(SDL_EVENTS ON)
set(SDL_FILE OFF)
set(SDL_FILESYSTEM OFF)
set(SDL_LIBC ON)
set(SDL_LOADSO ON)
set(SDL_LOCALE OFF)
set(SDL_MISC OFF)
set(SDL_OPENGL ON)
set(SDL_OPENGLES OFF)
set(SDL_THREADS ON)
set(SDL_TIMERS OFF)
set(SDL_VULKAN OFF)
set(SDL_WASAPI OFF)

# Configure TinyEXR
set(TINYEXR_BUILD_SAMPLE OFF)
set(TINYEXR_USE_MINIZ OFF)
add_compile_definitions(
	TINYEXR_USE_MINIZ=0
	TINYEXR_USE_STB_ZLIB=1
)

# Configure Ogg
set(BUILD_TESTING OFF)

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
	SDL3
	stb
	tinyexr
	ogg
	vorbis
	lua
)

# Build dr_wav static library
configure_file(${dr_libs_SOURCE_DIR}/dr_wav.h ${dr_libs_BINARY_DIR}/src/dr_wav.c COPYONLY)
file(COPY ${dr_libs_SOURCE_DIR}/dr_wav.h DESTINATION ${dr_libs_BINARY_DIR}/include)
add_library(dr_wav STATIC ${dr_libs_BINARY_DIR}/src/dr_wav.c)
target_compile_definitions(dr_wav
	PRIVATE
		DR_WAV_IMPLEMENTATION
		# DR_WAV_NO_CONVERSION_API
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

# Build lua static library
add_library(lua STATIC ${lua_SOURCE_DIR}/onelua.c)
target_compile_definitions(lua
	PRIVATE
		MAKE_LIB
)
file(COPY ${lua_SOURCE_DIR}/luaconf.h DESTINATION ${lua_BINARY_DIR}/include)
file(COPY ${lua_SOURCE_DIR}/lua.h DESTINATION ${lua_BINARY_DIR}/include)
file(COPY ${lua_SOURCE_DIR}/lualib.h DESTINATION ${lua_BINARY_DIR}/include)
file(COPY ${lua_SOURCE_DIR}/lauxlib.h DESTINATION ${lua_BINARY_DIR}/include)
target_include_directories(lua
	PUBLIC
		$<BUILD_INTERFACE:${lua_SOURCE_DIR}>
		$<INSTALL_INTERFACE:${lua_BINARY_DIR}/include>
)

# Generate OpenGL loader with glad
glad_add_library(glad STATIC
	API gl:core=4.6
	EXTENSIONS
		GL_EXT_texture_filter_anisotropic
)

# Make tinyexr.h available
file(COPY ${tinyexr_SOURCE_DIR}/tinyexr.h DESTINATION ${tinyexr_BINARY_DIR}/include)

# Collect source files
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
	${PROJECT_SOURCE_DIR}/src/*.cpp
)

# Collect header files
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS
	${PROJECT_SOURCE_DIR}/src/*.hpp
	${PROJECT_SOURCE_DIR}/src/*.hpp.in
)

# Collect module files
#file(GLOB_RECURSE MODULE_FILES CONFIGURE_DEPENDS
#	${PROJECT_SOURCE_DIR}/src/*.ixx
#	${PROJECT_SOURCE_DIR}/src/*.cxx
#)

# Filter out platform-specific source and header files
list(FILTER SOURCE_FILES EXCLUDE REGEX "${PROJECT_SOURCE_DIR}/src/game/platform/.*")
list(FILTER HEADER_FILES EXCLUDE REGEX "${PROJECT_SOURCE_DIR}/src/game/platform/.*")

# Generate configuration header files
configure_file(${PROJECT_SOURCE_DIR}/src/engine/config.hpp.in ${PROJECT_BINARY_DIR}/config.hpp.in @ONLY)
file(GENERATE
	OUTPUT ${PROJECT_BINARY_DIR}/$<CONFIG>/src/engine/config.hpp
	INPUT ${PROJECT_BINARY_DIR}/config.hpp.in
)

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
	configure_file(${PROJECT_SOURCE_DIR}/res/windows/version.rc.in ${PROJECT_BINARY_DIR}/version.rc.in @ONLY)
	file(GENERATE
		OUTPUT ${PROJECT_BINARY_DIR}/$<CONFIG>/res/windows/version.rc
		INPUT ${PROJECT_BINARY_DIR}/version.rc.in
	)

	list(APPEND SOURCE_FILES "${PROJECT_BINARY_DIR}/$<CONFIG>/res/windows/version.rc")
	
	# Make executable DPI-aware on Windows
	list(APPEND SOURCE_FILES "${PROJECT_SOURCE_DIR}/res/windows/dpi-aware.manifest")
	
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES} ${HEADER_FILES} ${MODULE_FILES})

# Add executable target
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

# Set additional target sources
target_sources(${PROJECT_NAME}
	PUBLIC
		FILE_SET HEADERS FILES ${HEADER_FILES}
		#FILE_SET CXX_MODULES FILES ${MODULE_FILES}
)

# Set target properties
set_target_properties(${PROJECT_NAME}
	PROPERTIES
		OUTPUT_NAME $<LOWER_CASE:${PROJECT_NAME}>
		RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<LOWER_CASE:${PROJECT_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${ANTKEEPER_BUILD_ARCH}-$<CONFIG>>
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
		
		# Set MSVC-specific compile options
		$<$<CXX_COMPILER_ID:MSVC>:
		
			# Set warning level 4
			/W4
			
			# Treat specific warnings as errors
			/we4061 # enumerator 'identifier' in switch of enum 'enumeration' is not explicitly handled by a case label
			/we4062 # enumerator 'identifier' in switch of enum 'enumeration' is not handled
			/we4265 # 'class' : class has virtual functions, but destructor is not virtual
			/we4355 # 'this' : used in base member initializer list
			/we4822 # local class member function does not have a body
			/we4946 # reinterpret_cast used between related classes
			/we5204 # 'function' : class has virtual functions, but destructor is not virtual
			/we5263 # 'function' : 'noexcept' used with no exception specification

			# Enable C++ exception handling
			/EHsc

			# Disable RTTI
			/GR-

			# Enable experimental modules
			#/experimental:module

			# Set Debug configuration options
			$<$<CONFIG:Debug>:
				$<$<NOT:$<BOOL:${ANTKEEPER_ASAN}>>:
					/ZI    # Enable Edit and Continue
				>
				/Gy        # Enable function-level linking
				/GS        # Enable security checks
				/fp:except # Enable floating-point exceptions
			>

			# Set Release configuration options
			$<$<CONFIG:Release>:
				/O2                    # Optimize for speed
				/Ob3                   # Inline any suitable function
				/Oi                    # Enable intrinsic functions
				/Ot                    # Favor speed
				/Oy                    # Omits frame pointers
				/GT                    # Enable fiber-safe optimizations
				/GL                    # Enable whole program optimization

				/Qfast_transcendentals # Use fast transcendentals
				/GS-                   # Disable security checks
				/Zi                    # Generate PDB file

				/GF                    # Enable string pooling
				/Qpar                  # Enable parallel code generation
			>
		>

		# Set GNU/Clang-specific compile options
		$<$<CXX_COMPILER_ID:GNU,Clang>:

			#-Wall      # Treat all warnings as errors
			#-Wpedantic # Issue all the warnings demanded by strict ISO C and C++
			#-Wextra    # Enable  extra warning flags that are not enabled by -Wall

			-Wno-changes-meaning
		>

		# Set Clang-specific compile options
		$<$<CXX_COMPILER_ID:Clang>:
			
			# Suppress warnings about compatibility with older C++ standards
			-Wno-c++98-compat
			-Wno-c++98-compat-pedantic
			-Wno-c++11-compat
			-Wno-c++11-compat-pedantic
			-Wno-c++14-compat
			-Wno-c++14-compat-pedantic
			-Wno-c++17-compat
			-Wno-c++17-compat-pedantic
			-Wno-c++20-compat
			-Wno-c++20-compat-pedantic
		>
)

# Set link options
target_link_options(${PROJECT_NAME} PRIVATE
	
	# Set Windows-specific link options
	$<$<PLATFORM_ID:Windows>:
			
		# Set MSVC-specific link options
		$<$<CXX_COMPILER_ID:MSVC>:

			# Set link options for Debug configuration
			$<$<CONFIG:Debug>:
				/SUBSYSTEM:CONSOLE # Specify console subsystem
				/INCREMENTAL       # Enable incremental linking
			>

			# Set link options for Release configuration
			$<$<CONFIG:Release>:
				/SUBSYSTEM:WINDOWS    # Specify Windows subsystem
				/ENTRY:mainCRTStartup # Specify entry point
				/OPT:REF              # Eliminate unreferenced functions/data
				/OPT:ICF              # Perform identical COMDAT folding
				/INCREMENTAL:NO       # Disable incremental linking
				/LTCG                 # Enable link-time code generation
				/DEBUG                # Generate debug information
			>
		>

		# Set GNU/Clang-specific link options
		$<$<CXX_COMPILER_ID:GNU,Clang>:
			$<$<CONFIG:Debug>:-mconsole>
			$<$<CONFIG:Release>:-mwindows --entry=mainCRTStartup>
		>
	>

	# Set GNU/Clang-specific link options
	$<$<CXX_COMPILER_ID:GNU,Clang>:
		-static
		$<$<CONFIG:Release>:-flto>
	>
)

# Set include directories
target_include_directories(${PROJECT_NAME}
	PRIVATE
		${PROJECT_SOURCE_DIR}/src
		${PROJECT_BINARY_DIR}/$<CONFIG>/src
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
		SDL3::SDL3-static
		OpenAL
		physfs-static
		freetype
		${OPENGL_gl_LIBRARY}
		vorbisfile
		lua

		$<$<PLATFORM_ID:Windows>:Dbghelp>

		# Set GNU-specific dependencies
		$<$<CXX_COMPILER_ID:GNU>:
			stdc++
			stdc++exp # Link to libstdc++exp.a for <stacktrace>
		>
)

# Determine data output directory
get_target_property(RUNTIME_OUTPUT_DIRECTORY ${PROJECT_NAME} RUNTIME_OUTPUT_DIRECTORY)
set(DATA_OUTPUT_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}/data")

# Add localization CMakeLists
add_subdirectory(${PROJECT_SOURCE_DIR}/res/localization)

# Add documentation CMakeLists
add_subdirectory(${PROJECT_SOURCE_DIR}/docs)

# Build antkeeper-data module (if exists)
if(EXISTS ${PROJECT_SOURCE_DIR}/res/data/CMakeLists.txt)
	
	add_subdirectory(${PROJECT_SOURCE_DIR}/res/data)

#	ExternalProject_Add(antkeeper-data
#		SOURCE_DIR ${PROJECT_SOURCE_DIR}/res/data
#		CMAKE_ARGS
#			"-DCMAKE_BUILD_TYPE:STRING=$<CONFIG>"
#			"-DRUNTIME_OUTPUT_DIRECTORY:STRING=${PROJECT_SOURCE_DIR}/bin/$<LOWER_CASE:${PROJECT_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${ANTKEEPER_BUILD_ARCH}>"
#			"-DAPPLICATION_NAME=${PROJECT_NAME}"
#			"-DAPPLICATION_VERSION=${PROJECT_VERSION}"
#			"-DAPPLICATION_AUTHOR=${PROJECT_AUTHOR}"
#			"-DBLENDER=${BLENDER}"
#		INSTALL_COMMAND ""
#		BUILD_ALWAYS 1)
endif()

if(ANTKEEPER_TEST)

	# Enable testing
	include(CTest)
	enable_testing()

	# Collect test files
	file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/test/test-*.cpp)

	# Add test targets
	foreach(TEST_FILE ${TEST_FILES})

		get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
		add_executable(${TEST_NAME} ${TEST_FILE} ${PROJECT_SOURCE_DIR}/test/test.cpp)
		set_target_properties(${TEST_NAME}
			PROPERTIES
				COMPILE_WARNING_AS_ERROR ON
				CXX_STANDARD 23
				CXX_STANDARD_REQUIRED ON
				CXX_EXTENSIONS OFF
				MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
		)
		target_include_directories(${TEST_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
		add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

	endforeach()

endif()