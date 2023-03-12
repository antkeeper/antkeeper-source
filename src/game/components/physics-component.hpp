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

#ifndef ANTKEEPER_GAME_PHYSICS_COMPONENT_HPP
#define ANTKEEPER_GAME_PHYSICS_COMPONENT_HPP

#include <engine/math/vector.hpp>

struct physics_component
{
	/// Mass, in kg.
	float mass;
	
	/// Inverse mass, in kg^-1.
	//float inverse_mass{0.0f};
	
	/// Force vector, in newtons.
	math::vector<float, 3> force{0.0f, 0.0f, 0.0f};
	
	/// Acceleration vector, in m/s^2.
	math::vector<float, 3> acceleration{0.0f, 0.0f, 0.0f};
	
	/// Velocity vector, in m/s.
	math::vector<float, 3> velocity{0.0f, 0.0f, 0.0f};
};

#endif // ANTKEEPER_GAME_PHYSICS_COMPONENT_HPP
