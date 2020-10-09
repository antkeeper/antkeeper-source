cmake_minimum_required(VERSION 3.7)

option(VERSION_STRING "Project version string" "0.0.0")

project(antkeeper VERSION ${VERSION_STRING} LANGUAGES CXX)


# Find dependency packages
find_package(dr_wav REQUIRED CONFIG)
find_package(stb REQUIRED CONFIG)
find_package(glad REQUIRED CONFIG)
find_package(EnTT REQUIRED CONFIG)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED COMPONENTS SDL2::SDL2-static SDL2::SDL2main CONFIG)
find_package(OpenAL REQUIRED CONFIG)
find_library(physfs REQUIRED NAMES physfs-static PATHS "${CMAKE_PREFIX_PATH}/lib")

# Determine dependencies
set(STATIC_LIBS
	dr_wav
	stb
	glad
	EnTT
	SDL2::SDL2-static
	SDL2::SDL2main
	OpenAL::OpenAL
	${physfs})
set(SHARED_LIBS
	${OPENGL_gl_LIBRARY})

# Generate configuration header file
configure_file(${PROJECT_SOURCE_DIR}/src/configuration.hpp.in
	${PROJECT_BINARY_DIR}/src/configuration.hpp)

# Collect source files
file(GLOB_RECURSE SOURCE_FILES
	${PROJECT_SOURCE_DIR}/src/*.cpp)

# Remove platform-specific source files
set(EXCLUDE_DIR "${PROJECT_SOURCE_DIR}/src/platform/")
foreach(TMP_PATH ${SOURCE_FILES})
	string(FIND ${TMP_PATH} ${EXCLUDE_DIR} EXCLUDE_DIR_FOUND)
	if (NOT ${EXCLUDE_DIR_FOUND} EQUAL -1)
		list(REMOVE_ITEM SOURCE_FILES ${TMP_PATH})
	endif ()
endforeach(TMP_PATH)

if(MSVC)
	# Add platform-specific source files
	list(APPEND SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/platform/windows/nvidia.cpp")
	
	# Generate Windows icon resource file
	set(ICON_FILE "${PROJECT_SOURCE_DIR}/../antkeeper-data/src/icons/antkeeper.ico")
	configure_file(${PROJECT_SOURCE_DIR}/src/platform/windows/icon.rc.in
		${PROJECT_BINARY_DIR}/src/platform/windows/icon.rc)
	
	# Add executable icon
	list(APPEND SOURCE_FILES "${PROJECT_BINARY_DIR}/src/platform/windows/icon.rc")
	
	# Make DPI-aware on Windows
	list(APPEND SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/platform/windows/dpi-aware.manifest")
endif()

# Add executable target
set(EXECUTABLE_TARGET ${PROJECT_NAME}-executable)
add_executable(${EXECUTABLE_TARGET} ${SOURCE_FILES})
set_target_properties(${EXECUTABLE_TARGET} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

# Add compile definitions
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(${EXECUTABLE_TARGET} PRIVATE DEBUG)
else()
	target_compile_definitions(${EXECUTABLE_TARGET} PRIVATE NDEBUG)
endif()

# Set C++17 standard
set_target_properties(${EXECUTABLE_TARGET} PROPERTIES
	CXX_STANDARD 17
	CXX_EXTENSIONS OFF)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set_target_properties(${EXECUTABLE_TARGET} PROPERTIES COMPILE_FLAGS "-std=c++17")
elseif(MSVC)
	set_target_properties(${EXECUTABLE_TARGET} PROPERTIES COMPILE_FLAGS "/std:c++17")
endif()

# Set link flags to show console window on debug builds and hide it on release builds
if(MSVC)
	#set_target_properties(${EXECUTABLE_TARGET} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:libvcruntime.lib")
	set_target_properties(${EXECUTABLE_TARGET} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
	set_target_properties(${EXECUTABLE_TARGET} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:\"mainCRTStartup\"")
endif(MSVC)

# Set include directories
target_include_directories(${EXECUTABLE_TARGET}
	PUBLIC
		${PROJECT_SOURCE_DIR}/src
		${PROJECT_BINARY_DIR}/src)

# Link to dependencies
target_link_libraries(${EXECUTABLE_TARGET} ${STATIC_LIBS} ${SHARED_LIBS})

# Install executable
if(PACKAGE_PLATFORM MATCHES "linux")
	install(TARGETS ${EXECUTABLE_TARGET} DESTINATION bin)
elseif(PACKAGE_PLATFORM MATCHES "win")
	# Install executable
	install(TARGETS ${EXECUTABLE_TARGET} DESTINATION .)

	# Install SDL2 DLLs
	#if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	#	get_target_property(SDL2_DLL SDL2::SDL2 IMPORTED_LOCATION_DEBUG)
	#else()
	#	get_target_property(SDL2_DLL SDL2::SDL2 IMPORTED_LOCATION_RELEASE)
	#endif()
	#install(FILES ${SDL2_DLL} DESTINATION .)
	
	# Install OpenAL DLL
	#if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	#	get_target_property(OPENAL_DLL OpenAL::OpenAL IMPORTED_LOCATION_DEBUG)
	#else()
	#	get_target_property(OPENAL_DLL OpenAL::OpenAL IMPORTED_LOCATION_RELEASE)
	#endif()
	#install(FILES ${OPENAL_DLL} DESTINATION .)
endif()

