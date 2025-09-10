// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_STRINGS_HPP
#define ANTKEEPER_GAME_STRINGS_HPP

#include "game/game.hpp"
#include <engine/utility/sized-types.hpp>
#include <string_view>

/// Returns a localized string.
/// @param ctx Game context.
/// @param key String key.
/// @return String value.
[[nodiscard]] std::string get_string(const ::game& ctx, std::string_view key);

#endif // ANTKEEPER_GAME_STRINGS_HPP
