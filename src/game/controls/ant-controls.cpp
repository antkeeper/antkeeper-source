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

#include "game/controls.hpp"
#include "game/components/ant-caste-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include "game/components/legged-locomotion-component.hpp"
#include "game/components/ovary-component.hpp"
#include <engine/math/interpolation.hpp>
#include <engine/debug/log.hpp>

namespace {
	
	/**
	 * Updates the locomotive speed of the controlled ant.
	 *
	 * @param ctx Game context.
	 */
	void update_controlled_ant_speed(::game& ctx)
	{
		if (ctx.controlled_ant_eid == entt::null)
		{
			return;
		}
		
		// Get phenome of controlled ant caste
		const auto& caste_phenome = *ctx.entity_registry->get<::ant_caste_component>(ctx.controlled_ant_eid).phenome;
		
		// Determine locomotive speed
		float locomotive_speed;
		if (ctx.ant_move_slow_action.is_active())
		{
			// Interpolate locomotive speed between walking and creeping
			locomotive_speed = math::lerp(caste_phenome.legs->walking_speed, caste_phenome.legs->creeping_speed, ctx.ant_move_slow_action.get_input_value());
		}
		else if (ctx.ant_move_fast_action.is_active())
		{
			// Interpolate locomotive speed between walking and running
			locomotive_speed = math::lerp(caste_phenome.legs->walking_speed, caste_phenome.legs->running_speed, ctx.ant_move_fast_action.get_input_value());
		}
		else
		{
			// Set locomotive speed to walking speed
			locomotive_speed = caste_phenome.legs->walking_speed;
		}
		
		// Scale locomotive speed by move ant forward action input value
		locomotive_speed *= ctx.ant_move_forward_action.get_input_value();
		
		// Scale locomotive speed by scale of controlled ant
		locomotive_speed *= ctx.entity_registry->get<rigid_body_component>(ctx.controlled_ant_eid).body->get_transform().scale.x();
		
		// Update speed of legged locomotion component
		ctx.entity_registry->patch<::legged_locomotion_component>
		(
			ctx.controlled_ant_eid,
			[&](auto& component)
			{
				component.speed = locomotive_speed;
			}
		);
	}
	
	/**
	 * Turns the controlled ant.
	 *
	 * @param ctx Game context.
	 * @param scale Angular velocity scale.
	 */
	void turn_controlled_ant(::game& ctx, float scale)
	{
		if (ctx.controlled_ant_eid == entt::null)
		{
			return;
		}
		
		ctx.entity_registry->patch<::legged_locomotion_component>
		(
			ctx.controlled_ant_eid,
			[&](auto& component)
			{
				component.angular_velocity = math::radians(120.0f) * scale;
			}
		);
	}
}

void setup_ant_controls(::game& ctx)
{	
	// Ant move forward
	ctx.event_subscriptions.emplace_back
	(
		ctx.ant_move_forward_action.get_active_channel().subscribe
		(
			[&](const auto& event)
			{
				update_controlled_ant_speed(ctx);
			}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.ant_move_forward_action.get_deactivated_channel().subscribe
		(
			[&](const auto& event)
			{
				update_controlled_ant_speed(ctx);
			}
		)
	);
	
	// Ant move back
	ctx.event_subscriptions.emplace_back
	(
		ctx.ant_move_back_action.get_active_channel().subscribe
		(
			[&](const auto& event)
			{
				update_controlled_ant_speed(ctx);
			}
		)
	);
	
	// Ant move left
	ctx.event_subscriptions.emplace_back
	(
		ctx.ant_move_left_action.get_active_channel().subscribe
		(
			[&](const auto& event)
			{
				turn_controlled_ant(ctx, event.input_value);
			}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.ant_move_left_action.get_deactivated_channel().subscribe
		(
			[&](const auto& event)
			{
				turn_controlled_ant(ctx, 0.0f);
			}
		)
	);
	
	// Ant move right
	ctx.event_subscriptions.emplace_back
	(
		ctx.ant_move_right_action.get_active_channel().subscribe
		(
			[&](const auto& event)
			{
				turn_controlled_ant(ctx, -event.input_value);
			}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.ant_move_right_action.get_deactivated_channel().subscribe
		(
			[&](const auto& event)
			{
				turn_controlled_ant(ctx, 0.0f);
			}
		)
	);
	
	// Ant oviposit
	ctx.event_subscriptions.emplace_back
	(
		ctx.ant_oviposit_action.get_activated_channel().subscribe
		(
			[&](const auto& event)
			{
				if (ctx.controlled_ant_eid == entt::null)
				{
					return;
				}
				
				if (auto ovary_component = ctx.entity_registry->try_get<ovary_component>(ctx.controlled_ant_eid))
				{
					ovary_component->ovipositing = true;
				}
			}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.ant_oviposit_action.get_deactivated_channel().subscribe
		(
			[&](const auto& event)
			{
				if (ctx.controlled_ant_eid == entt::null)
				{
					return;
				}
				
				if (auto ovary_component = ctx.entity_registry->try_get<ovary_component>(ctx.controlled_ant_eid))
				{
					ovary_component->ovipositing = false;
				}
			}
		)
	);
}

void enable_ant_controls(::game& ctx)
{
	ctx.ant_action_map.enable();
}

void disable_ant_controls(::game& ctx)
{
	ctx.ant_action_map.disable();
	ctx.ant_action_map.reset();
}
