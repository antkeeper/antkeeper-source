/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANTKEEPER_GAME_MENU_HPP
#define ANTKEEPER_GAME_MENU_HPP

#include "game/game.hpp"

namespace menu {

void init_menu_item_index(::game& ctx, hash::fnv1a32_t menu_name);
void setup_animations(::game& ctx);

void clear_callbacks(::game& ctx);
void remove_text_from_ui(::game& ctx);
void delete_text(::game& ctx);
void delete_animations(::game& ctx);



void fade_in(::game& ctx, const std::function<void()>& end_callback);
void fade_out(::game& ctx, const std::function<void()>& end_callback);

void fade_in_bg(::game& ctx);
void fade_out_bg(::game& ctx);

void update_text_color(::game& ctx);
void update_text_font(::game& ctx);
void align_text(::game& ctx, bool center = false, bool has_back = true, float anchor_y = 0.0f);
void refresh_text(::game& ctx);
void add_text_to_ui(::game& ctx);

} // namespace menu

#endif // ANTKEEPER_GAME_MENU_HPP
