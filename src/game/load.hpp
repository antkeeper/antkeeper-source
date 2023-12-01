// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_LOAD_HPP
#define ANTKEEPER_GAME_LOAD_HPP

#include "game/game.hpp"

namespace load {

/**
 * Loads a colony
 */
void colony(::game& ctx, const std::filesystem::path& path);

} // namespace load

#endif // ANTKEEPER_GAME_LOAD_HPP
