# Prevent in-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "In-source builds prohibited. Call cmake from the build directory.")
endif()

cmake_minimum_required(VERSION 3.7)

# Set compiler flags
if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_CXX_FLAGS "-Wall -Wextra")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")
elseif(MSVC)
	set(CMAKE_CXX_FLAGS "/W3 /MP /MD")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS}")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /Ox")
endif()

# Include project macro
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/project.cmake)

# Find dependency packages
find_package(emergent REQUIRED CONFIG)
find_package(OpenAL REQUIRED CONFIG)

# Determine dependencies
set(STATIC_LIBS
	emergent)
set(SHARED_LIBS
	OpenAL::OpenAL)

# Generate configuration header file
configure_file(${PROJECT_SOURCE_DIR}/src/configuration.hpp.in
	${PROJECT_BINARY_DIR}/src/configuration.hpp)

# Collect source files
file(GLOB_RECURSE SOURCE_FILES
	${PROJECT_SOURCE_DIR}/src/*.cpp)

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
	
	# Install OpenAL DLL
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		get_target_property(OPENAL_DLL OpenAL::OpenAL IMPORTED_LOCATION_DEBUG)
	else()
		get_target_property(OPENAL_DLL OpenAL::OpenAL IMPORTED_LOCATION_RELEASE)
	endif()
	
	install(FILES ${OPENAL_DLL} DESTINATION .)
endif()
