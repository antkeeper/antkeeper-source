// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <entt/entt.hpp>
#include "game/components/transform-component.hpp"
#include "game/components/winged-locomotion-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include "game/systems/steering-system.hpp"
#include "game/components/steering-component.hpp"
#include <engine/entity/id.hpp>
#include <engine/ai/steering/behavior/seek.hpp>
#include <engine/ai/steering/behavior/wander.hpp>
#include <engine/math/functions.hpp>
#include <engine/math/quaternion.hpp>

void steering_system::fixed_update(entity::registry& registry, float, float dt)
{
	registry.group<steering_component>(entt::get<transform_component, winged_locomotion_component, rigid_body_component>).each
	(
		[&](entity::id entity_id, auto& steering, auto& transform, auto&, const auto& body_component)
		{
			auto& agent = steering.agent;
			auto& body = *body_component.body;
			
			// Update agent parameters
			agent.position = transform.local.translation;
			agent.orientation = transform.local.rotation;
			agent.velocity = body.get_linear_velocity();
			
			// Accumulate steering forces
			math::fvec3 force = {0, 0, 0};
			if (steering.wander_weight)
			{
				//force += ai::steering::behavior::wander_2d(agent, steering.wander_noise * dt, steering.wander_distance, steering.wander_radius, steering.wander_angle) * steering.wander_weight;
				force += ai::steering::behavior::wander_3d(agent, steering.wander_noise * dt, steering.wander_distance, steering.wander_radius, steering.wander_angle, steering.wander_angle2) * steering.wander_weight;
			}
			if (steering.seek_weight)
			{
				force += ai::steering::behavior::seek(agent, steering.seek_target) * steering.seek_weight;
			}
			
			// Normalize force
			if (steering.sum_weights)
			{
				force /= steering.sum_weights;
			}
			
			// Pass force to winged locomotion component
			registry.patch<::winged_locomotion_component>
			(
				entity_id,
				[&](auto& component)
				{
					component.force = force;
				}
			);
			
			// Rotate agent
			const float speed_squared = math::sqr_length(agent.velocity);
			if (speed_squared)
			{
				agent.orientation = math::look_rotation(agent.velocity / math::sqrt(speed_squared), agent.up);
				agent.forward = agent.orientation * global_forward;
				agent.up = agent.orientation * global_up;
			}
			
			// Update orientation
			registry.patch<::transform_component>
			(
				entity_id,
				[&agent](auto& component)
				{
					component.local.rotation = agent.orientation;
				}
			);
		}
	);
}
