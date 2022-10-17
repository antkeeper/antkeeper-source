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
#include "game/state/nest-selection.hpp"
#include "game/ant/swarm.hpp"
#include "entity/archetype.hpp"
#include "game/system/camera.hpp"
#include "game/system/astronomy.hpp"
#include "game/system/atmosphere.hpp"
#include "game/system/collision.hpp"
#include "game/component/locomotion.hpp"
#include "game/component/transform.hpp"
#include "game/component/terrain.hpp"
#include "game/component/camera.hpp"
#include "game/component/model.hpp"
#include "game/component/constraint/constraint.hpp"
#include "game/component/constraint-stack.hpp"
#include "game/component/steering.hpp"
#include "game/component/picking.hpp"
#include "game/component/spring.hpp"
#include "math/projection.hpp"
#include "game/controls.hpp"
#include "entity/commands.hpp"
#include "animation/screen-transition.hpp"
#include "animation/ease.hpp"
#include "resources/resource-manager.hpp"
#include "game/world.hpp"
#include "application.hpp"
#include "render/passes/clear-pass.hpp"
#include "render/passes/ground-pass.hpp"
#include "state-machine.hpp"
#include "config.hpp"
#include "game/load.hpp"
#include "game/ant/breed.hpp"
#include "game/ant/morphogenesis.hpp"
#include "math/interpolation.hpp"
#include "physics/light/exposure.hpp"
#include "color/color.hpp"
#include "application.hpp"
#include "input/mouse.hpp"
#include <iostream>

using namespace game::ant;

