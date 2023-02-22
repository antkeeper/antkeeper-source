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
