/*
 * Copyright (C) 2021  Christopher J. Howard
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

/// Delay, in seconds, to when menu input should be activated.
static constexpr float input_delay = 0.01f;

/// RGBA color of active menu items.
static constexpr float4 active_color{1.0f, 1.0f, 1.0f, 1.0f};

/// RGBA color of inactive menu items.
static constexpr float4 inactive_color{1.0f, 1.0f, 1.0f, 0.5f};

/// Padding of the mouseover bounds, as a percentage of the font size.
static constexpr float mouseover_padding = 0.1f;

void init_menu_item_index(game::context* ctx, const std::string& menu_name);
void setup_controls(game::context* ctx);

void clear_controls(game::context* ctx);
void clear_callbacks(game::context* ctx);
void remove_text_from_ui(game::context* ctx);
void delete_text(game::context* ctx);

void update_text_color(game::context* ctx);
void update_text_font(game::context* ctx);
void update_text_tweens(game::context* ctx);
void align_text(game::context* ctx, bool center = false, bool has_back = true, float anchor_y = 0.0f);
void refresh_text(game::context* ctx);
void add_text_to_ui(game::context* ctx);

} // namespace menu
} // namespace game

#endif // ANTKEEPER_GAME_MENU_HPP
