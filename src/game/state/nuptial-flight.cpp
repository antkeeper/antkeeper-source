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

#include "game/state/nuptial-flight.hpp"
#include "game/state/pause-menu.hpp"
#include "entity/archetype.hpp"
#include "entity/systems/astronomy.hpp"
#include "entity/systems/orbit.hpp"
#include "entity/systems/camera.hpp"
#include "entity/components/observer.hpp"
#include "entity/components/transform.hpp"
#include "entity/components/terrain.hpp"
#include "entity/components/camera.hpp"
#include "entity/components/constraints/spring-to.hpp"
#include "entity/components/constraints/three-dof.hpp"
#include "entity/components/constraint-stack.hpp"
#include "entity/commands.hpp"
#include "animation/screen-transition.hpp"
#include "animation/ease.hpp"
#include "resources/resource-manager.hpp"
#include "game/world.hpp"
#include "application.hpp"
#include "render/passes/clear-pass.hpp"
#include <memory>
#include <iostream>
#include "state-machine.hpp"

namespace game {
namespace state {

nuptial_flight::nuptial_flight(game::context& ctx):
	game::state::base(ctx)
{
	ctx.logger->push_task("Entering nuptial flight state");
	
	// Disable UI color clear
	ctx.ui_clear_pass->set_cleared_buffers(false, true, false);
	
	// Create world
	game::world::create_stars(ctx);
	game::world::create_sun(ctx);
	game::world::create_planet(ctx);
	game::world::create_moon(ctx);
	
	// Set time to solar noon
	game::world::set_time(ctx, 0.0);
	
	// Freeze time
	game::world::set_time_scale(ctx, 0.0);
	
	// Switch to surface camera
	ctx.underground_camera->set_active(false);
	ctx.surface_camera->set_active(true);
	
	// Find planet EID by name
	entity::id planet_eid = ctx.entities["planet"];
	
	// Remove terrain component from planet (if any)
	//if (ctx.entity_registry->has<entity::component::terrain>(planet_eid))
	//	ctx.entity_registry->remove<entity::component::terrain>(planet_eid);
	
	// Enable clouds in sky pass
	//ctx.surface_sky_pass->set_clouds_model(ctx.resource_manager->load<render::model>("cloud-plane.mdl"));
	
	// Create biome terrain component
	entity::component::terrain biome_terrain;
	biome_terrain.max_lod = 18;
	biome_terrain.patch_material = ctx.resource_manager->load<render::material>("desert-terrain.mtl");
	biome_terrain.elevation = [](double, double) -> double
	{
		return 0.0;
	};
	// Replace planet terrain component with biome terrain component
	ctx.entity_registry->replace<entity::component::terrain>(planet_eid, biome_terrain);
	
	// Create observer
	entity::id observer_eid = ctx.entity_registry->create();
	{
		entity::component::observer observer;
		observer.reference_body_eid = planet_eid;
		observer.elevation = 2000.0;
		observer.latitude = 0.0;
		observer.longitude = 0.0;
		observer.camera = ctx.surface_camera;
		ctx.entity_registry->assign<entity::component::observer>(observer_eid, observer);
		
		// Set reference location of astronomy system
		ctx.astronomy_system->set_reference_body(planet_eid);
		ctx.astronomy_system->set_observer_location(double3{observer.elevation, observer.latitude, observer.longitude});
	}
	
	// Setup camera
	setup_camera();
	/*
	ctx.surface_camera->look_at({0, 0, 1}, {0, 0, 0}, {0, 1, 0});
	ctx.surface_camera->set_exposure(-14.5f);
	ctx.surface_scene->update_tweens();
	*/
	
	// Queue fade in
	ctx.fade_transition_color->set_value({1, 1, 1});
	ctx.function_queue.push(std::bind(&screen_transition::transition, ctx.fade_transition, 5.0f, true, math::lerp<float, float>, true, nullptr));
	
	// Queue control setup
	ctx.function_queue.push(std::bind(&nuptial_flight::enable_controls, this));
	
	ctx.logger->pop_task(EXIT_SUCCESS);
}

nuptial_flight::~nuptial_flight()
{
	ctx.logger->push_task("Exiting nuptial flight state");
	
	// Resume time
	//const double time_scale = (*ctx.config)["time_scale"].get<double>();
	//game::world::set_time_scale(ctx, time_scale);
	
	ctx.logger->pop_task(EXIT_SUCCESS);
}

void nuptial_flight::setup_camera()
{
	// Switch to surface camera
	ctx.underground_camera->set_active(false);
	ctx.surface_camera->set_active(true);
	
	// Create surface camera entity
	if (!ctx.entities.count("surface_cam"))
	{
		// Create camera target entity
		entity::id target_eid = ctx.entity_registry->create();
		ctx.entities["surface_cam_target"] = target_eid;
		{
			// Transform
			entity::component::transform target_transform;
			target_transform.local = math::identity_transform<float>;
			target_transform.world = target_transform.local;
			target_transform.warp = true;
			ctx.entity_registry->assign<entity::component::transform>(target_eid, target_transform);
		}
		
		// Create camera entity
		entity::id camera_eid = ctx.entity_registry->create();
		ctx.entities["surface_cam"] = camera_eid;
		
		// Create camera transform component
		entity::component::transform transform;
		transform.local = math::identity_transform<float>;
		transform.world = transform.local;
		transform.warp = true;
		ctx.entity_registry->assign<entity::component::transform>(camera_eid, transform);
		
		// Create camera camera component
		entity::component::camera camera;
		camera.object = ctx.surface_camera;
		ctx.entity_registry->assign<entity::component::camera>(camera_eid, camera);
		
		// Create camera 3DOF constraint entity
		entity::id three_dof_constraint_eid = ctx.entity_registry->create();
		ctx.entities["surface_cam_3dof"] = three_dof_constraint_eid;
		{
			// Create 3DOF to constraint
			entity::component::constraint::three_dof three_dof;
			three_dof.yaw = 0.0f;
			three_dof.pitch = 0.0f;
			three_dof.roll = 0.0f;
			ctx.entity_registry->assign<entity::component::constraint::three_dof>(three_dof_constraint_eid, three_dof);
			
			// Create constraint stack node component
			entity::component::constraint_stack_node node;
			node.active = true;
			node.weight = 1.0f;
			node.next = entt::null;
			ctx.entity_registry->assign<entity::component::constraint_stack_node>(three_dof_constraint_eid, node);
		}
		
		// Create camera spring to constraint entity
		entity::id spring_constraint_eid = ctx.entity_registry->create();
		{
			// Create spring to constraint
			entity::component::constraint::spring_to spring;
			spring.target = target_eid;
			spring.translation = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 1.0f, math::two_pi<float>};
			spring.translation.w = hz_to_rads(8.0f);
			
			spring.spring_translation = true;
			spring.spring_rotation = false;
			ctx.entity_registry->assign<entity::component::constraint::spring_to>(spring_constraint_eid, spring);
			
			// Create constraint stack node component
			entity::component::constraint_stack_node node;
			node.active = true;
			node.weight = 1.0f;
			node.next = three_dof_constraint_eid;
			ctx.entity_registry->assign<entity::component::constraint_stack_node>(spring_constraint_eid, node);
		}
		
		// Create camera constraint stack component
		entity::component::constraint_stack constraint_stack;
		constraint_stack.head = spring_constraint_eid;
		ctx.entity_registry->assign<entity::component::constraint_stack>(camera_eid, constraint_stack);
	}
	
