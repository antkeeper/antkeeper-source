cmake_minimum_required(VERSION 3.27)

option(APPLICATION_NAME "Application name" "Antkeeper")
option(APPLICATION_VERSION "Application version string" "0.0.0")
option(APPLICATION_AUTHOR "Application author" "C. J. Howard")

# Slugify application name
string(TOLOWER ${APPLICATION_NAME} APPLICATION_SLUG)
string(REPLACE " " "-" APPLICATION_SLUG ${APPLICATION_SLUG})

project(${APPLICATION_SLUG} VERSION ${APPLICATION_VERSION} LANGUAGES CXX)

set(APPLICATION_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(APPLICATION_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(APPLICATION_VERSION_PATCH ${PROJECT_VERSION_PATCH})

# Find dependency packages
find_package(dr_wav REQUIRED CONFIG)
find_package(stb REQUIRED CONFIG)
find_package(tinyexr REQUIRED CONFIG)
find_package(glad REQUIRED CONFIG)
find_package(EnTT REQUIRED CONFIG)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED CONFIG)
find_package(OpenAL REQUIRED CONFIG)
find_library(physfs REQUIRED NAMES physfs-static PATHS "${CMAKE_PREFIX_PATH}/lib")
find_package(freetype REQUIRED CONFIG)

# Determine dependencies
set(STATIC_LIBS
	dr_wav
	stb
	tinyexr
	glad
	SDL2::SDL2-static
	SDL2::SDL2main
	OpenAL::OpenAL
	${physfs}
	freetype
)
set(SHARED_LIBS
	${OPENGL_gl_LIBRARY}
)

# Generate configuration header file
configure_file(${PROJECT_SOURCE_DIR}/src/engine/config.hpp.in ${PROJECT_BINARY_DIR}/src/engine/config.hpp)

# Collect platform-independent source files
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
	${PROJECT_SOURCE_DIR}/src/*.cpp
	EXCLUDE_DIR "${PROJECT_SOURCE_DIR}/src/game/platform"
)

if(MSVC)
	# Collect Windows-specific source files
	file(GLOB_RECURSE WINDOWS_SOURCE_FILES CONFIGURE_DEPENDS
		${PROJECT_SOURCE_DIR}/src/game/platform/windows/*.cpp
	)
	
	# Add Windows-specific source files to SOURCE_FILES
	list(APPEND SOURCE_FILES ${WINDOWS_SOURCE_FILES})
	
	# Generate Windows icon resource file
	set(ICON_FILE "${PROJECT_SOURCE_DIR}/../antkeeper-data/src/icons/antkeeper.ico")
	if(EXISTS "${ICON_FILE}")
		configure_file(${PROJECT_SOURCE_DIR}/src/game/platform/windows/icon.rc.in ${PROJECT_BINARY_DIR}/src/game/platform/windows/icon.rc)
		list(APPEND SOURCE_FILES "${PROJECT_BINARY_DIR}/src/game/platform/windows/icon.rc")
	endif()
	
	# Generate Windows version-information resource file
	configure_file(${PROJECT_SOURCE_DIR}/src/game/platform/windows/version.rc.in ${PROJECT_BINARY_DIR}/src/game/platform/windows/version.rc)
	list(APPEND SOURCE_FILES "${PROJECT_BINARY_DIR}/src/game/platform/windows/version.rc")
	
	# Make executable DPI-aware on Windows
	list(APPEND SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/game/platform/windows/dpi-aware.manifest")
endif()

# Add executable target
set(EXECUTABLE_TARGET ${APPLICATION_SLUG}-executable)
add_executable(${EXECUTABLE_TARGET} ${SOURCE_FILES})

# Set target properties
set_target_properties(${EXECUTABLE_TARGET} PROPERTIES
	OUTPUT_NAME ${APPLICATION_SLUG}
	COMPILE_WARNING_AS_ERROR ON
	CXX_STANDARD 23
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

# Set compile definitions
target_compile_definitions(${EXECUTABLE_TARGET} PRIVATE
	$<$<CONFIG:Debug>:DEBUG>
	$<$<NOT:$<CONFIG:Debug>>:NDEBUG>
	_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
)

# Set compile options
target_compile_options(${EXECUTABLE_TARGET} PRIVATE
	$<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -pedantic>
	$<$<CXX_COMPILER_ID:MSVC>:
		/W3
		$<$<NOT:$<CONFIG:Debug>>:/GL>
		$<$<CONFIG:Release>:/O2 /Ob3 /Qfast_transcendentals /GS->
	>
)

# Set link options
target_link_options(${EXECUTABLE_TARGET} PRIVATE
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
target_include_directories(${EXECUTABLE_TARGET} PUBLIC
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_BINARY_DIR}/src
)

# Link to dependencies
target_link_libraries(${EXECUTABLE_TARGET}
	${STATIC_LIBS}
	${SHARED_LIBS}
)

# Install executable
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	install(TARGETS ${EXECUTABLE_TARGET} DESTINATION bin)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
	install(TARGETS ${EXECUTABLE_TARGET} DESTINATION .)
endif()
