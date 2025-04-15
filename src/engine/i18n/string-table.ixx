// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.i18n.string_table;
export import <string>;
export import <vector>;

export namespace engine::i18n
{
	/// Table of strings.
	struct string_table
	{
		/// Rows of column strings.
		std::vector<std::vector<std::string>> rows;
	};
}
