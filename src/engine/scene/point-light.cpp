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

#include <engine/scene/point-light.hpp>
#include <engine/math/interpolation.hpp>

namespace scene {

point_light::point_light():
	attenuation(float3{1, 0, 0}, math::lerp<float3, float>)
{}

void point_light::set_attenuation(const float3& attenuation)
{
	this->attenuation[1] = attenuation;
}

void point_light::update_tweens()
{
	light::update_tweens();
	attenuation.update();
}

} // namespace scene
