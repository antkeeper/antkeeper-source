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

#include <engine/scene/directional-light.hpp>
#include <engine/config.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/math/interpolation.hpp>

namespace scene {

static float3 interpolate_direction(const float3& x, const float3& y, float a)
{
	math::quaternion<float> q0 = math::rotation(config::global_forward, x);
	math::quaternion<float> q1 = math::rotation(config::global_forward, y);
	return math::normalize(math::slerp(q0, q1, a) * config::global_forward);
}

directional_light::directional_light():
	direction(config::global_forward, interpolate_direction),
	shadow_caster(false),
	shadow_framebuffer(nullptr),
	shadow_bias(0.005f),
	shadow_cascade_count(4),
	shadow_cascade_coverage(1.0f),
	shadow_cascade_distribution(0.8f)
{
	shadow_cascade_distances.resize(shadow_cascade_count);
	shadow_cascade_matrices.resize(shadow_cascade_count);
}

void directional_light::set_shadow_caster(bool caster) noexcept
{
	shadow_caster = caster;
}

void directional_light::set_shadow_framebuffer(const gl::framebuffer* framebuffer) noexcept
{
	shadow_framebuffer = framebuffer;
}

void directional_light::set_shadow_bias(float bias) noexcept
{
	shadow_bias = bias;
}

void directional_light::set_shadow_cascade_count(unsigned int count) noexcept
{
	shadow_cascade_count = count;
	shadow_cascade_distances.resize(shadow_cascade_count);
	shadow_cascade_matrices.resize(shadow_cascade_count);
}

void directional_light::set_shadow_cascade_coverage(float factor) noexcept
{
	shadow_cascade_coverage = factor;
}

void directional_light::set_shadow_cascade_distribution(float weight) noexcept
{
	shadow_cascade_distribution = weight;
}

void directional_light::update_tweens()
{
	light::update_tweens();
	direction.update();
}

void directional_light::transformed()
{
	direction[1] = math::normalize(get_transform().rotation * config::global_forward);
}

} // namespace scene
