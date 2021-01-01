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

#ifndef ANTKEEPER_ECS_CELESTIAL_BODY_COMPONENT_HPP
#define ANTKEEPER_ECS_CELESTIAL_BODY_COMPONENT_HPP

#include "game/astronomy/celestial-mechanics.hpp"
#include "utility/fundamental-types.hpp"
#include "math/quaternion-type.hpp"

namespace ecs {

struct celestial_body_component
{
	ast::orbital_elements orbital_elements;
	ast::orbital_elements orbital_rate;
	ast::orbital_state orbital_state;
	
	double3 position;
	double3 velocity;
	double3 acceleration;
	double mass;
	double radius;
	math::quaternion<double> orientation;
};

struct blackbody_radiator
{
	double temperature;
};

struct diffuse_reflector
{
	double albedo;
};

} // namespace ecs

#endif // ANTKEEPER_ECS_CELESTIAL_BODY_COMPONENT_HPP
