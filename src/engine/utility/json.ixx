// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <nlohmann/json.hpp>

export module engine.utility.json;

export namespace engine
{
	/// JSON element.
	using json = nlohmann::json;
}
