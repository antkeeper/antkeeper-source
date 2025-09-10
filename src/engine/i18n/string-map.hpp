// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/hash/fnv.hpp>
#include <string>
#include <unordered_map>

namespace engine::i18n
{
	/// Maps 32-bit keys to strings.
	using string_map = std::unordered_map<hash::fnv32_t, std::string>;
}
