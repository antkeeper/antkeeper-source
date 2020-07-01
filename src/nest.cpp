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

#include "nest.hpp"
#include "animation/easings.hpp"
#include "marching-cubes.hpp"
#include "geometry/mesh-functions.hpp"
#include "sdf.hpp"
#include <functional>
#include <iostream>
#include <map>
#include <vector>

#include "math.hpp"
#include <functional>
#include <limits>

nest::nest()
{
	dig_radius = 1.25f;
}

float3 nest::extend_shaft(shaft& shaft)
{
	float3 dig_position = get_shaft_position(shaft, shaft.current_depth);

	float dr = frand(dig_radius * 0.75f, dig_radius * 1.25f);

	shaft.current_depth += dr * 0.5f;

	return dig_position;
}

float3 nest::expand_chamber(chamber& chamber)
{
	float dig_angle = frand(0.0f, vmq::two_pi<float>);
	float2 dig_direction = vmq::normalize(float2{std::cos(dig_angle), std::sin(dig_angle)});

	float3 chamber_center = get_shaft_position(*chamber.shaft, chamber.depth);
	float3 dig_position = chamber_center;

	float dr = frand(dig_radius * 0.75f, dig_radius * 1.25f);

	float t = frand(0.0f, 1.0f);
	dig_position.x += dig_direction.x * (chamber.outer_radius - dr) * t;
	dig_position.z += dig_direction.y * (chamber.outer_radius - dr) * t;

	return dig_position;
}

void nest::set_tunnel_radius(float radius)
{
	tunnel_radius = radius;
}

float nest::get_shaft_angle(const shaft& shaft, float depth) const
{
	float shaft_length = shaft.depth[1] - shaft.depth[0];
	float depth_factor = (depth - shaft.depth[0]) / shaft_length;
	float pitch = ease_linear(shaft.pitch[0], shaft.pitch[1], depth_factor);
	return shaft.rotation + (depth / pitch) * shaft.chirality;
}

float3 nest::get_shaft_position(const shaft& shaft, float depth) const
{
	float shaft_length = shaft.depth[1] - shaft.depth[0];
	float depth_factor = (depth - shaft.depth[0]) / shaft_length;

	float pitch = ease_linear(shaft.pitch[0], shaft.pitch[1], depth_factor);
	float radius = ease_out_expo(shaft.radius[0], shaft.radius[1], depth_factor);
	float translation_x = ease_linear(shaft.translation[0][0], shaft.translation[1][0], depth_factor);
	float translation_z = ease_linear(shaft.translation[0][1], shaft.translation[1][1], depth_factor);
	float angle = shaft.rotation + (depth / pitch) * shaft.chirality;

	float3 position;
	position[0] = std::cos(angle) * radius + translation_x;
	position[1] = -std::max<float>(shaft.depth[0], std::min<float>(shaft.depth[1], depth));
	position[2] = std::sin(angle) * radius + translation_z;

	return position;

}