namespace game {
namespace state {

nuptial_flight::nuptial_flight(game::context& ctx):
	game::state::base(ctx)
{
	ctx.logger->push_task("Entering nuptial flight state");
	
	// Init selected picking flag
	selected_picking_flag = std::uint32_t{1} << (sizeof(std::uint32_t) * 8 - 1);
	selected_eid = entt::null;
	
	// Disable UI color clear
	ctx.ui_clear_pass->set_cleared_buffers(false, true, false);
	
	// Create world if not yet created
	if (ctx.entities.find("earth") == ctx.entities.end())
	{
		// Create cosmos
		game::world::cosmogenesis(ctx);
		
		// Create observer
		game::world::create_observer(ctx);
	}
	
	// Load biome
	//game::load::biome(ctx, "desert-scrub.bio");
	
	// Set world time
	game::world::set_time(ctx, 2022, 6, 21, 12, 0, 0.0);
	
	// Set world time scale
	game::world::set_time_scale(ctx, 0.0);
	
	// Setup and enable sky and ground passes
	ctx.sky_pass->set_enabled(true);
	ctx.ground_pass->set_enabled(true);
	
	// Create mating swarm
	swarm_eid = game::ant::create_swarm(ctx);
	
	// Switch to surface camera
	ctx.underground_camera->set_active(false);
	ctx.surface_camera->set_active(true);
	
	// Set camera exposure
	const float ev100_sunny16 = physics::light::ev::from_settings(16.0f, 1.0f / 100.0f, 100.0f);
	ctx.surface_camera->set_exposure(ev100_sunny16);
	
	const auto& viewport_dimensions = ctx.app->get_viewport_dimensions();
	const float aspect_ratio = static_cast<float>(viewport_dimensions[0]) / static_cast<float>(viewport_dimensions[1]);
	
	// Init camera rig params
	camera_rig_near_distance = 1.0f;
	camera_rig_far_distance = 150.0f;
	camera_rig_near_fov = math::vertical_fov(math::radians(100.0f), aspect_ratio);
	camera_rig_far_fov = math::vertical_fov(math::radians(60.0f), aspect_ratio);
	camera_rig_zoom_speed = 4.0f;
	camera_rig_translation_spring_angular_frequency = period_to_rads(0.125f);
	camera_rig_rotation_spring_angular_frequency = period_to_rads(0.125f);
	camera_rig_fov_spring_angular_frequency = period_to_rads(0.125f);
	camera_rig_focus_ease_to_duration = 1.0f;
	
	// Read camera rig settingss
	if (ctx.config->contains("near_fov"))
		camera_rig_near_fov = math::vertical_fov(math::radians((*ctx.config)["near_fov"].get<float>()), aspect_ratio);
	if (ctx.config->contains("far_fov"))
		camera_rig_far_fov = math::vertical_fov(math::radians((*ctx.config)["far_fov"].get<float>()), aspect_ratio);
	
	// Create camera rig
	create_camera_rig();
	
	// Select random alate
	ctx.entity_registry->view<component::transform, component::steering>().each
	(
		[&](entity::id alate_eid, auto& transform, auto& steering)
		{
			select_entity(alate_eid);
		}
	);
	
	// Satisfy camera rig constraints
	satisfy_camera_rig_constraints();
	
	// Queue fade in
	ctx.fade_transition_color->set_value({1, 1, 1});
	ctx.function_queue.push(std::bind(&screen_transition::transition, ctx.fade_transition, config::nuptial_flight_fade_in_duration, true, ease<float>::out_sine, true, nullptr));
	
	// Queue control setup
	ctx.function_queue.push(std::bind(&nuptial_flight::enable_controls, this));
	
	ctx.logger->pop_task(EXIT_SUCCESS);
}

nuptial_flight::~nuptial_flight()
{
	ctx.logger->push_task("Exiting nuptial flight state");
	
	// Deselect selected entity
	select_entity(entt::null);
	
	destroy_camera_rig();
	game::ant::destroy_swarm(ctx, swarm_eid);
	
	ctx.logger->pop_task(EXIT_SUCCESS);
}

void nuptial_flight::create_camera_rig()
{
	// Construct camera rig focus ease to constraint
	component::constraint::ease_to camera_rig_focus_ease_to;
	camera_rig_focus_ease_to.target = selected_eid;
	camera_rig_focus_ease_to.start = {0, 0, 0};
	camera_rig_focus_ease_to.duration = camera_rig_focus_ease_to_duration;
	camera_rig_focus_ease_to.t = camera_rig_focus_ease_to.duration;
	camera_rig_focus_ease_to.function = &ease<float3, float>::out_expo;
	component::constraint_stack_node camera_rig_focus_ease_to_node;
	camera_rig_focus_ease_to_node.active = true;
	camera_rig_focus_ease_to_node.weight = 1.0f;
	camera_rig_focus_ease_to_node.next = entt::null;
	camera_rig_focus_ease_to_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<component::constraint::ease_to>(camera_rig_focus_ease_to_eid, camera_rig_focus_ease_to);
	ctx.entity_registry->emplace<component::constraint_stack_node>(camera_rig_focus_ease_to_eid, camera_rig_focus_ease_to_node);
	
	// Construct camera rig focus constraint stack
	component::constraint_stack camera_rig_focus_constraint_stack;
	camera_rig_focus_constraint_stack.priority = 1;
	camera_rig_focus_constraint_stack.head = camera_rig_focus_ease_to_eid;
	
	// Construct camera rig focus transform component
	component::transform camera_rig_focus_transform;
	camera_rig_focus_transform.local = math::transform<float>::identity;
	camera_rig_focus_transform.world = camera_rig_focus_transform.local;
	camera_rig_focus_transform.warp = true;
	
	// Construct camera rig focus entity
	camera_rig_focus_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<component::transform>(camera_rig_focus_eid, camera_rig_focus_transform);
	ctx.entity_registry->emplace<component::constraint_stack>(camera_rig_focus_eid, camera_rig_focus_constraint_stack);
	
	// Construct camera rig pivot constraint
	component::constraint::pivot camera_rig_pivot;
	camera_rig_pivot.target = camera_rig_focus_eid;
	camera_rig_pivot.offset = {0, 0, 0};
	component::constraint_stack_node camera_rig_pivot_node;
	camera_rig_pivot_node.active = true;
	camera_rig_pivot_node.weight = 1.0f;
	camera_rig_pivot_node.next = entt::null;
	camera_rig_pivot_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<component::constraint::pivot>(camera_rig_pivot_eid, camera_rig_pivot);
	ctx.entity_registry->emplace<component::constraint_stack_node>(camera_rig_pivot_eid, camera_rig_pivot_node);
	
	// Construct camera rig copy translation constraint
	component::constraint::copy_translation camera_rig_copy_translation;
	camera_rig_copy_translation.target = camera_rig_focus_eid;
	camera_rig_copy_translation.copy_x = true;
	camera_rig_copy_translation.copy_y = true;
	camera_rig_copy_translation.copy_z = true;
	camera_rig_copy_translation.invert_x = false;
	camera_rig_copy_translation.invert_y = false;
	camera_rig_copy_translation.invert_z = false;
	camera_rig_copy_translation.offset = true;
	component::constraint_stack_node camera_rig_copy_translation_node;
	camera_rig_copy_translation_node.active = true;
	camera_rig_copy_translation_node.weight = 1.0f;
	camera_rig_copy_translation_node.next = camera_rig_pivot_eid;
	camera_rig_copy_translation_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<component::constraint::copy_translation>(camera_rig_copy_translation_eid, camera_rig_copy_translation);
	ctx.entity_registry->emplace<component::constraint_stack_node>(camera_rig_copy_translation_eid, camera_rig_copy_translation_node);
	
	// Construct camera rig spring rotation constraint
	component::constraint::spring_rotation camera_rig_spring_rotation;
	camera_rig_spring_rotation.spring =
	{
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f},
		1.0f,
		camera_rig_rotation_spring_angular_frequency
	};
	component::constraint_stack_node camera_rig_spring_rotation_node;
	camera_rig_spring_rotation_node.active = true;
	camera_rig_spring_rotation_node.weight = 1.0f;
	camera_rig_spring_rotation_node.next = camera_rig_copy_translation_eid;
	camera_rig_spring_rotation_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<component::constraint::spring_rotation>(camera_rig_spring_rotation_eid, camera_rig_spring_rotation);
	ctx.entity_registry->emplace<component::constraint_stack_node>(camera_rig_spring_rotation_eid, camera_rig_spring_rotation_node);
	
