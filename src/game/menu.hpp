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

namespace game {
namespace menu {

void init_menu_item_index(game::context& ctx, const std::string& menu_name);
void setup_controls(game::context& ctx);
void clear_controls(game::context& ctx);
void enable_controls(game::context& ctx);
void disable_controls(game::context& ctx);
void setup_animations(game::context& ctx);

void clear_callbacks(game::context& ctx);
void remove_text_from_ui(game::context& ctx);
void delete_text(game::context& ctx);
void delete_animations(game::context& ctx);



void fade_in(game::context& ctx, const std::function<void()>& end_callback);
void fade_out(game::context& ctx, const std::function<void()>& end_callback);

void fade_in_bg(game::context& ctx);
void fade_out_bg(game::context& ctx);

void update_text_color(game::context& ctx);
void update_text_font(game::context& ctx);
void update_text_tweens(game::context& ctx);
void align_text(game::context& ctx, bool center = false, bool has_back = true, float anchor_y = 0.0f);
void refresh_text(game::context& ctx);
void add_text_to_ui(game::context& ctx);

} // namespace menu
} // namespace game

#endif // ANTKEEPER_GAME_MENU_HPP
