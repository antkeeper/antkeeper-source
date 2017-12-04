cmake_minimum_required(VERSION 3.7)
project(antkeeper
	VERSION "0.0.0"
)

option(STANDALONE "Distribute in a standalone archive." OFF)
set(LANGUAGE "en-us" CACHE STRING "")

# Determine target build platform according to binary dir
get_filename_component(PLATFORM ${PROJECT_BINARY_DIR} NAME CACHE)

# Check for architecture mismatches
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
	if (${PLATFORM} STREQUAL "win64" OR ${PLATFORM} STREQUAL "linux64")
		message(FATAL_ERROR "Compiler architecture is 32-bit but target architecture is 64-bit.")
	endif()
else()
	if (${PLATFORM} STREQUAL "win32" OR ${PLATFORM} STREQUAL "linux32")
		message(FATAL_ERROR "Compiler architecture is 64-bit but target architecture is 32-bit.")
	endif()
endif()

# Setup configuration strings
set(ANTKEEPER_VERSION ${PROJECT_VERSION})
set(ANTKEEPER_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(ANTKEEPER_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(ANTKEEPER_VERSION_PATCH ${PROJECT_VERSION_PATCH})
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(ANTKEEPER_BUILD_TYPE debug)
	set(ANTKEEPER_DEBUG ON)
else()
	set(ANTKEEPER_BUILD_TYPE release)
	set(ANTKEEPER_DEBUG OFF)
endif()

# Setup build type paths
set(BUILD_DIR ${PROJECT_SOURCE_DIR}/bin)
set(BUILD_DEBUG_DIR ${BUILD_DIR}/debug)
set(BUILD_RELEASE_DIR ${BUILD_DIR}/release)

# Set package name
set(PACKAGE_NAME ${PROJECT_NAME}-${PROJECT_VERSION}-${PLATFORM})
set(PACKAGE_BUILD_NAME ${PACKAGE_NAME}-${ANTKEEPER_BUILD_TYPE})

# Set package directory
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(PLATFORM_PACKAGE_DIR ${BUILD_DEBUG_DIR}/${PACKAGE_NAME})
else()
	set(PLATFORM_PACKAGE_DIR ${BUILD_RELEASE_DIR}/${PACKAGE_NAME})
endif()

# Set C++ compiler flags for debug and release build types
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W2")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	# Update if necessary
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -g")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -s -DNDEBUG")
endif()

# Set C and C++ compiler flags for the target architecture
if(${PLATFORM} STREQUAL "win32" OR ${PLATFORM} STREQUAL "linux32")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
elseif(${PLATFORM} STREQUAL "win64" OR ${PLATFORM} STREQUAL "linux64")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
elseif(${PLATFORM} STREQUAL "osx" AND CMAKE_OSX_ARCHITECTURES STREQUAL "")
	# 32-bit
	#set(CMAKE_OSX_ARCHITECTURES "i386" CACHE STRING "" FORCE)
	
	# 64-bit
	#set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "" FORCE)
	
	# 96-bit universal
	set(CMAKE_OSX_ARCHITECTURES "x86_64;i386" CACHE STRING "" FORCE)
endif()

# Disable .manifest  generation
if(${PLATFORM} STREQUAL "win32" OR ${PLATFORM} STREQUAL "win64")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO /OPT:REF /OPT:ICF")
endif()

# Set C++ version
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include ExternalProject_Add command
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

# Configure, build and install emergent
set(EMERGENT_BUILD_DIR ${PROJECT_BINARY_DIR}/deps/build/emergent)
set(EMERGENT_INSTALL_DIR ${PROJECT_BINARY_DIR}/deps/install/emergent)
set(EMERGENT_INCLUDE_DIR ${EMERGENT_INSTALL_DIR}/include)
set(EMERGENT_GL3W_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/lib/emergent/lib/gl3w/include)
set(EMERGENT_GLM_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/lib/emergent/lib/glm)
set(EMERGENT_STB_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/lib/emergent/lib/stb)
set(EMERGENT_INCLUDE_DIRS
	${EMERGENT_INCLUDE_DIR}
	${EMERGENT_GL3W_INCLUDE_DIR}
	${EMERGENT_GLM_INCLUDE_DIR}
	${EMERGENT_STB_INCLUDE_DIR}
)
set(EMERGENT_LIBRARY emergent)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(FREETYPE_LIBRARY freetyped)
else()
	set(FREETYPE_LIBRARY freetype)
endif()
set(EMERGENT_LIBRARIES
	${EMERGENT_LIBRARY}
	${FREETYPE_LIBRARY}
)
ExternalProject_Add(emergent-project
	SOURCE_DIR ${PROJECT_SOURCE_DIR}/lib/emergent
	CMAKE_ARGS
	"-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
	"-DCMAKE_INSTALL_PREFIX=${EMERGENT_INSTALL_DIR}"
	BINARY_DIR ${EMERGENT_BUILD_DIR}
	INSTALL_DIR ${EMERGENT_INSTALL_DIR})

# Configure, build and install SDL2
set(SDL2_BUILD_DIR ${PROJECT_BINARY_DIR}/deps/build/SDL2)
set(SDL2_INSTALL_DIR ${PROJECT_BINARY_DIR}/deps/install/SDL2)
set(SDL2_INCLUDE_DIR ${SDL2_INSTALL_DIR}/include)
set(SDL2_LIBRARY SDL2)
set(SDL2main_LIBRARY SDL2main)
set(SDL2_LIBRARIES
	${SDL2main_LIBRARY}
	${SDL2_LIBRARY}
	dinput8
	dxguid
	user32
	gdi32
	winmm
	imm32
	ole32
	oleaut32
	shell32
	version
	uuid
)
ExternalProject_Add(SDL2-project
	SOURCE_DIR ${PROJECT_SOURCE_DIR}/lib/SDL2
	CMAKE_ARGS
	"-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
	"-DCMAKE_INSTALL_PREFIX=${SDL2_INSTALL_DIR}"
	"-DSDL_STATIC=true"
	"-DSDL_SHARED=false"
	BINARY_DIR ${SDL2_BUILD_DIR}
	INSTALL_DIR ${SDL2_INSTALL_DIR})

# Configure, build and install dirent
set(DIRENT_BUILD_DIR ${PROJECT_BINARY_DIR}/deps/build/dirent)
set(DIRENT_INSTALL_DIR ${PROJECT_BINARY_DIR}/deps/install/dirent)
set(DIRENT_INCLUDE_DIR ${DIRENT_INSTALL_DIR}/include)
ExternalProject_Add(dirent-project
	SOURCE_DIR ${PROJECT_SOURCE_DIR}/lib/dirent
	CMAKE_ARGS
	"-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
	"-DCMAKE_INSTALL_PREFIX=${DIRENT_INSTALL_DIR}"
	BINARY_DIR ${DIRENT_BUILD_DIR}
	INSTALL_DIR ${DIRENT_INSTALL_DIR})

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find executable source directory
set(EXECUTABLE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

# Generate C++ configuration file
configure_file(${EXECUTABLE_SOURCE_DIR}/configuration.hpp.in ${EXECUTABLE_SOURCE_DIR}/configuration.hpp)

# Collect executable source files
set(EXECUTABLE_SOURCES
	${EXECUTABLE_SOURCE_DIR}/configuration.hpp
	${EXECUTABLE_SOURCE_DIR}/controls.cpp
	${EXECUTABLE_SOURCE_DIR}/render-passes.hpp
	${EXECUTABLE_SOURCE_DIR}/settings.cpp
	${EXECUTABLE_SOURCE_DIR}/settings.hpp
	${EXECUTABLE_SOURCE_DIR}/game/terrain.cpp
	${EXECUTABLE_SOURCE_DIR}/game/terrain.hpp
	${EXECUTABLE_SOURCE_DIR}/controls.hpp
	${EXECUTABLE_SOURCE_DIR}/input.cpp
	${EXECUTABLE_SOURCE_DIR}/input.hpp
	${EXECUTABLE_SOURCE_DIR}/main.cpp
	${EXECUTABLE_SOURCE_DIR}/mesh.cpp
	${EXECUTABLE_SOURCE_DIR}/mesh.hpp
	${EXECUTABLE_SOURCE_DIR}/application-state.hpp
	${EXECUTABLE_SOURCE_DIR}/application-state.cpp
	${EXECUTABLE_SOURCE_DIR}/application.hpp
	${EXECUTABLE_SOURCE_DIR}/application.cpp
	${EXECUTABLE_SOURCE_DIR}/states/loading-state.hpp
	${EXECUTABLE_SOURCE_DIR}/states/loading-state.cpp
	${EXECUTABLE_SOURCE_DIR}/states/splash-state.hpp
	${EXECUTABLE_SOURCE_DIR}/states/splash-state.cpp
	${EXECUTABLE_SOURCE_DIR}/states/title-state.hpp
	${EXECUTABLE_SOURCE_DIR}/states/title-state.cpp
	${EXECUTABLE_SOURCE_DIR}/states/game-state.hpp
	${EXECUTABLE_SOURCE_DIR}/states/game-state.cpp
	${EXECUTABLE_SOURCE_DIR}/ui/ui.hpp
	${EXECUTABLE_SOURCE_DIR}/ui/ui.cpp
	${EXECUTABLE_SOURCE_DIR}/ui/menu.hpp
	${EXECUTABLE_SOURCE_DIR}/ui/menu.cpp
	${EXECUTABLE_SOURCE_DIR}/ui/tween.hpp
	${EXECUTABLE_SOURCE_DIR}/ui/tween.cpp
	${EXECUTABLE_SOURCE_DIR}/ui/toolbar.hpp
	${EXECUTABLE_SOURCE_DIR}/ui/toolbar.cpp
	${EXECUTABLE_SOURCE_DIR}/ui/pie-menu.hpp
	${EXECUTABLE_SOURCE_DIR}/ui/pie-menu.cpp
	${EXECUTABLE_SOURCE_DIR}/render-passes.cpp
	${EXECUTABLE_SOURCE_DIR}/game/ant.hpp
	${EXECUTABLE_SOURCE_DIR}/game/ant.cpp
	${EXECUTABLE_SOURCE_DIR}/game/agent.hpp
	${EXECUTABLE_SOURCE_DIR}/game/agent.cpp
	${EXECUTABLE_SOURCE_DIR}/game/colony.hpp
	${EXECUTABLE_SOURCE_DIR}/game/colony.cpp
	${EXECUTABLE_SOURCE_DIR}/game/habitat.hpp
	${EXECUTABLE_SOURCE_DIR}/game/habitat.cpp
	${EXECUTABLE_SOURCE_DIR}/game/nest.hpp
	${EXECUTABLE_SOURCE_DIR}/game/nest.cpp
	${EXECUTABLE_SOURCE_DIR}/game/navmesh.hpp
	${EXECUTABLE_SOURCE_DIR}/game/navmesh.cpp
	${EXECUTABLE_SOURCE_DIR}/game/pheromone-matrix.hpp
	${EXECUTABLE_SOURCE_DIR}/game/pheromone-matrix.cpp
	${EXECUTABLE_SOURCE_DIR}/game/level.hpp
	${EXECUTABLE_SOURCE_DIR}/game/level.cpp
	${EXECUTABLE_SOURCE_DIR}/game/biome.hpp
	${EXECUTABLE_SOURCE_DIR}/game/biome.cpp
	${EXECUTABLE_SOURCE_DIR}/game/tool.cpp
	${EXECUTABLE_SOURCE_DIR}/debug.hpp
	${EXECUTABLE_SOURCE_DIR}/debug.cpp
	${EXECUTABLE_SOURCE_DIR}/camera-controller.hpp
	${EXECUTABLE_SOURCE_DIR}/camera-controller.cpp
	${EXECUTABLE_SOURCE_DIR}/model-loader.hpp
	${EXECUTABLE_SOURCE_DIR}/model-loader.cpp
	${EXECUTABLE_SOURCE_DIR}/material-loader.hpp
	${EXECUTABLE_SOURCE_DIR}/material-loader.cpp
)

# Setup manifest and exe icon for windows
if(${PLATFORM} STREQUAL "win32" OR ${PLATFORM} STREQUAL "win64")
	list(APPEND EXECUTABLE_SOURCES "${EXECUTABLE_SOURCE_DIR}/dpi-aware.manifest")
	
	if(EXISTS ${PROJECT_SOURCE_DIR}/data)
		set(RC_FILES "${PROJECT_SOURCE_DIR}/data/icons/icon.rc")
		#set(CMAKE_RC_COMPILER_INIT windres)
		#enable_language(RC)
		#set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <SOURCE> <OBJECT>")
		set(EXECUTABLE_SOURCES "${EXECUTABLE_SOURCES};${RC_FILES}")
	endif()
endif()

# Set link directories
link_directories(${EMERGENT_INSTALL_DIR}/lib
	${SDL2_INSTALL_DIR}/lib
)

# Set executable and library output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PLATFORM_PACKAGE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PLATFORM_PACKAGE_DIR})

# Add executable
set(EXECUTABLE_NAME antkeeper)
set(EXECUTABLE_TARGET ${PACKAGE_BUILD_NAME}-executable)
add_executable(${EXECUTABLE_TARGET} ${EXECUTABLE_SOURCES})
add_dependencies(${EXECUTABLE_TARGET} emergent-project SDL2-project dirent-project)
set_target_properties(${EXECUTABLE_TARGET} PROPERTIES OUTPUT_NAME ${EXECUTABLE_NAME})

# Set include directories
target_include_directories(${EXECUTABLE_TARGET} PUBLIC
	${EMERGENT_INCLUDE_DIRS}
	${SDL2_INCLUDE_DIR}
	${DIRENT_INCLUDE_DIR}
	${OPENGL_INCLUDE_DIRS}
)

# Build library list
list(APPEND EXECUTABLE_LIBRARIES
	${SDL2_LIBRARIES}
	${EMERGENT_LIBRARIES}
	${OPENGL_gl_LIBRARY}
	${FREETYPE_LIBRARY}
	#m
	#stdc++
)

# Link libraries
target_link_libraries(${EXECUTABLE_TARGET} ${EXECUTABLE_LIBRARIES})

# Add run target
add_custom_target(run
	COMMAND ${EXECUTABLE_TARGET}
	DEPENDS ${EXECUTABLE_TARGET}
	WORKING_DIRECTORY ${PLATFORM_PACKAGE_DIR}
)

# Add dist target
add_custom_target(dist
	COMMAND
	${CMAKE_COMMAND} -E tar "cfvz" "${PROJECT_SOURCE_DIR}/dist/${PACKAGE_NAME}.zip" --format=zip
	"${PLATFORM_PACKAGE_DIR}")

# Add clean targets
add_custom_target(clean-build
	COMMAND git clean -fdX
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/build"
)

# Add data subdirectory (if it exists)
if(EXISTS ${PROJECT_SOURCE_DIR}/data)
	add_subdirectory(${PROJECT_SOURCE_DIR}/data)
endif()


# Distribution
install(TARGETS ${EXECUTABLE_TARGET} DESTINATION ".")
install(DIRECTORY ${PLATFORM_PACKAGE_DIR}/data
	DESTINATION .)

set(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "C. J. Howard")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${PLATFORM})
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CPACK_OUTPUT_FILE_PREFIX "${PROJECT_SOURCE_DIR}/dist/debug")
else()
	set(CPACK_OUTPUT_FILE_PREFIX "${PROJECT_SOURCE_DIR}/dist/release")
endif()

if(${PLATFORM} STREQUAL "win32" OR ${PLATFORM} STREQUAL "win64")
	set(CPACK_GENERATOR "ZIP")
	if (NOT STANDALONE)
		set(CPACK_GENERATOR "NSIS")
		set(CPACK_MONOLITHIC_INSTALL 1)
		set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_FILE_NAME}-installer)
		set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/data/icons/antkeeper-icon.ico")
		set(CPACK_PACKAGE_INSTALL_DIRECTORY "Antkeeper")
		get_target_property(EXECUTABLE_OUTPUT_NAME ${EXECUTABLE_TARGET} OUTPUT_NAME)
		set(CPACK_NSIS_MENU_LINKS "${EXECUTABLE_OUTPUT_NAME}" "Antkeeper")
	endif()
elseif(${PLATFORM} STREQUAL "linux32" OR ${PLATFORM} STREQUAL "linux64")
	set(CPACK_GENERATOR "TGZ")
	if(NOT STANDALONE)
		set(CPACK_GENERATOR "DEB")
		set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR}")
	endif()
endif()

include(CPack)