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

#include "game/state/brood.hpp"
#include "entity/archetype.hpp"
#include "entity/commands.hpp"
#include "entity/components/observer.hpp"
#include "entity/components/camera.hpp"
#include "entity/components/terrain.hpp"
#include "entity/components/transform.hpp"
#include "entity/components/chamber.hpp"
#include "entity/components/constraints/spring-to.hpp"
#include "entity/components/constraints/three-dof.hpp"
#include "entity/components/constraint-stack.hpp"
#include "animation/screen-transition.hpp"
#include "animation/ease.hpp"
#include "resources/resource-manager.hpp"
#include "render/passes/sky-pass.hpp"
#include "application.hpp"
#include <iostream>

namespace game {
namespace state {
namespace brood {

static void setup_nest(game::context* ctx);
static void setup_ants(game::context* ctx);
static void setup_camera(game::context* ctx);
static void setup_controls(game::context* ctx);

void enter(game::context* ctx)
{
	setup_nest(ctx);
	setup_ants(ctx);
	setup_camera(ctx);
	setup_controls(ctx);
	
	ctx->underground_ambient_light->set_intensity(1.0f);
	
	// Create larva
	{
		entity::archetype* larva_archetype = ctx->resource_manager->load<entity::archetype>("ant-larva.ent");
		auto larva_eid = larva_archetype->create(*ctx->entity_registry);
		entity::command::warp_to(*ctx->entity_registry, larva_eid, {0.0f, 0.0f, 0.0f});
		entity::command::assign_render_layers(*ctx->entity_registry, larva_eid, 0b1);
	}
	
	// Create cocoon
	{
		entity::archetype* cocoon_archetype = ctx->resource_manager->load<entity::archetype>("ant-cocoon.ent");
		auto cocoon_eid = cocoon_archetype->create(*ctx->entity_registry);
		entity::command::warp_to(*ctx->entity_registry, cocoon_eid, {-50, 0.1935f, 0});
		entity::command::assign_render_layers(*ctx->entity_registry, cocoon_eid, 0b1);
	}
	
	// Reset tweening
	ctx->underground_scene->update_tweens();
	
	// Start fade in
	ctx->fade_transition->transition(1.0f, true, ease<float>::in_quad);
}

void exit(game::context* ctx)
{}

void setup_nest(game::context* ctx)
{
	// Create nest central shaft entity
	if (!ctx->entities.count("shaft"))
	{
		entity::id shaft_eid = ctx->entity_registry->create();
		ctx->entities["shaft"] = shaft_eid;
		
		entity::component::transform transform;
		transform.local = math::transform<float>::identity;
		transform.world = transform.local;
		transform.warp = true;
		
		ctx->entity_registry->assign<entity::component::transform>(shaft_eid, transform);
	}
	
	// Create nest lobby chamber entity
	if (!ctx->entities.count("lobby"))
	{
		entity::id lobby_eid = ctx->entity_registry->create();
		ctx->entities["lobby"] = lobby_eid;
		
		entity::component::chamber chamber;
		chamber.shaft_eid = ctx->entities["shaft"];
		chamber.distance = 10.0f;
		chamber.previous_chamber_eid = entt::null;
		chamber.next_chamber_eid = entt::null;
	}
}

void setup_ants(game::context* ctx)
{
	// Create queen ant entity
	if (!ctx->entities.count("queen"))
	{
		entity::id queen_eid = ctx->entity_registry->create();
		ctx->entities["queen"] = queen_eid;
	}
}

void setup_camera(game::context* ctx)
{
	// Switch to underground camera
	ctx->surface_camera->set_active(false);
	ctx->underground_camera->set_active(true);
	
	// Create underground camera entity
	if (!ctx->entities.count("underground_cam"))
	{
		// Create camera target entity
		entity::id target_eid = ctx->entity_registry->create();
		ctx->entities["underground_cam_target"] = target_eid;
		{
			// Transform
			entity::component::transform target_transform;
			target_transform.local = math::transform<float>::identity;
			target_transform.world = target_transform.local;
			target_transform.warp = true;
			ctx->entity_registry->assign<entity::component::transform>(target_eid, target_transform);
		}
		
		// Create camera entity
		entity::id camera_eid = ctx->entity_registry->create();
		ctx->entities["underground_cam"] = camera_eid;
		
		// Create camera transform component
		entity::component::transform transform;
		transform.local = math::transform<float>::identity;
		transform.world = transform.local;
		transform.warp = true;
		ctx->entity_registry->assign<entity::component::transform>(camera_eid, transform);
		
		// Create camera camera component
		entity::component::camera camera;
		camera.object = ctx->underground_camera;
		ctx->entity_registry->assign<entity::component::camera>(camera_eid, camera);
		
		// Create camera 3DOF constraint entity
		entity::id three_dof_constraint_eid = ctx->entity_registry->create();
		ctx->entities["underground_cam_3dof"] = three_dof_constraint_eid;
		{
			// Create 3DOF to constraint
			entity::component::constraint::three_dof three_dof;
			three_dof.yaw = 0.0f;
			three_dof.pitch = 0.0f;
			three_dof.roll = 0.0f;
			ctx->entity_registry->assign<entity::component::constraint::three_dof>(three_dof_constraint_eid, three_dof);
			
			// Create constraint stack node component
			entity::component::constraint_stack_node node;
			node.active = true;
			node.weight = 1.0f;
			node.next = entt::null;
			ctx->entity_registry->assign<entity::component::constraint_stack_node>(three_dof_constraint_eid, node);
		}
		
		// Create camera spring to constraint entity
		entity::id spring_constraint_eid = ctx->entity_registry->create();
		{
			// Create spring to constraint
			entity::component::constraint::spring_to spring;
			spring.target = target_eid;
			spring.translation = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 1.0f, math::two_pi<float>};
			spring.translation.w = hz_to_rads(8.0f);
			
			spring.spring_translation = true;
			spring.spring_rotation = false;
			ctx->entity_registry->assign<entity::component::constraint::spring_to>(spring_constraint_eid, spring);
			
			// Create constraint stack node component
			entity::component::constraint_stack_node node;
			node.active = true;
			node.weight = 1.0f;
			node.next = three_dof_constraint_eid;
			ctx->entity_registry->assign<entity::component::constraint_stack_node>(spring_constraint_eid, node);
		}
		
		// Create camera constraint stack component
		entity::component::constraint_stack constraint_stack;
		constraint_stack.head = spring_constraint_eid;
		ctx->entity_registry->assign<entity::component::constraint_stack>(camera_eid, constraint_stack);
	}
	