	ctx.surface_camera->set_exposure(-14.0f);
}

void nuptial_flight::enable_controls()
{
	// Get camera entities
	entity::id camera_eid = ctx.entities["surface_cam"];
	entity::id target_eid = ctx.entities["surface_cam_target"];
	entity::id three_dof_eid = ctx.entities["surface_cam_3dof"];
	
	const float slow_modifier = 0.25f;
	const float fast_modifier = 4.0f;
	const float dolly_speed = 20.0f;
	const float truck_speed = dolly_speed;
	const float pedestal_speed = 30.0f;
	float mouse_tilt_sensitivity = 1.0f;
	float mouse_pan_sensitivity = 1.0f;
	bool mouse_invert_tilt = false;
	bool mouse_invert_pan = false;
	float gamepad_tilt_sensitivity = 1.0f;
	float gamepad_pan_sensitivity = 1.0f;
	bool gamepad_invert_tilt = false;
	bool gamepad_invert_pan = false;
	bool mouse_look_toggle = false;
	ctx.mouse_look = false;
	
	if (ctx.config->contains("mouse_tilt_sensitivity"))
		mouse_tilt_sensitivity = math::radians((*ctx.config)["mouse_tilt_sensitivity"].get<float>());
	if (ctx.config->contains("mouse_pan_sensitivity"))
		mouse_pan_sensitivity = math::radians((*ctx.config)["mouse_pan_sensitivity"].get<float>());
	if (ctx.config->contains("mouse_invert_tilt"))
		mouse_invert_tilt = math::radians((*ctx.config)["mouse_invert_tilt"].get<bool>());
	if (ctx.config->contains("mouse_invert_pan"))
		mouse_invert_pan = math::radians((*ctx.config)["mouse_invert_pan"].get<bool>());
	if (ctx.config->contains("mouse_look_toggle"))
		mouse_look_toggle = math::radians((*ctx.config)["mouse_look_toggle"].get<bool>());
	
	if (ctx.config->contains("gamepad_tilt_sensitivity"))
		gamepad_tilt_sensitivity = math::radians((*ctx.config)["gamepad_tilt_sensitivity"].get<float>());
	if (ctx.config->contains("gamepad_pan_sensitivity"))
		gamepad_pan_sensitivity = math::radians((*ctx.config)["gamepad_pan_sensitivity"].get<float>());
	if (ctx.config->contains("gamepad_invert_tilt"))
		gamepad_invert_tilt = math::radians((*ctx.config)["gamepad_invert_tilt"].get<bool>());
	if (ctx.config->contains("gamepad_invert_pan"))
		gamepad_invert_pan = math::radians((*ctx.config)["gamepad_invert_pan"].get<bool>());
	
	const input::control* move_slow = ctx.controls["move_slow"];
	const input::control* move_fast = ctx.controls["move_fast"];
	const input::control* mouse_look = ctx.controls["mouse_look"];
	
	float mouse_tilt_factor = mouse_tilt_sensitivity * (mouse_invert_tilt ? -1.0f : 1.0f);
	float mouse_pan_factor = mouse_pan_sensitivity * (mouse_invert_pan ? -1.0f : 1.0f);
	float gamepad_tilt_factor = gamepad_tilt_sensitivity * (gamepad_invert_tilt ? -1.0f : 1.0f);
	float gamepad_pan_factor = gamepad_pan_sensitivity * (gamepad_invert_pan ? -1.0f : 1.0f);
	
	ctx.controls["move_forward"]->set_active_callback
	(
		[&ctx = this->ctx, target_eid, three_dof_eid, truck_speed, move_slow, move_fast, slow_modifier, fast_modifier](float value)
		{
			if (move_slow->is_active())
				value *= slow_modifier;
			if (move_fast->is_active())
				value *= fast_modifier;
			
			auto& three_dof = ctx.entity_registry->get<entity::component::constraint::three_dof>(three_dof_eid);
			const math::quaternion<float> yaw = math::angle_axis(three_dof.yaw, {0.0f, 1.0f, 0.0f});

			const float3 movement = {0.0f, 0.0f, -truck_speed * value * (1.0f / 60.0f)};
			entity::command::translate(*ctx.entity_registry, target_eid, yaw * movement);
		}
	);
	
	// Dolly backward
	ctx.controls["move_back"]->set_active_callback
	(
		[&ctx = this->ctx, target_eid, three_dof_eid, truck_speed, move_slow, move_fast, slow_modifier, fast_modifier](float value)
		{
			if (move_slow->is_active())
				value *= slow_modifier;
			if (move_fast->is_active())
				value *= fast_modifier;
			
			auto& three_dof = ctx.entity_registry->get<entity::component::constraint::three_dof>(three_dof_eid);
			const math::quaternion<float> yaw = math::angle_axis(three_dof.yaw, {0.0f, 1.0f, 0.0f});
			
			const float3 movement = {0.0f, 0.0f, truck_speed * value * (1.0f / 60.0f)};
			entity::command::translate(*ctx.entity_registry, target_eid, yaw * movement);
		}
	);
	
	// Truck right
	ctx.controls["move_right"]->set_active_callback
	(
		[&ctx = this->ctx, target_eid, three_dof_eid, truck_speed, move_slow, move_fast, slow_modifier, fast_modifier](float value)
		{
			if (move_slow->is_active())
				value *= slow_modifier;
			if (move_fast->is_active())
				value *= fast_modifier;
			
			auto& three_dof = ctx.entity_registry->get<entity::component::constraint::three_dof>(three_dof_eid);
			const math::quaternion<float> yaw = math::angle_axis(three_dof.yaw, {0.0f, 1.0f, 0.0f});
			
			const float3 movement = {truck_speed * value * (1.0f / 60.0f), 0.0f, 0.0f};
			entity::command::translate(*ctx.entity_registry, target_eid, yaw * movement);
		}
	);
	
	// Truck left
	ctx.controls["move_left"]->set_active_callback
	(
		[&ctx = this->ctx, target_eid, three_dof_eid, truck_speed, move_slow, move_fast, slow_modifier, fast_modifier](float value)
		{
			if (move_slow->is_active())
				value *= slow_modifier;
			if (move_fast->is_active())
				value *= fast_modifier;
			
			auto& three_dof = ctx.entity_registry->get<entity::component::constraint::three_dof>(three_dof_eid);
			const math::quaternion<float> yaw = math::angle_axis(three_dof.yaw, {0.0f, 1.0f, 0.0f});
			
			const float3 movement = {-truck_speed * value * (1.0f / 60.0f), 0.0f, 0.0f};
			entity::command::translate(*ctx.entity_registry, target_eid, yaw * movement);
		}
	);
	
	// Pedestal up
	ctx.controls["move_up"]->set_active_callback
	(
		[&ctx = this->ctx, target_eid, pedestal_speed, move_slow, move_fast, slow_modifier, fast_modifier](float value)
		{
			if (move_slow->is_active())
				value *= slow_modifier;
			if (move_fast->is_active())
				value *= fast_modifier;
				
			const float3 movement = {0.0f, pedestal_speed * value * (1.0f / 60.0f), 0.0f};
			entity::command::translate(*ctx.entity_registry, target_eid, movement);
		}
	);
	
	// Pedestal down
	ctx.controls["move_down"]->set_active_callback
	(
		[&ctx = this->ctx, target_eid, pedestal_speed, move_slow, move_fast, slow_modifier, fast_modifier](float value)
		{
			if (move_slow->is_active())
				value *= slow_modifier;
			if (move_fast->is_active())
				value *= fast_modifier;
				
			const float3 movement = {0.0f, -pedestal_speed * value * (1.0f / 60.0f), 0.0f};
			entity::command::translate(*ctx.entity_registry, target_eid, movement);
		}
	);
	
	// Mouse rotate
	ctx.controls["mouse_look"]->set_activated_callback
	(
		[&ctx = this->ctx, mouse_look_toggle]()
		{
			if (mouse_look_toggle)
				ctx.mouse_look = !ctx.mouse_look;
			else
				ctx.mouse_look = true;
			
			ctx.app->set_relative_mouse_mode(ctx.mouse_look);
		}
	);
	ctx.controls["mouse_look"]->set_deactivated_callback
	(
		[&ctx = this->ctx, mouse_look_toggle]()
		{
			if (!mouse_look_toggle)
			{
				ctx.mouse_look = false;
				ctx.app->set_relative_mouse_mode(false);
			}
		}
	);
	// Pan left
	ctx.controls["look_left_gamepad"]->set_active_callback
	(
		[&ctx = this->ctx, three_dof_eid, gamepad_pan_factor](float value)
		{
			auto& three_dof = ctx.entity_registry->get<entity::component::constraint::three_dof>(three_dof_eid);
			three_dof.yaw += gamepad_pan_factor * value * (1.0f / 60.0f);
		}
	);
	ctx.controls["look_left_mouse"]->set_active_callback
	(
		[&ctx = this->ctx, three_dof_eid, mouse_pan_factor](float value)
		{
			if (!ctx.mouse_look)
				return;
			
			auto& three_dof = ctx.entity_registry->get<entity::component::constraint::three_dof>(three_dof_eid);
			three_dof.yaw += mouse_pan_factor * value * (1.0f / 60.0f);
		}
	);
	
	// Pan right
	ctx.controls["look_right_gamepad"]->set_active_callback
	(
		[&ctx = this->ctx, three_dof_eid, gamepad_pan_factor](float value)
		{
			auto& three_dof = ctx.entity_registry->get<entity::component::constraint::three_dof>(three_dof_eid);
			three_dof.yaw -= gamepad_pan_factor * value * (1.0f / 60.0f);
		}
	);
	ctx.controls["look_right_mouse"]->set_active_callback
	(
		[&ctx = this->ctx, three_dof_eid, mouse_pan_factor](float value)
		{
			if (!ctx.mouse_look)
				return;
			
			auto& three_dof = ctx.entity_registry->get<entity::component::constraint::three_dof>(three_dof_eid);
			three_dof.yaw -= mouse_pan_factor * value * (1.0f / 60.0f);
		}
	);
	// Tilt up
	ctx.controls["look_up_gamepad"]->set_active_callback
	(
		[&ctx = this->ctx, three_dof_eid, gamepad_tilt_factor](float value)
		{
			auto& three_dof = ctx.entity_registry->get<entity::component::constraint::three_dof>(three_dof_eid);
			three_dof.pitch -= gamepad_tilt_factor * value * (1.0f / 60.0f);
			three_dof.pitch = std::max<float>(math::radians(-90.0f), three_dof.pitch);
		}
	);
	ctx.controls["look_up_mouse"]->set_active_callback
	(
		[&ctx = this->ctx, three_dof_eid, mouse_tilt_factor](float value)
		{
			if (!ctx.mouse_look)
				return;
			
			auto& three_dof = ctx.entity_registry->get<entity::component::constraint::three_dof>(three_dof_eid);
			three_dof.pitch -= mouse_tilt_factor * value * (1.0f / 60.0f);
			three_dof.pitch = std::max<float>(math::radians(-90.0f), three_dof.pitch);
		}
	);
	// Tilt down
	ctx.controls["look_down_gamepad"]->set_active_callback
	(
		[&ctx = this->ctx, three_dof_eid, gamepad_tilt_factor](float value)
		{
			auto& three_dof = ctx.entity_registry->get<entity::component::constraint::three_dof>(three_dof_eid);
			three_dof.pitch += gamepad_tilt_factor * value * (1.0f / 60.0f);
			three_dof.pitch = std::min<float>(math::radians(90.0f), three_dof.pitch);
		}
	);
	ctx.controls["look_down_mouse"]->set_active_callback
	(
		[&ctx = this->ctx, three_dof_eid, mouse_tilt_factor](float value)
		{
			if (!ctx.mouse_look)
				return;
			
			auto& three_dof = ctx.entity_registry->get<entity::component::constraint::three_dof>(three_dof_eid);
			three_dof.pitch += mouse_tilt_factor * value * (1.0f / 60.0f);
			three_dof.pitch = std::min<float>(math::radians(90.0f), three_dof.pitch);
		}
	);
	/*
	// Use tool
	ctx.controls["use_tool"]->set_activated_callback
	(
		[&ctx]()
		{
			if (ctx.entities.count("active_tool"))
			{
				entity::id tool_eid = ctx.entities["active_tool"];
				const auto& tool = ctx.entity_registry->get<entity::component::tool>(tool_eid);
				if (tool.activated)
					tool.activated();
			}
		}
	);
	ctx.controls["use_tool"]->set_deactivated_callback
	(
		[&ctx]()
		{
			if (ctx.entities.count("active_tool"))
			{
				entity::id tool_eid = ctx.entities["active_tool"];
				const auto& tool = ctx.entity_registry->get<entity::component::tool>(tool_eid);
				if (tool.deactivated)
					tool.deactivated();
			}
		}
	);
	ctx.controls["use_tool"]->set_active_callback
	(
		[&ctx](float value)
		{
			if (ctx.entities.count("active_tool"))
			{
				entity::id tool_eid = ctx.entities["active_tool"];
				const auto& tool = ctx.entity_registry->get<entity::component::tool>(tool_eid);
				if (tool.active)
					tool.active();
			}
		}
	);
	*/
	
	// Setup pause control
	ctx.controls["pause"]->set_activated_callback
	(
		[this, &ctx = this->ctx]()
		{
			// Disable controls
			this->disable_controls();
			
			// Set resume callback
			ctx.resume_callback = [this, &ctx]()
			{
				this->enable_controls();
				ctx.resume_callback = nullptr;
			};
			
			// Push pause menu state
			ctx.state_machine.emplace(new game::state::pause_menu(ctx));
		}
	);
}

void nuptial_flight::disable_controls()
{
	ctx.controls["move_forward"]->set_active_callback(nullptr);
	ctx.controls["move_back"]->set_active_callback(nullptr);
	ctx.controls["move_right"]->set_active_callback(nullptr);
	ctx.controls["move_left"]->set_active_callback(nullptr);
	ctx.controls["move_up"]->set_active_callback(nullptr);
	ctx.controls["move_down"]->set_active_callback(nullptr);
	ctx.controls["mouse_look"]->set_activated_callback(nullptr);
	ctx.controls["mouse_look"]->set_deactivated_callback(nullptr);
	ctx.controls["look_left_gamepad"]->set_active_callback(nullptr);
	ctx.controls["look_left_mouse"]->set_active_callback(nullptr);
	ctx.controls["look_right_gamepad"]->set_active_callback(nullptr);
	ctx.controls["look_right_mouse"]->set_active_callback(nullptr);
	ctx.controls["look_up_gamepad"]->set_active_callback(nullptr);
	ctx.controls["look_up_mouse"]->set_active_callback(nullptr);
	ctx.controls["look_down_gamepad"]->set_active_callback(nullptr);
	ctx.controls["look_down_mouse"]->set_active_callback(nullptr);
	ctx.controls["pause"]->set_activated_callback(nullptr);
}

} // namespace state
} // namespace game
