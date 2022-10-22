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

#include "directional-light.hpp"
#include "config.hpp"
#include "math/quaternion.hpp"
#include "math/interpolation.hpp"

namespace scene {

static float3 interpolate_direction(const float3& x, const float3& y, float a)
{
	math::quaternion<float> q0 = math::rotation(config::global_forward, x);
	math::quaternion<float> q1 = math::rotation(config::global_forward, y);
	return math::normalize(math::slerp(q0, q1, a) * config::global_forward);
}

directional_light::directional_light():
	direction(config::global_forward, interpolate_direction),
	light_texture(nullptr),
	light_texture_opacity(1.0f, math::lerp<float, float>),
	light_texture_scale({1.0f, 1.0f}, math::lerp<float2, float>)
{}

void directional_light::set_light_texture(const gl::texture_2d* texture)
{
	light_texture = texture;
}

void directional_light::set_light_texture_opacity(float opacity)
{
	light_texture_opacity[1] = opacity;
}

void directional_light::set_light_texture_scale(const float2& scale)
{
	light_texture_scale[1] = scale;
}

void directional_light::update_tweens()
{
	light::update_tweens();
	direction.update();
	
	if (light_texture)
	{
		light_texture_opacity.update();
		light_texture_scale.update();
	}
}

void directional_light::transformed()
{
	direction[1] = math::normalize(get_transform().rotation * config::global_forward);
}

} // namespace scene
