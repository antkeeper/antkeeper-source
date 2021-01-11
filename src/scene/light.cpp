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

#include "scene/light.hpp"
#include "animation/ease.hpp"
#include "math/interpolation.hpp"

namespace scene {

light::light():
	bounds(get_translation(), 0.0f),
	color(float3{1.0f, 1.0f, 1.0f}, math::lerp<float3, float>),
	intensity(1.0f, math::lerp<float, float>),
	scaled_color(float3{1.0f, 1.0f, 1.0f}, math::lerp<float3, float>)
{}

void light::set_color(const float3& color)
{
	this->color[1] = color;
	scaled_color[1] = color * intensity[1];
}

void light::set_intensity(float intensity)
{
	this->intensity[1] = intensity;
	scaled_color[1] = color[1] * intensity;
}

void light::update_tweens()
{
	object_base::update_tweens();
	color.update();
	intensity.update();
	scaled_color.update();
}

void light::transformed()
{
	bounds = sphere<float>(get_translation(), 0.0f);
}

} // namespace scene
