// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_MENU_HPP
#define ANTKEEPER_GAME_MENU_HPP

#include "game/game.hpp"

void build_menus(::game& ctx);

void open_main_menu(::game& ctx, bool fade_in);
void close_main_menu(::game& ctx);

void open_pause_menu(::game& ctx);
void close_pause_menu(::game& ctx);

#endif // ANTKEEPER_GAME_MENU_HPP
