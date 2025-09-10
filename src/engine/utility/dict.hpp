// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <any>
#include <unordered_map>

namespace engine
{
	/// Unordered dictionary type.
	/// @tparam Key Key type.
	template <class Key>
	using dict = std::unordered_map<Key, std::any>;
}
