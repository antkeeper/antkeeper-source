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

#include "spotlight.hpp"
#include "configuration.hpp"
#include "math/math.hpp"
#include <cmath>

static float3 interpolate_direction(const float3& x, const float3& y, float a)
{
	math::quaternion<float> q0 = math::rotation(global_forward, x);
	math::quaternion<float> q1 = math::rotation(global_forward, y);
	return math::normalize(math::slerp(q0, q1, a) * global_forward);
}

spotlight::spotlight():
	direction(global_forward, interpolate_direction),
	attenuation(float3{1, 0, 0}, math::lerp<float3, float>),
	cutoff(float2{math::pi<float>, math::pi<float>}, math::lerp<float2, float>),
	cosine_cutoff(float2{std::cos(math::pi<float>), std::cos(math::pi<float>)}, math::lerp<float2, float>)
{}

void spotlight::set_attenuation(const float3& attenuation)
{
	this->attenuation[1] = attenuation;
}

void spotlight::set_cutoff(const float2& cutoff)
{
	this->cutoff[1] = cutoff;
	this->cosine_cutoff[1] = {std::cos(cutoff.x), std::cos(cutoff.y)};
}

void spotlight::update_tweens()
{
	light::update_tweens();
	direction.update();
	attenuation.update();
	cutoff.update();
	cosine_cutoff.update();
}

void spotlight::transformed()
{
	direction[1] = math::normalize(get_transform().rotation * global_forward);
}

