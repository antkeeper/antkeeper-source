/*
 * Copyright (C) 2020  Christopher J. Howard
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

#include "renderer/render-pass.hpp"

render_pass::render_pass(::rasterizer* rasterizer, const ::framebuffer* framebuffer):
	rasterizer(rasterizer),
	framebuffer(framebuffer),
	enabled(true)
{}

render_pass::~render_pass()
{}

void render_pass::set_enabled(bool enabled)
{
	this->enabled = enabled;
}

