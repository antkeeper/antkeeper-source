// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_GRAPHICS_HPP
#define ANTKEEPER_GAME_GRAPHICS_HPP

#include "game/game.hpp"
#include <engine/render/anti-aliasing-method.hpp>

namespace graphics {

void create_framebuffers(::game& ctx);
void destroy_framebuffers(::game& ctx);
void change_render_resolution(::game& ctx, float scale);
void save_screenshot(::game& ctx);
void toggle_bloom(::game& ctx, bool enabled);
void select_anti_aliasing_method(::game& ctx, render::anti_aliasing_method method);

} // namespace graphics

#endif // ANTKEEPER_GAME_GRAPHICS_HPP
