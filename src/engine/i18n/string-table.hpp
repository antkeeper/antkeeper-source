// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_I18N_STRING_TABLE_HPP
#define ANTKEEPER_I18N_STRING_TABLE_HPP

#include <string>
#include <vector>

namespace i18n {

/**
 * Table of strings.
 */
struct string_table
{
	/// Rows of column strings.
	std::vector<std::vector<std::string>> rows;
};

} // namespace i18n

#endif // ANTKEEPER_I18N_STRING_TABLE_HPP
