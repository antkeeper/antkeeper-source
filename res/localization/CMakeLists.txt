# SPDX-FileCopyrightText: 2024 C. J. Howard
# SPDX-License-Identifier: GPL-3.0-or-later

# Collect language files
file(GLOB_RECURSE LANGUAGE_FILES CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/languages/*.json
)

# Generate language manifest
set(LANGUAGE_MANIFEST "${DATA_OUTPUT_DIRECTORY}/localization/languages.json")
add_custom_command(
	OUTPUT ${LANGUAGE_MANIFEST}
	COMMAND ${Python3_EXECUTABLE} "${PROJECT_SOURCE_DIR}/tools/generate-language-manifest.py" "${CMAKE_CURRENT_SOURCE_DIR}/languages" "${LANGUAGE_MANIFEST}"
	DEPENDS
		${PROJECT_SOURCE_DIR}/tools/generate-language-manifest.py
		${LANGUAGE_FILES}
)

# Export strings for each supported language
foreach(LANGUAGE_FILE IN LISTS LANGUAGE_FILES)
	get_filename_component(LANGUAGE_TAG ${LANGUAGE_FILE} NAME_WE)
	set(OUTPUT_FILE "${DATA_OUTPUT_DIRECTORY}/localization/strings.${LANGUAGE_TAG}.json")
	add_custom_command(
		OUTPUT ${OUTPUT_FILE}
		COMMAND ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/tools/strings-to-json.py ${CMAKE_CURRENT_SOURCE_DIR}/strings.csv ${LANGUAGE_TAG} ${OUTPUT_FILE}
		DEPENDS
			${PROJECT_SOURCE_DIR}/tools/strings-to-json.py
			${CMAKE_CURRENT_SOURCE_DIR}/strings.csv
	)
	list(APPEND STRING_FILES "${OUTPUT_FILE}")
endforeach()

# Add localization target
add_custom_target(localization ALL
	DEPENDS
		${LANGUAGE_MANIFEST}
		${STRING_FILES}
)
