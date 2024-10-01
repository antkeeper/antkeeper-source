// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_I18N_STRING_MAP_HPP
#define ANTKEEPER_I18N_STRING_MAP_HPP

#include <string>
#include <unordered_map>
#include <engine/hash/fnv1a.hpp>

namespace i18n {

/// Maps 32-bit keys to strings.
using string_map = std::unordered_map<hash::fnv1a32_t, std::string>;

} // namespace i18n

#endif // ANTKEEPER_I18N_STRING_MAP_HPP