	// Construct camera rig spring translation constraint
	component::constraint::spring_translation camera_rig_spring_translation;
	camera_rig_spring_translation.spring =
	{
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f},
		1.0f,
		camera_rig_translation_spring_angular_frequency
	};
	component::constraint_stack_node camera_rig_spring_translation_node;
	camera_rig_spring_translation_node.active = true;
	camera_rig_spring_translation_node.weight = 1.0f;
	camera_rig_spring_translation_node.next = camera_rig_spring_rotation_eid;
	camera_rig_spring_translation_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<component::constraint::spring_translation>(camera_rig_spring_translation_eid, camera_rig_spring_translation);
	ctx.entity_registry->emplace<component::constraint_stack_node>(camera_rig_spring_translation_eid, camera_rig_spring_translation_node);
	
	// Construct camera rig constraint stack
	component::constraint_stack camera_rig_constraint_stack;
	camera_rig_constraint_stack.priority = 2;
	camera_rig_constraint_stack.head = camera_rig_spring_translation_eid;
	
	// Construct camera rig transform component
	component::transform camera_rig_transform;
	camera_rig_transform.local = math::transform<float>::identity;
	camera_rig_transform.world = camera_rig_transform.local;
	camera_rig_transform.warp = true;
	
	// Construct camera rig camera component
	component::camera camera_rig_camera;
	camera_rig_camera.object = ctx.surface_camera;
	
	// Construct camera rig entity
	camera_rig_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<component::camera>(camera_rig_eid, camera_rig_camera);
	ctx.entity_registry->emplace<component::transform>(camera_rig_eid, camera_rig_transform);
	ctx.entity_registry->emplace<component::constraint_stack>(camera_rig_eid, camera_rig_constraint_stack);
	
	// Construct camera rig fov spring
	component::spring1 camera_rig_fov_spring;
	camera_rig_fov_spring.spring =
	{
		0.0f,
		0.0f,
		0.0f,
		1.0f,
		camera_rig_fov_spring_angular_frequency
	};
	camera_rig_fov_spring.callback = [&](float fov)
	{
		ctx.surface_camera->set_perspective(fov, ctx.surface_camera->get_aspect_ratio(), ctx.surface_camera->get_clip_near(), ctx.surface_camera->get_clip_far());
	};
	
	// Construct camera rig fov spring entity
	camera_rig_fov_spring_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<component::spring1>(camera_rig_fov_spring_eid, camera_rig_fov_spring);
	
	set_camera_rig_zoom(0.25f);
}

void nuptial_flight::destroy_camera_rig()
{
	ctx.entity_registry->destroy(camera_rig_eid);
	ctx.entity_registry->destroy(camera_rig_spring_translation_eid);
	ctx.entity_registry->destroy(camera_rig_spring_rotation_eid);
	ctx.entity_registry->destroy(camera_rig_copy_translation_eid);
	ctx.entity_registry->destroy(camera_rig_pivot_eid);
	
	ctx.entity_registry->destroy(camera_rig_focus_eid);
	ctx.entity_registry->destroy(camera_rig_focus_ease_to_eid);
	
	ctx.entity_registry->destroy(camera_rig_fov_spring_eid);
}

