// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_MENU_HPP
#define ANTKEEPER_GAME_MENU_HPP

#include "game/game.hpp"

namespace menu {

void init_menu_item_index(::game& ctx, hash::fnv32_t menu_name);
void setup_animations(::game& ctx);

void clear_callbacks(::game& ctx);
void remove_text_from_ui(::game& ctx);
void delete_text(::game& ctx);

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
