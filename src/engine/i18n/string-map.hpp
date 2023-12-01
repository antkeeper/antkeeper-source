// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_I18N_STRING_MAP_HPP
#define ANTKEEPER_I18N_STRING_MAP_HPP

#include <string>
#include <unordered_map>
#include <engine/utility/hash/fnv1a.hpp>

namespace i18n {

/**
 * Maps 32-bit keys to strings.
 */
typedef std::unordered_map<hash::fnv1a32_t, std::string> string_map;

} // namespace i18n

#endif // ANTKEEPER_I18N_STRING_MAP_HPP
