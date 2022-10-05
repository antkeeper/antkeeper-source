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

#include "game/ant/swarm.hpp"
#include "game/component/transform.hpp"
#include "game/component/steering.hpp"
#include "game/component/model.hpp"
#include "resources/resource-manager.hpp"
#include "config.hpp"
#include <cmath>
#include <random>

namespace game {
namespace ant {

/**
 * Generates a random point in a unit sphere.
 *
 * @see https://math.stackexchange.com/questions/87230/picking-random-points-in-the-volume-of-sphere-with-uniform-probability/87238#87238
 */
template <class T, class Generator>
static math::vector3<T> sphere_random(Generator& rng)
{
	const std::uniform_real_distribution<T> distribution(T{-1}, T{1});
	
	const T x = distribution(rng);
	const T y = distribution(rng);
	const T z = distribution(rng);
	
	const T cbrt_u = std::cbrt(distribution(rng));
	
	const T scale = (T{1} / std::sqrt(x * x + y * y + z * z)) * cbrt_u;
	
	return {x * scale, y * scale, z * scale};
}

void create_swarm(game::context& ctx)
{
	// Determine swarm properties
	const float3 swarm_center = {0, 100, 0};
	const float swarm_radius = 25.0f;
	const std::size_t male_count = 50;
	const std::size_t queen_count = 50;
	const std::size_t alate_count = male_count + queen_count;
	
	const float3 male_scale = {0.5, 0.5, 0.5};
	const float3 queen_scale = {1, 1, 1};
	
	// Init male model component
	game::component::model male_model;
	male_model.render_model = ctx.resource_manager->load<render::model>("male-boid.mdl");
	male_model.instance_count = 0;
	male_model.layers = 1;
	
	// Init queen model component
	game::component::model queen_model;
	queen_model.render_model = ctx.resource_manager->load<render::model>("queen-boid.mdl");
	queen_model.instance_count = 0;
	queen_model.layers = 1;
	
	// Init transform component
	game::component::transform transform;
	transform.local = math::transform<float>::identity;
	transform.world = transform.local;
	transform.warp = true;
	
	// Init steering component
	game::component::steering steering;
	steering.agent.mass = 1.0f;
	steering.agent.velocity = {0, 0, 0};
	steering.agent.acceleration = {0, 0, 0};
	steering.agent.max_force = 4.0f;
	steering.agent.max_speed = 5.0f;
	steering.agent.max_speed_squared = steering.agent.max_speed * steering.agent.max_speed;
	steering.agent.orientation = math::quaternion<float>::identity;
	steering.agent.forward = steering.agent.orientation * config::global_forward;
	steering.agent.up = steering.agent.orientation * config::global_up;
	steering.wander_weight = 1.0f;
	steering.wander_noise = math::radians(2000.0f);
	steering.wander_distance = 10.0f;
	steering.wander_radius = 8.0f;
	steering.wander_angle = 0.0f;
	steering.wander_angle2 = 0.0f;
	steering.seek_weight = 0.2f;
	steering.seek_target = {0, 100, 0};
	steering.flee_weight = 0.0f;
	steering.sum_weights = steering.wander_weight + steering.seek_weight + steering.flee_weight;
	
	// Construct and seed random number generator
	std::random_device seed;
    std::mt19937 rng(seed());
	
	// Create alates
	for (std::size_t i = 0; i < alate_count; ++i)
	{
		// Generate random position in swarm sphere
		steering.agent.position = swarm_center + sphere_random<float>(rng) * swarm_radius;
		transform.local.translation = steering.agent.position;
		
		entity::id alate_eid = ctx.entity_registry->create();
		ctx.entity_registry->assign<game::component::steering>(alate_eid, steering);
		
		if (i < male_count)
		{
			// Create male
			ctx.entity_registry->assign<game::component::model>(alate_eid, male_model);
			
			transform.local.scale = male_scale;
			transform.world = transform.local;
			ctx.entity_registry->assign<game::component::transform>(alate_eid, transform);
		}
		else
		{
			// Create queen
			ctx.entity_registry->assign<game::component::model>(alate_eid, queen_model);
			
			transform.local.scale = queen_scale;
			transform.world = transform.local;
			ctx.entity_registry->assign<game::component::transform>(alate_eid, transform);
		}
	}
}

void destroy_swarm(game::context& ctx)
{
	auto view = ctx.entity_registry->view<game::component::steering>();
	ctx.entity_registry->destroy(view.begin(), view.end());
}

} // namespace ant
} // namespace game
