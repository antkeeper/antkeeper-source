// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <string>
#include <vector>

namespace engine
{
	/// Virtual text file.
	struct text_file
	{
		/// Text file lines.
		std::vector<std::string> lines;
	};
}
