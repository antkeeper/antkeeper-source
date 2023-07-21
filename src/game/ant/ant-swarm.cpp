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

#include "game/ant/ant-swarm.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/steering-component.hpp"
#include "game/components/scene-component.hpp"
#include "game/components/picking-component.hpp"
#include "game/components/winged-locomotion-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include "game/components/ant-caste-component.hpp"
#include <engine/resources/resource-manager.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/math/angles.hpp>
#include <engine/scene/static-mesh.hpp>
#include <engine/config.hpp>
#include <cmath>
#include <random>

/**
 * Generates a random point in a unit sphere.
 *
 * @see https://math.stackexchange.com/questions/87230/picking-random-points-in-the-volume-of-sphere-with-uniform-probability/87238#87238
 */
template <class T, class URBG>
static math::vec3<T> sphere_random(URBG& urbg)
{
	std::uniform_real_distribution<T> distribution(T{-1}, T{1});
	
	math::vec3<T> position;
	for (std::size_t i = 0; i < 3; ++i)
		position[i] = distribution(urbg);
	
	return math::normalize(position) * std::cbrt(distribution(urbg));
}

entity::id create_ant_swarm(::game& ctx)
{
	// Determine swarm properties
	const math::fvec3 swarm_center = {0, 100, 0};
	const float swarm_radius = 25.0f;
	const std::size_t male_count = 50;
	const std::size_t queen_count = 50;
	const std::size_t alate_count = male_count + queen_count;
	
	const math::fvec3 male_scale = {0.5, 0.5, 0.5};
	const math::fvec3 queen_scale = {1, 1, 1};
	
	// Init transform component
	::transform_component transform;
	transform.local = math::transform<float>::identity();
	transform.world = transform.local;
	
	// Init picking component
	::picking_component picking;
	picking.sphere = {math::fvec3{0, 0, 0}, 0.5f * 2.0f};
	std::uint32_t male_picking_flags = 0b01;
	std::uint32_t queen_picking_flags = 0b10;
	
	// Create swarm entity
	entity::id swarm_eid = ctx.entity_registry->create();
	transform.local.translation = swarm_center;
	transform.world = transform.local;
	ctx.entity_registry->emplace<::transform_component>(swarm_eid, transform);
	
	// Load male model
	std::shared_ptr<render::model> male_model = ctx.resource_manager->load<render::model>("male-boid.mdl");
	
	// Load queen model
	std::shared_ptr<render::model> queen_model = ctx.resource_manager->load<render::model>("queen-boid.mdl");
	
	// Init steering component
	::steering_component steering;
	steering.agent.mass = 1.0f;
	steering.agent.velocity = {0, 0, 0};
	steering.agent.acceleration = {0, 0, 0};
	steering.agent.max_force = 4.0f;
	steering.agent.max_speed = 5.0f;
	steering.agent.max_speed_squared = steering.agent.max_speed * steering.agent.max_speed;
	steering.agent.orientation = math::fquat::identity();
	steering.agent.forward = steering.agent.orientation * config::global_forward;
	steering.agent.up = steering.agent.orientation * config::global_up;
	steering.wander_weight = 1.0f;
	steering.wander_noise = math::radians(2000.0f);
	steering.wander_distance = 10.0f;
	steering.wander_radius = 8.0f;
	steering.wander_angle = 0.0f;
	steering.wander_angle2 = 0.0f;
	steering.seek_weight = 0.2f;
	steering.seek_target = swarm_center;
	steering.flee_weight = 0.0f;
	steering.sum_weights = steering.wander_weight + steering.seek_weight + steering.flee_weight;
	
	// Init rigid body
	physics::rigid_body rigid_body;
	rigid_body.set_mass(1.0f);
	
	// Init winged locomotion component
	winged_locomotion_component winged_locomotion;
	
	// Init queen caste component
	ant_caste_component queen_caste;
	queen_caste.caste_type = ant_caste_type::queen;
	
	// Init male caste component
	ant_caste_component male_caste;
	male_caste.caste_type = ant_caste_type::male;
	
	// Create alates
	for (std::size_t i = 0; i < alate_count; ++i)
	{
		// Generate random position in swarm sphere
		steering.agent.position = swarm_center + sphere_random<float>(ctx.rng) * swarm_radius;
		transform.local.translation = steering.agent.position;
		
		entity::id alate_eid = ctx.entity_registry->create();
		ctx.entity_registry->emplace<::steering_component>(alate_eid, steering);
		ctx.entity_registry->emplace<::rigid_body_component>(alate_eid, std::make_unique<physics::rigid_body>(rigid_body));
		ctx.entity_registry->emplace<::winged_locomotion_component>(alate_eid, winged_locomotion);
		
		if (i < male_count)
		{
			// Create male
			ctx.entity_registry->emplace<ant_caste_component>(alate_eid, male_caste);
			ctx.entity_registry->emplace<::scene_component>(alate_eid, std::make_unique<scene::static_mesh>(male_model), std::uint8_t{1});

			
			transform.local.scale = male_scale;
			transform.world = transform.local;
			ctx.entity_registry->emplace<::transform_component>(alate_eid, transform);
			
			picking.flags = male_picking_flags;
			ctx.entity_registry->emplace<::picking_component>(alate_eid, picking);
		}
		else
		{
			// Create queen
			ctx.entity_registry->emplace<ant_caste_component>(alate_eid, queen_caste);
			ctx.entity_registry->emplace<::scene_component>(alate_eid, std::make_unique<scene::static_mesh>(queen_model), std::uint8_t{1});
			
			transform.local.scale = queen_scale;
			transform.world = transform.local;
			ctx.entity_registry->emplace<::transform_component>(alate_eid, transform);
			
			picking.flags = queen_picking_flags;
			ctx.entity_registry->emplace<::picking_component>(alate_eid, picking);
		}
	}
	
	return swarm_eid;
}

void destroy_ant_swarm(::game& ctx, entity::id swarm_eid)
{
	// Destroy alates
	auto view = ctx.entity_registry->view<::steering_component>();
	ctx.entity_registry->destroy(view.begin(), view.end());
	
	// Destroy swarm
	ctx.entity_registry->destroy(swarm_eid);
}