void nuptial_flight::set_camera_rig_zoom(float zoom)
{
	camera_rig_zoom = zoom;
	
	const float distance = math::log_lerp(camera_rig_far_distance, camera_rig_near_distance, camera_rig_zoom);
	ctx.entity_registry->patch<component::constraint::spring_translation>
	(
		camera_rig_spring_translation_eid,
		[&](auto& component)
		{
			component.spring.x1[2] = distance;
		}
	);
	
	const float fov = math::log_lerp(camera_rig_far_fov, camera_rig_near_fov, camera_rig_zoom);	
	ctx.entity_registry->patch<component::spring1>
	(
		camera_rig_fov_spring_eid,
		[&](auto& component)
		{
			component.spring.x1 = fov;
		}
	);
}

void nuptial_flight::satisfy_camera_rig_constraints()
{
	// Satisfy camera rig focus ease to constraint
	ctx.entity_registry->patch<component::constraint::ease_to>
	(
		camera_rig_focus_ease_to_eid,
		[&](auto& component)
		{
			component.t = component.duration;
		}
	);
	
	// Satisfy camera rig spring translation constraint
	ctx.entity_registry->patch<component::constraint::spring_translation>
	(
		camera_rig_spring_translation_eid,
		[&](auto& component)
		{
			component.spring.x0 = component.spring.x1;
			component.spring.v *= 0.0f;
		}
	);
	
	// Satisfy camera rig spring rotation constraint
	ctx.entity_registry->patch<component::constraint::spring_rotation>
	(
		camera_rig_spring_rotation_eid,
		[&](auto& component)
		{
			component.spring.x0 = component.spring.x1;
			component.spring.v *= 0.0f;
		}
	);
	
	// Satisfycamera rig fov spring
	ctx.entity_registry->patch<component::spring1>
	(
		camera_rig_fov_spring_eid,
		[&](auto& component)
		{
			component.spring.x0 = component.spring.x1;
			component.spring.v *= 0.0f;
		}
	);
}

