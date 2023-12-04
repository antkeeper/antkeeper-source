# SPDX-FileCopyrightText: 2023 C. J. Howard
# SPDX-License-Identifier: GPL-3.0-or-later

function(export_strings LANGUAGE_TAG)
	set(OUTPUT_FILE "${DATA_OUTPUT_DIRECTORY}/localization/${LANGUAGE_TAG}.json")
	add_custom_command(
		OUTPUT ${OUTPUT_FILE}
		COMMAND ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/tools/strings-to-json.py ${CMAKE_CURRENT_SOURCE_DIR}/strings.csv ${LANGUAGE_TAG} ${OUTPUT_FILE}
		DEPENDS
			${PROJECT_SOURCE_DIR}/tools/strings-to-json.py
			${CMAKE_CURRENT_SOURCE_DIR}/strings.csv
	)
	list(APPEND OUTPUT_FILES "${OUTPUT_FILE}")
	set(OUTPUT_FILES "${OUTPUT_FILES}" PARENT_SCOPE)
endfunction()

# Generate language manifest
add_custom_command(
	OUTPUT ${DATA_OUTPUT_DIRECTORY}/localization/languages.txt
	COMMAND ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/tools/index-languages.py ${CMAKE_CURRENT_SOURCE_DIR}/strings.csv ${DATA_OUTPUT_DIRECTORY}/localization/languages.txt
	DEPENDS
		${PROJECT_SOURCE_DIR}/tools/index-languages.py
		${CMAKE_CURRENT_SOURCE_DIR}/strings.csv
)
list(APPEND OUTPUT_FILES "${DATA_OUTPUT_DIRECTORY}/localization/languages.txt")

# Export strings
export_strings(en)
export_strings(zh-Hans)
export_strings(zh-Hant)

# Add localization target
add_custom_target(localization ALL
	DEPENDS
		${OUTPUT_FILES}
)
