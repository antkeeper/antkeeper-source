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

#include "game/context.hpp"

namespace menu {

void init_menu_item_index(::context& ctx, const std::string& menu_name);
void setup_animations(::context& ctx);

void clear_callbacks(::context& ctx);
void remove_text_from_ui(::context& ctx);
void delete_text(::context& ctx);
void delete_animations(::context& ctx);



void fade_in(::context& ctx, const std::function<void()>& end_callback);
void fade_out(::context& ctx, const std::function<void()>& end_callback);

void fade_in_bg(::context& ctx);
void fade_out_bg(::context& ctx);

void update_text_color(::context& ctx);
void update_text_font(::context& ctx);
void update_text_tweens(::context& ctx);
void align_text(::context& ctx, bool center = false, bool has_back = true, float anchor_y = 0.0f);
void refresh_text(::context& ctx);
void add_text_to_ui(::context& ctx);

} // namespace menu

#endif // ANTKEEPER_GAME_MENU_HPP