void nuptial_flight::enable_controls()
{
	// Reset mouse look
	mouse_look = false;
	
	double time_scale = 0.0;
	double ff_time_scale = 60.0 * 200.0;
	
	// Init control settings
	float mouse_tilt_sensitivity = 1.0f;
	float mouse_pan_sensitivity = 1.0f;
	bool mouse_invert_tilt = false;
	bool mouse_invert_pan = false;
	bool mouse_look_toggle = false;
	float gamepad_tilt_sensitivity = 1.0f;
	float gamepad_pan_sensitivity = 1.0f;
	bool gamepad_invert_tilt = false;
	bool gamepad_invert_pan = false;
	
	// Read control settings
	if (ctx.config->contains("mouse_tilt_sensitivity"))
		mouse_tilt_sensitivity = math::radians((*ctx.config)["mouse_tilt_sensitivity"].get<float>());
	if (ctx.config->contains("mouse_pan_sensitivity"))
		mouse_pan_sensitivity = math::radians((*ctx.config)["mouse_pan_sensitivity"].get<float>());
	if (ctx.config->contains("mouse_invert_tilt"))
		mouse_invert_tilt = (*ctx.config)["mouse_invert_tilt"].get<bool>();
	if (ctx.config->contains("mouse_invert_pan"))
		mouse_invert_pan = (*ctx.config)["mouse_invert_pan"].get<bool>();
	if (ctx.config->contains("mouse_look_toggle"))
		mouse_look_toggle = (*ctx.config)["mouse_look_toggle"].get<bool>();
	if (ctx.config->contains("gamepad_tilt_sensitivity"))
		gamepad_tilt_sensitivity = math::radians((*ctx.config)["gamepad_tilt_sensitivity"].get<float>());
	if (ctx.config->contains("gamepad_pan_sensitivity"))
		gamepad_pan_sensitivity = math::radians((*ctx.config)["gamepad_pan_sensitivity"].get<float>());
	if (ctx.config->contains("gamepad_invert_tilt"))
		gamepad_invert_tilt = (*ctx.config)["gamepad_invert_tilt"].get<bool>();
	if (ctx.config->contains("gamepad_invert_pan"))
		gamepad_invert_pan = (*ctx.config)["gamepad_invert_pan"].get<bool>();
	
	// Determine tilt and pan factors according to sensitivity and inversion
	const float mouse_tilt_factor = mouse_tilt_sensitivity * (mouse_invert_tilt ? -1.0f : 1.0f);
	const float mouse_pan_factor = mouse_pan_sensitivity * (mouse_invert_pan ? -1.0f : 1.0f);
	const float gamepad_tilt_factor = gamepad_tilt_sensitivity * (gamepad_invert_tilt ? -1.0f : 1.0f);
	const float gamepad_pan_factor = gamepad_pan_sensitivity * (gamepad_invert_pan ? -1.0f : 1.0f);
	
	// Mouse look
	ctx.controls["mouse_look"]->set_activated_callback
	(
		[&, mouse_look_toggle]()
		{
			if (mouse_look_toggle)
				mouse_look = !mouse_look;
			else
				mouse_look = true;
			
			ctx.app->set_relative_mouse_mode(mouse_look);
		}
	);
	ctx.controls["mouse_look"]->set_deactivated_callback
	(
		[&, mouse_look_toggle]()
		{
			if (!mouse_look_toggle && mouse_look)
			{
				mouse_look = false;
				ctx.app->set_relative_mouse_mode(false);
			}
		}
	);
	
	// Arc left control
	ctx.controls["look_right_mouse"]->set_active_callback
	(
		[&, mouse_pan_factor](float value)
		{
			if (!mouse_look)
				return;
			
			ctx.entity_registry->patch<component::constraint::spring_rotation>
			(
				camera_rig_spring_rotation_eid,
				[&, mouse_pan_factor](auto& component)
				{
					component.spring.x1[0] -= mouse_pan_factor * value;
				}
			);
		}
	);
	ctx.controls["look_right_gamepad"]->set_active_callback
	(
		[&, gamepad_pan_factor](float value)
		{
			ctx.entity_registry->patch<component::constraint::spring_rotation>
			(
				camera_rig_spring_rotation_eid,
				[&, gamepad_pan_factor](auto& component)
				{
					component.spring.x1[0] -= gamepad_pan_factor * value * static_cast<float>(ctx.loop.get_update_period());
				}
			);
		}
	);
	
	// Arc right control
	ctx.controls["look_left_mouse"]->set_active_callback
	(
		[&, mouse_pan_factor](float value)
		{
			if (!mouse_look)
				return;
			
			ctx.entity_registry->patch<component::constraint::spring_rotation>
			(
				camera_rig_spring_rotation_eid,
				[&, mouse_pan_factor](auto& component)
				{
					component.spring.x1[0] += mouse_pan_factor * value;
				}
			);
		}
	);
	ctx.controls["look_left_gamepad"]->set_active_callback
	(
		[&, gamepad_pan_factor](float value)
		{
			ctx.entity_registry->patch<component::constraint::spring_rotation>
			(
				camera_rig_spring_rotation_eid,
				[&, gamepad_pan_factor](auto& component)
				{
					component.spring.x1[0] += gamepad_pan_factor * value * static_cast<float>(ctx.loop.get_update_period());
				}
			);
		}
	);
	
	// Arc down control
	ctx.controls["look_up_mouse"]->set_active_callback
	(
		[&, mouse_tilt_factor](float value)
		{
			if (!mouse_look)
				return;
			
			ctx.entity_registry->patch<component::constraint::spring_rotation>
			(
				camera_rig_spring_rotation_eid,
				[&, mouse_tilt_factor](auto& component)
				{
					component.spring.x1[1] -= mouse_tilt_factor * value;
					component.spring.x1[1] = std::max(-math::half_pi<float>, component.spring.x1[1]);
				}
			);
		}
	);
	ctx.controls["look_up_gamepad"]->set_active_callback
	(
		[&, gamepad_tilt_factor](float value)
		{
			ctx.entity_registry->patch<component::constraint::spring_rotation>
			(
				camera_rig_spring_rotation_eid,
				[&, gamepad_tilt_factor](auto& component)
				{
					component.spring.x1[1] -= gamepad_tilt_factor * value * static_cast<float>(ctx.loop.get_update_period());
					component.spring.x1[1] = std::max(-math::half_pi<float>, component.spring.x1[1]);
				}
			);
		}
	);
	
	// Arc up control
	ctx.controls["look_down_mouse"]->set_active_callback
	(
		[&, mouse_tilt_factor](float value)
		{
			if (!mouse_look)
				return;
			
			ctx.entity_registry->patch<component::constraint::spring_rotation>
			(
				camera_rig_spring_rotation_eid,
				[&, mouse_tilt_factor](auto& component)
				{
					component.spring.x1[1] += mouse_tilt_factor * value;
					component.spring.x1[1] = std::min(math::half_pi<float>, component.spring.x1[1]);
				}
			);
		}
	);
	ctx.controls["look_down_gamepad"]->set_active_callback
	(
		[&, gamepad_tilt_factor](float value)
		{
			ctx.entity_registry->patch<component::constraint::spring_rotation>
			(
				camera_rig_spring_rotation_eid,
				[&, gamepad_tilt_factor](auto& component)
				{
					component.spring.x1[1] += gamepad_tilt_factor * value * static_cast<float>(ctx.loop.get_update_period());
					component.spring.x1[1] = std::min(math::half_pi<float>, component.spring.x1[1]);
				}
			);
		}
	);
	
	// Dolly in control
	ctx.controls["move_up"]->set_active_callback
	(
		[&](float value)
		{
			set_camera_rig_zoom(std::min(1.0f, camera_rig_zoom + camera_rig_zoom_speed * static_cast<float>(ctx.loop.get_update_period())));
		}
	);
	
	// Dolly out control
	ctx.controls["move_down"]->set_active_callback
	(
		[&](float value)
		{
			set_camera_rig_zoom(std::max(0.0f, camera_rig_zoom - camera_rig_zoom_speed * static_cast<float>(ctx.loop.get_update_period())));
		}
	);
	
	// Mouse select control
	ctx.controls["select_mouse"]->set_activated_callback
	(
		[&]()
		{
			// Get window-space mouse coordinates
			auto [mouse_x, mouse_y] = ctx.app->get_mouse()->get_current_position();
			
			// Get window viewport dimensions
			const auto viewport_dimensions = ctx.app->get_viewport_dimensions();
			
			// Transform mouse coordinates from window space to NDC space
			const float2 mouse_ndc =
			{
				static_cast<float>(mouse_x) / static_cast<float>(viewport_dimensions[0] - 1) * 2.0f - 1.0f,
				(1.0f - static_cast<float>(mouse_y) / static_cast<float>(viewport_dimensions[1] - 1)) * 2.0f - 1.0f
			};
			
			// Get picking ray from camera
			const geom::ray<float> ray = ctx.surface_camera->pick(mouse_ndc);
			
			// Pick entity
			entity::id picked_eid = ctx.collision_system->pick_nearest(ray, ~selected_picking_flag);
			if (picked_eid != entt::null)
			{
				select_entity(picked_eid);
			}
		}
	);
	
	// Select forward control
	ctx.controls["move_forward"]->set_activated_callback
	(
		[&]()
		{
			select_nearest_entity({0.0f, 0.0f, -1.0f});
		}
	);
	
	// Select back control
	ctx.controls["move_back"]->set_activated_callback
	(
		[&]()
		{
			select_nearest_entity({0.0f, 0.0f, 1.0f});
		}
	);
	
	// Select right control
	ctx.controls["move_right"]->set_activated_callback
	(
		[&]()
		{
			select_nearest_entity({1.0f, 0.0f, 0.0f});
		}
	);
	
	// Select left control
	ctx.controls["move_left"]->set_activated_callback
	(
		[&]()
		{
			select_nearest_entity({-1.0f, 0.0f, 0.0f});
		}
	);
	
	// Action control
	ctx.controls["action"]->set_activated_callback
	(
		[&]()
		{
			// Disable controls
			disable_controls();
			
			// Change to nest selection state
			ctx.state_machine.pop();
			ctx.state_machine.emplace(new game::state::nest_selection(ctx));
		}
	);
	
	// Fast-forward
	ctx.controls["fast_forward"]->set_activated_callback
	(
		[&ctx = this->ctx, ff_time_scale]()
		{
			game::world::set_time_scale(ctx, ff_time_scale);
		}
	);
	ctx.controls["fast_forward"]->set_deactivated_callback
	(
		[&ctx = this->ctx, time_scale]()
		{
			game::world::set_time_scale(ctx, time_scale);
		}
	);
	ctx.controls["rewind"]->set_activated_callback
	(
		[&ctx = this->ctx, ff_time_scale]()
		{
			game::world::set_time_scale(ctx, -ff_time_scale);
		}
	);
	ctx.controls["rewind"]->set_deactivated_callback
	(
		[&ctx = this->ctx, time_scale]()
		{
			game::world::set_time_scale(ctx, time_scale);
		}
	);
	
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
	
	ctx.controls["increase_exposure"]->set_active_callback
	(
		[&ctx = this->ctx](float)
		{
			//ctx.astronomy_system->set_exposure_offset(ctx.astronomy_system->get_exposure_offset() - 1.0f);
			ctx.surface_camera->set_exposure(ctx.surface_camera->get_exposure() + 3.0f * static_cast<float>(ctx.loop.get_update_period()));
			ctx.logger->log("EV100: " + std::to_string(ctx.surface_camera->get_exposure()));
		}
	);
	
	ctx.controls["decrease_exposure"]->set_active_callback
	(
		[&ctx = this->ctx](float)
		{
			//ctx.astronomy_system->set_exposure_offset(ctx.astronomy_system->get_exposure_offset() + 1.0f);
			ctx.surface_camera->set_exposure(ctx.surface_camera->get_exposure() - 3.0f * static_cast<float>(ctx.loop.get_update_period()));
			ctx.logger->log("EV100: " + std::to_string(ctx.surface_camera->get_exposure()));
		}
	);
	
	const float wavelength_speed = 20.0;
	ctx.controls["dec_red"]->set_active_callback
	(
		[&ctx = this->ctx, wavelength_speed](float)
		{
			ctx.rgb_wavelengths.x() -= wavelength_speed * ctx.loop.get_update_period();
			ctx.atmosphere_system->set_rgb_wavelengths(ctx.rgb_wavelengths * 1e-9);
			std::stringstream stream;
			stream << ctx.rgb_wavelengths;
			ctx.logger->log("wavelengths: " + stream.str());
		}
	);
	ctx.controls["inc_red"]->set_active_callback
	(
		[&ctx = this->ctx, wavelength_speed](float)
		{
			ctx.rgb_wavelengths.x() += wavelength_speed * ctx.loop.get_update_period();
			ctx.atmosphere_system->set_rgb_wavelengths(ctx.rgb_wavelengths * 1e-9);
			std::stringstream stream;
			stream << ctx.rgb_wavelengths;
			ctx.logger->log("wavelengths: " + stream.str());
		}
	);
	
	ctx.controls["dec_green"]->set_active_callback
	(
		[&ctx = this->ctx, wavelength_speed](float)
		{
			ctx.rgb_wavelengths.y() -= wavelength_speed * ctx.loop.get_update_period();
			ctx.atmosphere_system->set_rgb_wavelengths(ctx.rgb_wavelengths * 1e-9);
			std::stringstream stream;
			stream << ctx.rgb_wavelengths;
			ctx.logger->log("wavelengths: " + stream.str());
		}
	);
	ctx.controls["inc_green"]->set_active_callback
	(
		[&ctx = this->ctx, wavelength_speed](float)
		{
			ctx.rgb_wavelengths.y() += wavelength_speed * ctx.loop.get_update_period();
			ctx.atmosphere_system->set_rgb_wavelengths(ctx.rgb_wavelengths * 1e-9);
			std::stringstream stream;
			stream << ctx.rgb_wavelengths;
			ctx.logger->log("wavelengths: " + stream.str());
		}
	);
	
	ctx.controls["dec_blue"]->set_active_callback
	(
		[&ctx = this->ctx, wavelength_speed](float)
		{
			ctx.rgb_wavelengths.z() -= wavelength_speed * ctx.loop.get_update_period();
			ctx.atmosphere_system->set_rgb_wavelengths(ctx.rgb_wavelengths * 1e-9);
			std::stringstream stream;
			stream << ctx.rgb_wavelengths;
			ctx.logger->log("wavelengths: " + stream.str());
		}
	);
	ctx.controls["inc_blue"]->set_active_callback
	(
		[&ctx = this->ctx, wavelength_speed](float)
		{
			ctx.rgb_wavelengths.z() += wavelength_speed * ctx.loop.get_update_period();
			ctx.atmosphere_system->set_rgb_wavelengths(ctx.rgb_wavelengths * 1e-9);
			std::stringstream stream;
			stream << ctx.rgb_wavelengths;
			ctx.logger->log("wavelengths: " + stream.str());
		}
	);
}

