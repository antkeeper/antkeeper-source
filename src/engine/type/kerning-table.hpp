// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_TYPE_KERNING_TABLE_HPP
#define ANTKEEPER_TYPE_KERNING_TABLE_HPP

#include <engine/math/vector.hpp>
#include <unordered_map>

namespace type {

/// Table containing kerning offsets for pairs of glyphs.
typedef std::unordered_map<char32_t, std::unordered_map<char32_t, math::fvec2>> kerning_table;

} // namespace type

#endif // ANTKEEPER_TYPE_KERNING_TABLE_HPP
