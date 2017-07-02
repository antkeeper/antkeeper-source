cmake_minimum_required(VERSION 3.7)
project(antkeeper
	VERSION "0.0.0"
)

set(PLATFORM "" CACHE STRING "")
set(LANGUAGE "en-us" CACHE STRING "")

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
set(BUILD_DIR ${PROJECT_SOURCE_DIR}/build)
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

# Add Emergent library
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/emergent)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${PROJECT_SOURCE_DIR}/lib/emergent)

# Find dependencies
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

# Set C++ compiler flags for debug and release build types
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -s -DNDEBUG -mwindows")

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

# Set C++ version
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find executable source directory
set(EXECUTABLE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

# Generate C++ configuration file
configure_file(${EXECUTABLE_SOURCE_DIR}/configuration.hpp.in ${EXECUTABLE_SOURCE_DIR}/configuration.hpp)

# Collect executable source files
set(EXECUTABLE_SOURCES
	${EXECUTABLE_SOURCE_DIR}/configuration.hpp
	${EXECUTABLE_SOURCE_DIR}/windows-dirent.h
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
	${EXECUTABLE_SOURCE_DIR}/states/main-menu-state.hpp
	${EXECUTABLE_SOURCE_DIR}/states/main-menu-state.cpp
	${EXECUTABLE_SOURCE_DIR}/states/play-state.hpp
	${EXECUTABLE_SOURCE_DIR}/states/play-state.cpp
	${EXECUTABLE_SOURCE_DIR}/ui/ui.hpp
	${EXECUTABLE_SOURCE_DIR}/ui/ui.cpp
	${EXECUTABLE_SOURCE_DIR}/ui/tween.hpp
	${EXECUTABLE_SOURCE_DIR}/ui/tween.cpp
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
	${EXECUTABLE_SOURCE_DIR}/game/pheromone.hpp
	${EXECUTABLE_SOURCE_DIR}/game/pheromone.cpp
	${EXECUTABLE_SOURCE_DIR}/game/level.hpp
	${EXECUTABLE_SOURCE_DIR}/game/level.cpp
	${EXECUTABLE_SOURCE_DIR}/game/biome.hpp
	${EXECUTABLE_SOURCE_DIR}/game/biome.cpp
	${EXECUTABLE_SOURCE_DIR}/debug.hpp
	${EXECUTABLE_SOURCE_DIR}/debug.cpp
	${EXECUTABLE_SOURCE_DIR}/camera-controller.hpp
	${EXECUTABLE_SOURCE_DIR}/camera-controller.cpp
	${EXECUTABLE_SOURCE_DIR}/model-loader.hpp
	${EXECUTABLE_SOURCE_DIR}/model-loader.cpp
	${EXECUTABLE_SOURCE_DIR}/material-loader.hpp
	${EXECUTABLE_SOURCE_DIR}/material-loader.cpp
)

# Setup exe icon for windows
if(${PLATFORM} STREQUAL "win32" OR ${PLATFORM} STREQUAL "win64")
	if(EXISTS ${PROJECT_SOURCE_DIR}/data)
		set(RC_FILES "${PROJECT_SOURCE_DIR}/data/icons/icon.rc")
		set(CMAKE_RC_COMPILER_INIT windres)
		enable_language(RC)
		set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
		set(EXECUTABLE_SOURCES "${EXECUTABLE_SOURCES};${RC_FILES}")
	endif()
endif()

# Set executable and library output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PLATFORM_PACKAGE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PLATFORM_PACKAGE_DIR})

# Add executable
set(EXECUTABLE_NAME antkeeper)
set(EXECUTABLE_TARGET ${PACKAGE_BUILD_NAME}-executable)
add_executable(${EXECUTABLE_TARGET} ${EXECUTABLE_SOURCES})
set_target_properties(${EXECUTABLE_TARGET} PROPERTIES OUTPUT_NAME ${EXECUTABLE_NAME})

# Set include directories
target_include_directories(${EXECUTABLE_TARGET} PUBLIC
	${PROJECT_SOURCE_DIR}/lib/emergent/include
	${SDL2_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIRS}
)

message("SDL: ${SDL2_LIBRARIES}")

# Build library list
list(APPEND EXECUTABLE_LIBRARIES
	m
	#dl
	mingw32
	#PROBLEM IS LINKING TO STDC++!!!!!!!!!!!!!!!!
	#stdc++
	emergent
	${SDL2_LIBRARIES}
	${OPENGL_gl_LIBRARY}
)

# Link libraries
target_link_libraries(${EXECUTABLE_TARGET} ${EXECUTABLE_LIBRARIES})

# Add run target
add_custom_target(run
	COMMAND ${EXECUTABLE_TARGET}
	DEPENDS ${EXECUTABLE_TARGET}
	WORKING_DIRECTORY ${PLATFORM_PACKAGE_DIR}
)

# Add data subdirectory (if it exists)
if(EXISTS ${PROJECT_SOURCE_DIR}/data)
	add_subdirectory(${PROJECT_SOURCE_DIR}/data)
endif()