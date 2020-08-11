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

#ifndef ANTKEEPER_NEST_HPP
#define ANTKEEPER_NEST_HPP

#include "geometry/mesh.hpp"
#include "utility/fundamental-types.hpp"
#include <array>
#include <vector>

class nest
{
public:
	struct chamber;

	struct shaft
	{
		std::array<float, 2> depth;  ///< start and end shaft depth
		float chirality; ///< 1 = right-handed, -1 = left-handed
		float rotation;  ///< starting helix angle, in radians
		std::array<float, 2> radius; ///< start and end helix radius
		std::array<float, 2> pitch;  ///< start and end helix pitch
		std::array<float2, 2> translation; ///< start and end helix translation
		std::vector<chamber> chambers;
		float current_depth;
	};

	struct chamber
	{
		shaft* shaft; ///< parent shaft
		float depth; ///< chamber depth, relative to parent shaft
		float rotation; ///< chamber rotation, relative to helix angle
		float sector_angle; ///<
		float inner_radius;
		float outer_radius;
	};

	/**
	 * Creates a nest.
	 */
	nest();

	float3 extend_shaft(shaft& shaft);
	float3 expand_chamber(chamber& chamber);

	void regenerate();

	void set_tunnel_radius(float radius);

	shaft* get_central_shaft();

	/**
	 * Calculates the position on a shaft at the specified depth.
	 */
	float3 get_shaft_position(const shaft& shaft, float depth) const;

	float get_shaft_angle(const shaft& shaft, float depth) const;
	
	float get_shaft_depth(const shaft& shaft, float turns) const;
	
private:
	float tunnel_radius;
	shaft central_shaft;
	float dig_radius;
};

inline nest::shaft* nest::get_central_shaft()
{
	return &central_shaft;
}

#endif // ANTKEEPER_NEST_HPP

