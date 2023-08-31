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

#include <engine/scene/light.hpp>
#include <engine/config.hpp>
#include <engine/color/cct.hpp>

namespace scene {

void light::set_color(const math::fvec3& color)
{
	m_color = color;
	color_updated();
}

void light::set_color_temperature(float temperature)
{
	set_color(config::scene_linear_color_space<float>.from_xyz * color::cct_to_xyz(temperature));
}

void light::transformed()
{
	m_bounds = {get_translation(), get_translation()};
}

} // namespace scene
