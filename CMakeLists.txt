cmake_minimum_required(VERSION 3.7)

# Set compiler flags
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O3 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Include project macro
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/project.cmake)

# Find dependency packages
find_package(emergent REQUIRED CONFIG)
find_package(OpenAL REQUIRED CONFIG)

# Determine dependencies
set(STATIC_LIBS
	emergent
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

# Set include directories
target_include_directories(${EXECUTABLE_TARGET}
	PUBLIC
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_BINARY_DIR}/src)

# Link to dependencies
target_link_libraries(${EXECUTABLE_TARGET} ${STATIC_LIBS})

# Install executable
install(TARGETS ${EXECUTABLE_TARGET} DESTINATION bin)
