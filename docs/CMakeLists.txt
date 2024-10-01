# SPDX-FileCopyrightText: 2024 C. J. Howard
# SPDX-License-Identifier: GPL-3.0-or-later

option(ANTKEEPER_BUILD_DOCS "Build documentation" OFF)

if(ANTKEEPER_BUILD_DOCS)
	# Find Doxygen
	find_package(Doxygen
		OPTIONAL_COMPONENTS
			dot
	)
endif()

if(ANTKEEPER_BUILD_DOCS AND DOXYGEN_FOUND)
	
	# Download Doxygen Awesome
	FetchContent_Declare(doxygen_awesome
		GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
		GIT_TAG df88fe4fdd97714fadfd3ef17de0b4401f804052 # v2.3.1
	)
	FetchContent_MakeAvailable(doxygen_awesome)
	
	# Generate Doxyfile
	configure_file(${PROJECT_SOURCE_DIR}/docs/doxygen/Doxyfile.in ${PROJECT_BINARY_DIR}/docs/doxygen/Doxyfile)
	
	# Add docs target
	add_custom_target(docs
		COMMAND Doxygen::doxygen ${PROJECT_BINARY_DIR}/docs/doxygen/Doxyfile
	)
endif()
