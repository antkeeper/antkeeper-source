// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_UTILITY_DICT_HPP
#define ANTKEEPER_UTILITY_DICT_HPP

#include <any>
#include <unordered_map>

/**
 * Unordered dictionary type.
 *
 * @tparam Key Key type.
 */
template <class Key>
using dict = std::unordered_map<Key, std::any>;

#endif // ANTKEEPER_UTILITY_DICT_HPP
