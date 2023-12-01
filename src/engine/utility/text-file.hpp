// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_UTILITY_TEXT_FILE_HPP
#define ANTKEEPER_UTILITY_TEXT_FILE_HPP

#include <string>
#include <vector>

/**
 * Virtual text file.
 */
struct text_file
{
	/// Text file lines.
	std::vector<std::string> lines;
};

#endif // ANTKEEPER_UTILITY_TEXT_FILE_HPP