void nuptial_flight::disable_controls()
{
	if (mouse_look)
	{
		mouse_look = false;
		ctx.app->set_relative_mouse_mode(false);
	}
	
	ctx.controls["mouse_look"]->set_activated_callback(nullptr);
	ctx.controls["mouse_look"]->set_deactivated_callback(nullptr);
	ctx.controls["look_right_mouse"]->set_active_callback(nullptr);
	ctx.controls["look_right_gamepad"]->set_active_callback(nullptr);
	ctx.controls["look_left_mouse"]->set_active_callback(nullptr);
	ctx.controls["look_left_gamepad"]->set_active_callback(nullptr);
	ctx.controls["look_up_mouse"]->set_active_callback(nullptr);
	ctx.controls["look_up_gamepad"]->set_active_callback(nullptr);
	ctx.controls["look_down_mouse"]->set_active_callback(nullptr);
	ctx.controls["look_down_gamepad"]->set_active_callback(nullptr);
	ctx.controls["move_up"]->set_active_callback(nullptr);
	ctx.controls["move_down"]->set_active_callback(nullptr);
	ctx.controls["select_mouse"]->set_activated_callback(nullptr);
	ctx.controls["move_forward"]->set_activated_callback(nullptr);
	ctx.controls["move_back"]->set_activated_callback(nullptr);
	ctx.controls["move_right"]->set_activated_callback(nullptr);
	ctx.controls["move_left"]->set_activated_callback(nullptr);
	ctx.controls["action"]->set_activated_callback(nullptr);
	ctx.controls["fast_forward"]->set_activated_callback(nullptr);
	ctx.controls["fast_forward"]->set_deactivated_callback(nullptr);
	ctx.controls["rewind"]->set_activated_callback(nullptr);
	ctx.controls["rewind"]->set_deactivated_callback(nullptr);
	ctx.controls["pause"]->set_activated_callback(nullptr);
	ctx.controls["increase_exposure"]->set_active_callback(nullptr);
	ctx.controls["decrease_exposure"]->set_active_callback(nullptr);
	ctx.controls["dec_red"]->set_active_callback(nullptr);
	ctx.controls["inc_red"]->set_active_callback(nullptr);
	ctx.controls["dec_green"]->set_active_callback(nullptr);
	ctx.controls["inc_green"]->set_active_callback(nullptr);
	ctx.controls["dec_blue"]->set_active_callback(nullptr);
	ctx.controls["inc_blue"]->set_active_callback(nullptr);
}

