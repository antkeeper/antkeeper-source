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

#include "directional-light.hpp"
#include "configuration.hpp"
#include "math/math.hpp"

static float3 interpolate_direction(const float3& x, const float3& y, float a)
{
	math::quaternion<float> q0 = math::rotation(global_forward, x);
	math::quaternion<float> q1 = math::rotation(global_forward, y);
	return math::normalize(math::slerp(q0, q1, a) * global_forward);
}

directional_light::directional_light():
	direction(global_forward, interpolate_direction)
{}

void directional_light::update_tweens()
{
	light::update_tweens();
	direction.update();
}

void directional_light::transformed()
{
	direction[1] = math::normalize(get_transform().rotation * global_forward);
}

