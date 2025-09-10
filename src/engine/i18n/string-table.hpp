// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <string>
#include <vector>

namespace engine::i18n
{
	/// Table of strings.
	struct string_table
	{
		/// Rows of column strings.
		std::vector<std::vector<std::string>> rows;
	};
}
