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

#ifndef ANTKEEPER_ENTITY_COMPONENT_STEERING_HPP
#define ANTKEEPER_ENTITY_COMPONENT_STEERING_HPP

#include "ai/steering/agent.hpp"

namespace entity {
namespace component {

struct steering
{
	/// Steering agent.
	ai::steering::agent agent;
	
	// Wander behavior
	float wander_weight;
	float wander_noise;
	float wander_distance;
	float wander_radius;
	float wander_angle;
	float wander_angle2;
	
	// Seek behavior
	float seek_weight;
	float3 seek_target;
	
	// Flee behavior
	float flee_weight;
	
	/// Sum of steering behavior weights
	float sum_weights;
};

} // namespace component
} // namespace entity

#endif // ANTKEEPER_ENTITY_COMPONENT_STEERING_HPP