	ctx->underground_camera->set_exposure(0.0f);
}

void setup_controls(game::context* ctx)
{
	// Get underground camera entity
	entity::id camera_eid = ctx->entities["underground_cam"];
	entity::id target_eid = ctx->entities["underground_cam_target"];
	entity::id three_dof_eid = ctx->entities["underground_cam_3dof"];
	
	const float dolly_speed = 20.0f;
	const float truck_speed = dolly_speed;
	const float pedestal_speed = 30.0f;
	const float pan_speed = math::radians(8.0f);
	const float tilt_speed = pan_speed;
	
	const input::control* move_slow = ctx->controls["move_slow"];
	const input::control* move_fast = ctx->controls["move_fast"];
	const input::control* mouse_rotate = ctx->controls["mouse_rotate"];
	
	ctx->controls["dolly_forward"]->set_active_callback
	(
		[ctx, target_eid, three_dof_eid, truck_speed, move_slow, move_fast](float value)
		{
			if (move_slow->is_active())
				value *= 0.5f;
			if (move_fast->is_active())
				value *= 2.0f;
			
			auto& three_dof = ctx->entity_registry->get<entity::component::constraint::three_dof>(three_dof_eid);
			const math::quaternion<float> yaw = math::angle_axis(three_dof.yaw, {0.0f, 1.0f, 0.0f});

			const float3 movement = {0.0f, 0.0f, -truck_speed * value * (1.0f / 60.0f)};
			entity::command::translate(*ctx->entity_registry, target_eid, yaw * movement);
		}
	);
	
	// Dolly backward
	ctx->controls["dolly_backward"]->set_active_callback
	(
		[ctx, target_eid, three_dof_eid, truck_speed, move_slow, move_fast](float value)
		{
			if (move_slow->is_active())
				value *= 0.5f;
			if (move_fast->is_active())
				value *= 2.0f;
			
			auto& three_dof = ctx->entity_registry->get<entity::component::constraint::three_dof>(three_dof_eid);
			const math::quaternion<float> yaw = math::angle_axis(three_dof.yaw, {0.0f, 1.0f, 0.0f});
			
			const float3 movement = {0.0f, 0.0f, truck_speed * value * (1.0f / 60.0f)};
			entity::command::translate(*ctx->entity_registry, target_eid, yaw * movement);
		}
	);
	
	// Truck right
	ctx->controls["truck_right"]->set_active_callback
	(
		[ctx, target_eid, three_dof_eid, truck_speed, move_slow, move_fast](float value)
		{
			if (move_slow->is_active())
				value *= 0.5f;
			if (move_fast->is_active())
				value *= 2.0f;
			
			auto& three_dof = ctx->entity_registry->get<entity::component::constraint::three_dof>(three_dof_eid);
			const math::quaternion<float> yaw = math::angle_axis(three_dof.yaw, {0.0f, 1.0f, 0.0f});
			
			const float3 movement = {truck_speed * value * (1.0f / 60.0f), 0.0f, 0.0f};
			entity::command::translate(*ctx->entity_registry, target_eid, yaw * movement);
		}
	);
	
	// Truck left
	ctx->controls["truck_left"]->set_active_callback
	(
		[ctx, target_eid, three_dof_eid, truck_speed, move_slow, move_fast](float value)
		{
			if (move_slow->is_active())
				value *= 0.5f;
			if (move_fast->is_active())
				value *= 2.0f;
			
			auto& three_dof = ctx->entity_registry->get<entity::component::constraint::three_dof>(three_dof_eid);
			const math::quaternion<float> yaw = math::angle_axis(three_dof.yaw, {0.0f, 1.0f, 0.0f});
			
			const float3 movement = {-truck_speed * value * (1.0f / 60.0f), 0.0f, 0.0f};
			entity::command::translate(*ctx->entity_registry, target_eid, yaw * movement);
		}
	);
	
	// Pedestal up
	ctx->controls["pedestal_up"]->set_active_callback
	(
		[ctx, target_eid, pedestal_speed, move_slow, move_fast](float value)
		{
			if (move_slow->is_active())
				value *= 0.5f;
			if (move_fast->is_active())
				value *= 2.0f;
				
			const float3 movement = {0.0f, pedestal_speed * value * (1.0f / 60.0f), 0.0f};
			entity::command::translate(*ctx->entity_registry, target_eid, movement);
		}
	);
	
	// Pedestal down
	ctx->controls["pedestal_down"]->set_active_callback
	(
		[ctx, target_eid, pedestal_speed, move_slow, move_fast](float value)
		{
			if (move_slow->is_active())
				value *= 0.5f;
			if (move_fast->is_active())
				value *= 2.0f;
				
			const float3 movement = {0.0f, -pedestal_speed * value * (1.0f / 60.0f), 0.0f};
			entity::command::translate(*ctx->entity_registry, target_eid, movement);
		}
	);
	
	// Mouse rotate
	ctx->controls["mouse_rotate"]->set_activated_callback
	(
		[&ctx]()
		{
			ctx->app->set_relative_mouse_mode(true);
		}
	);
	ctx->controls["mouse_rotate"]->set_deactivated_callback
	(
		[&ctx]()
		{
			ctx->app->set_relative_mouse_mode(false);
		}
	);
	
	// Pan left
	ctx->controls["pan_left_mouse"]->set_active_callback
	(
		[ctx, three_dof_eid, pan_speed, mouse_rotate](float value)
		{
			if (!mouse_rotate->is_active())
				return;
			
			auto& three_dof = ctx->entity_registry->get<entity::component::constraint::three_dof>(three_dof_eid);
			three_dof.yaw += pan_speed * value * (1.0f / 60.0f);
		}
	);
	
	// Pan right
	ctx->controls["pan_right_mouse"]->set_active_callback
	(
		[ctx, three_dof_eid, pan_speed, mouse_rotate](float value)
		{
			if (!mouse_rotate->is_active())
				return;
			
			auto& three_dof = ctx->entity_registry->get<entity::component::constraint::three_dof>(three_dof_eid);
			three_dof.yaw -= pan_speed * value * (1.0f / 60.0f);
		}
	);
	
	// Tilt up
	ctx->controls["tilt_up_mouse"]->set_active_callback
	(
		[ctx, three_dof_eid, tilt_speed, mouse_rotate](float value)
		{
			if (!mouse_rotate->is_active())
				return;
			
			auto& three_dof = ctx->entity_registry->get<entity::component::constraint::three_dof>(three_dof_eid);
			three_dof.pitch -= tilt_speed * value * (1.0f / 60.0f);
			three_dof.pitch = std::max<float>(math::radians(-90.0f), three_dof.pitch);
		}
	);
	
	// Tilt down
	ctx->controls["tilt_down_mouse"]->set_active_callback
	(
		[ctx, three_dof_eid, tilt_speed, mouse_rotate](float value)
		{
			if (!mouse_rotate->is_active())
				return;
			
			auto& three_dof = ctx->entity_registry->get<entity::component::constraint::three_dof>(three_dof_eid);
			three_dof.pitch += tilt_speed * value * (1.0f / 60.0f);
			three_dof.pitch = std::min<float>(math::radians(90.0f), three_dof.pitch);
		}
	);
}

} // namespace brood
} // namespace state
} // namespace game
