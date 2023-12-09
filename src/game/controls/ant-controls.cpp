// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/controls.hpp"
#include "game/world.hpp"
#include "game/components/ant-caste-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include "game/components/legged-locomotion-component.hpp"
#include "game/components/ovary-component.hpp"
#include "game/components/spring-arm-component.hpp"
#include "game/components/scene-component.hpp"
#include <engine/math/functions.hpp>
#include <engine/math/euler-angles.hpp>
#include <engine/debug/log.hpp>

namespace {
	
	/**
	 * Updates the locomotive speed of the controlled ant.
	 *
	 * @param ctx Game context.
	 */
	void steer_controlled_ant(::game& ctx)
	{
		if (ctx.controlled_ant_eid == entt::null)
		{
			return;
		}
		
		// Build control vector
		math::fvec2 control_vector = {};
		control_vector.x() -= ctx.ant_move_left_action.get_input_value();
		control_vector.x() += ctx.ant_move_right_action.get_input_value();
		control_vector.y() -= ctx.ant_move_forward_action.get_input_value();
		control_vector.y() += ctx.ant_move_back_action.get_input_value();
		
		// Get locomotion component of controlled ant
		auto& locomotion = ctx.entity_registry->get<legged_locomotion_component>(ctx.controlled_ant_eid);
		
		// Get phenome of controlled ant caste
		const auto& caste_phenome = *ctx.entity_registry->get<::ant_caste_component>(ctx.controlled_ant_eid).phenome;
		
		// Determine control direction and magnitude
		math::fvec2 control_direction{};
		float control_magnitude = 0.0f;
		if (auto sqr_length = math::sqr_length(control_vector))
		{
			control_magnitude = std::sqrt(sqr_length);
			control_direction = control_vector / control_magnitude;
		}
		
		// Clamp control magnitude
		const auto clamped_control_magnitude = std::min(1.0f, control_magnitude);
		
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
		
		// Scale locomotive speed by clamped control magnitude
		locomotive_speed *= clamped_control_magnitude;
		
		// Scale locomotive speed by scale of controlled ant
		auto& rigid_body = *ctx.entity_registry->get<rigid_body_component>(ctx.controlled_ant_eid).body;
		locomotive_speed *= rigid_body.get_scale().x();
		
		locomotion.speed = locomotive_speed;
		locomotion.angular_velocity = 0.0f;
		
		if (!locomotive_speed || ctx.active_camera_eid == entt::null)
		{
			return;
		}
		
		const auto& spring_arm = ctx.entity_registry->get<spring_arm_component>(ctx.active_camera_eid);
		
		// Calculate steering direction
		const auto spring_arm_yaw_rotation = math::angle_axis(spring_arm.angles_spring.get_value().y(), {0.0, 1.0, 0.0});
		locomotion.target_direction = math::fquat(math::normalize(spring_arm.up_rotation * spring_arm_yaw_rotation)) * math::fvec3{control_direction.x(), 0.0f, (spring_arm.angles_spring.get_value().x() > 0.0) ? -control_direction.y() : control_direction.y()};
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
				steer_controlled_ant(ctx);
			}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.ant_move_forward_action.get_deactivated_channel().subscribe
		(
			[&](const auto& event)
			{
				steer_controlled_ant(ctx);
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
				steer_controlled_ant(ctx);
			}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.ant_move_back_action.get_deactivated_channel().subscribe
		(
			[&](const auto& event)
			{
				steer_controlled_ant(ctx);
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
				steer_controlled_ant(ctx);
			}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.ant_move_left_action.get_deactivated_channel().subscribe
		(
			[&](const auto& event)
			{
				steer_controlled_ant(ctx);
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
				steer_controlled_ant(ctx);
			}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.ant_move_right_action.get_deactivated_channel().subscribe
		(
			[&](const auto& event)
			{
				steer_controlled_ant(ctx);
			}
		)
	);
	
	// Ant interact
	ctx.event_subscriptions.emplace_back
	(
		ctx.ant_interact_action.get_activated_channel().subscribe
		(
			[&](const auto& event)
			{
				if (ctx.active_camera_eid == entt::null)
				{
					return;
				}
				
				auto& camera_object = *ctx.entity_registry->get<scene_component>(ctx.active_camera_eid).object;
				
				camera_object.set_layer_mask(camera_object.get_layer_mask() == 1 ? 2 : 1);
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
