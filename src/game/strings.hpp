// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_STRINGS_HPP
#define ANTKEEPER_GAME_STRINGS_HPP

#include "game/game.hpp"
#include <engine/utility/hash/fnv1a.hpp>
#include <cstdint>
#include <string>

/**
 * Returns a localized string.
 *
 * @param ctx Game context.
 * @param key String key.
 *
 * @return String value.
 */
[[nodiscard]] std::string get_string(const ::game& ctx, hash::fnv1a32_t key);

#endif // ANTKEEPER_GAME_STRINGS_HPP
