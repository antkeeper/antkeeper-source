// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_STEERING_COMPONENT_HPP
#define ANTKEEPER_GAME_STEERING_COMPONENT_HPP

#include <engine/ai/steering/agent.hpp>


struct steering_component
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
	math::fvec3 seek_target;
	
	// Flee behavior
	float flee_weight;
	
	/// Sum of steering behavior weights
	float sum_weights;
};


#endif // ANTKEEPER_GAME_STEERING_COMPONENT_HPP