void nuptial_flight::select_entity(entity::id entity_id)
{
	if (entity_id != selected_eid)
	{
		if (ctx.entity_registry->valid(selected_eid) && ctx.entity_registry->all_of<component::picking>(selected_eid))
		{
			// Unset selected bit on picking flags of previously selected entity
			ctx.entity_registry->patch<component::picking>
			(
				selected_eid,
				[&](auto& component)
				{
					component.flags &= ~selected_picking_flag;

				}
			);
		}
		
		selected_eid = entity_id;
		
		if (ctx.entity_registry->valid(selected_eid) && ctx.entity_registry->all_of<component::picking>(selected_eid))
		{
			// Set selected bit on picking flags of current selected entity
			ctx.entity_registry->patch<component::picking>
			(
				selected_eid,
				[&](auto& component)
				{
					component.flags |= selected_picking_flag;
				}
			);
		}
		
		// Update camera rig focus ease to target
		ctx.entity_registry->patch<component::constraint::ease_to>
		(
			camera_rig_focus_ease_to_eid,
			[&](auto& component)
			{
				component.target = selected_eid;
				component.t = 0.0f;
				
				const component::transform* transform = ctx.entity_registry->try_get<component::transform>(camera_rig_focus_eid);
				if (transform)
					component.start = transform->world.translation;
			}
		);
	}
}

void nuptial_flight::select_nearest_entity(const float3& direction)
{
	if (!ctx.entity_registry->valid(selected_eid))
		return;
	
	const component::transform* selected_eid_transform = ctx.entity_registry->try_get<component::transform>(selected_eid);
	if (!selected_eid_transform)
		return;
	
	// Construct picking plane
	const float3 picking_normal = math::normalize(ctx.surface_camera->get_rotation() * direction);
	const float3 picking_origin = selected_eid_transform->world.translation;
	
	// Pick entity
	entity::id picked_eid = ctx.collision_system->pick_nearest(picking_origin, picking_normal, ~selected_picking_flag);
	if (picked_eid != entt::null)
	{
		select_entity(picked_eid);
	}
}

} // namespace state
} // namespace game
