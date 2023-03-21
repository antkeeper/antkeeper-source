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

#include "game/states/nuptial-flight-state.hpp"
#include "game/states/pause-menu-state.hpp"
#include "game/states/nest-selection-state.hpp"
#include "game/ant/ant-swarm.hpp"
#include <engine/entity/archetype.hpp>
#include "game/systems/camera-system.hpp"
#include "game/systems/astronomy-system.hpp"
#include "game/systems/atmosphere-system.hpp"
#include "game/systems/collision-system.hpp"
#include "game/components/ant-caste-component.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/terrain-component.hpp"
#include "game/components/name-component.hpp"
#include "game/components/constraint-stack-component.hpp"
#include "game/components/steering-component.hpp"
#include "game/components/picking-component.hpp"
#include "game/components/spring-component.hpp"
#include "game/components/scene-component.hpp"
#include "game/constraints/child-of-constraint.hpp"
#include "game/constraints/copy-rotation-constraint.hpp"
#include "game/constraints/copy-scale-constraint.hpp"
#include "game/constraints/copy-transform-constraint.hpp"
#include "game/constraints/copy-translation-constraint.hpp"
#include "game/constraints/ease-to-constraint.hpp"
#include "game/constraints/pivot-constraint.hpp"
#include "game/constraints/spring-rotation-constraint.hpp"
#include "game/constraints/spring-to-constraint.hpp"
#include "game/constraints/spring-translation-constraint.hpp"
#include "game/constraints/three-dof-constraint.hpp"
#include "game/constraints/track-to-constraint.hpp"
#include <engine/math/projection.hpp>
#include "game/controls.hpp"
#include "game/commands/commands.hpp"
#include <engine/animation/screen-transition.hpp>
#include <engine/animation/ease.hpp>
#include <engine/resources/resource-manager.hpp>
#include "game/world.hpp"
#include "game/strings.hpp"
#include <engine/render/passes/clear-pass.hpp>
#include <engine/render/passes/ground-pass.hpp>
#include <engine/utility/state-machine.hpp>
#include <engine/config.hpp>
#include <engine/math/interpolation.hpp>
#include <engine/physics/light/exposure.hpp>
#include <engine/color/color.hpp>
#include <engine/input/mouse.hpp>
#include <engine/utility/hash/fnv1a.hpp>
#include <random>

using namespace hash::literals;

nuptial_flight_state::nuptial_flight_state(::game& ctx):
	game_state(ctx)
{
	debug::log::trace("Entering nuptial flight state...");
	
	// Init selected picking flag
	selected_picking_flag = std::uint32_t{1} << (sizeof(std::uint32_t) * 8 - 1);
	selected_eid = entt::null;
	
	// Disable UI color clear
	ctx.ui_clear_pass->set_cleared_buffers(false, true, false);
	
	// Create world if not yet created
	if (ctx.entities.find("earth") == ctx.entities.end())
	{
		// Create cosmos
		::world::cosmogenesis(ctx);
		
		// Create observer
		::world::create_observer(ctx);
	}
	::world::enter_ecoregion(ctx, *ctx.resource_manager->load<::ecoregion>("seedy-scrub.eco"));
	
	// Set world time
	::world::set_time(ctx, 2022, 6, 21, 12, 0, 0.0);
	
	// Set world time scale
	::world::set_time_scale(ctx, 0.0);
	
	// Setup and enable sky and ground passes
	ctx.sky_pass->set_enabled(true);
	ctx.ground_pass->set_enabled(true);
	
	// Create mating swarm
	swarm_eid = create_ant_swarm(ctx);
	
	// Load name pools
	female_name_pool = ctx.resource_manager->load<text_file>("female-names-en.txt");
	male_name_pool = ctx.resource_manager->load<text_file>("male-names-en.txt");
	
	// Assign random ant names
	std::uniform_int_distribution<> female_name_pool_distribution(0, static_cast<int>(female_name_pool->lines.size() - 1));
	std::uniform_int_distribution<> male_name_pool_distribution(0, static_cast<int>(male_name_pool->lines.size() - 1));
	ctx.entity_registry->view<ant_caste_component>().each
	(
		[&](entity::id entity_id, const auto& caste)
		{
			if (caste.caste_type == ant_caste_type::male)
			{
				ctx.entity_registry->emplace_or_replace<name_component>
				(
					entity_id,
					male_name_pool->lines[male_name_pool_distribution(ctx.rng)]
				);
			}
			else
			{
				ctx.entity_registry->emplace_or_replace<name_component>
				(
					entity_id,
					female_name_pool->lines[female_name_pool_distribution(ctx.rng)]
				);
			}
		}
	);
	
	// Switch to surface camera
	ctx.underground_camera->set_active(false);
	ctx.surface_camera->set_active(true);
	
	// Set camera exposure
	const float ev100_sunny16 = physics::light::ev::from_settings(16.0f, 1.0f / 100.0f, 100.0f);
	ctx.surface_camera->set_exposure_value(ev100_sunny16);
	
	const auto& viewport_size = ctx.window->get_viewport_size();
	const float aspect_ratio = static_cast<float>(viewport_size[0]) / static_cast<float>(viewport_size[1]);
	
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
	
	// Create camera rig
	create_camera_rig();
	
	// Construct selection text
	selection_text.set_material(ctx.menu_font_material);
	selection_text.set_color({1.0f, 1.0f, 1.0f, 1.0f});
	selection_text.set_font(&ctx.menu_font);
	const auto& text_aabb = selection_text.get_bounds();
	float text_w = text_aabb.max.x() - text_aabb.min.x();
	float text_h = text_aabb.max.y() - text_aabb.min.y();
	selection_text.set_translation({std::round(viewport_size.x() * 0.5f - text_w * 0.5f), std::round(ctx.menu_font.get_font_metrics().size), 0.0f});
	
	// Add text to UI
	ctx.ui_scene->add_object(selection_text);
	
	// Select random alate
	entity::id random_alate_eid;
	ctx.entity_registry->view<transform_component, steering_component>().each
	(
		[&](entity::id entity_id, auto& transform, auto& steering)
		{
			random_alate_eid = entity_id;
		}
	);
	select_entity(random_alate_eid);
	
	// Satisfy camera rig constraints
	satisfy_camera_rig_constraints();
	
	// Setup controls
	setup_controls();
	
	// Queue enable controls
	ctx.function_queue.push
	(
		[&ctx]()
		{
			
			::enable_keeper_controls(ctx);
			::enable_game_controls(ctx);
		}
	);
	
	// Queue fade in
	ctx.fade_transition_color->set({0, 0, 0});
	ctx.function_queue.push(std::bind(&screen_transition::transition, ctx.fade_transition.get(), 1.0f, true, ease<float>::out_sine, true, nullptr));
	
	// Refresh frame scheduler
	ctx.frame_scheduler.refresh();
	
	debug::log::trace("Entered nuptial flight state");
}

nuptial_flight_state::~nuptial_flight_state()
{
	debug::log::trace("Exiting nuptial flight state...");
	
	// Disable game controls
	::disable_keeper_controls(ctx);
	::disable_game_controls(ctx);
	
	// Remove text from UI
	ctx.ui_scene->remove_object(selection_text);
	
	// Deselect selected entity
	select_entity(entt::null);
	
	
	destroy_camera_rig();
	destroy_ant_swarm(ctx, swarm_eid);
	
	debug::log::trace("Exited nuptial flight state");
}

void nuptial_flight_state::create_camera_rig()
{
	// Construct camera rig focus ease to constraint
	ease_to_constraint camera_rig_focus_ease_to;
	camera_rig_focus_ease_to.target = selected_eid;
	camera_rig_focus_ease_to.start = {0, 0, 0};
	camera_rig_focus_ease_to.duration = camera_rig_focus_ease_to_duration;
	camera_rig_focus_ease_to.t = camera_rig_focus_ease_to.duration;
	camera_rig_focus_ease_to.function = &ease<float3, float>::out_expo;
	constraint_stack_node_component camera_rig_focus_ease_to_node;
	camera_rig_focus_ease_to_node.active = true;
	camera_rig_focus_ease_to_node.weight = 1.0f;
	camera_rig_focus_ease_to_node.next = entt::null;
	camera_rig_focus_ease_to_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<ease_to_constraint>(camera_rig_focus_ease_to_eid, camera_rig_focus_ease_to);
	ctx.entity_registry->emplace<constraint_stack_node_component>(camera_rig_focus_ease_to_eid, camera_rig_focus_ease_to_node);
	
	// Construct camera rig focus constraint stack
	constraint_stack_component camera_rig_focus_constraint_stack;
	camera_rig_focus_constraint_stack.priority = 1;
	camera_rig_focus_constraint_stack.head = camera_rig_focus_ease_to_eid;
	
	// Construct camera rig focus transform component
	transform_component camera_rig_focus_transform;
	camera_rig_focus_transform.local = math::transform<float>::identity;
	camera_rig_focus_transform.world = camera_rig_focus_transform.local;
	
	// Construct camera rig focus entity
	camera_rig_focus_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<transform_component>(camera_rig_focus_eid, camera_rig_focus_transform);
	ctx.entity_registry->emplace<constraint_stack_component>(camera_rig_focus_eid, camera_rig_focus_constraint_stack);
	
	// Construct camera rig pivot constraint
	pivot_constraint camera_rig_pivot;
	camera_rig_pivot.target = camera_rig_focus_eid;
	camera_rig_pivot.offset = {0, 0, 0};
	constraint_stack_node_component camera_rig_pivot_node;
	camera_rig_pivot_node.active = true;
	camera_rig_pivot_node.weight = 1.0f;
	camera_rig_pivot_node.next = entt::null;
	camera_rig_pivot_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<pivot_constraint>(camera_rig_pivot_eid, camera_rig_pivot);
	ctx.entity_registry->emplace<constraint_stack_node_component>(camera_rig_pivot_eid, camera_rig_pivot_node);
	
	// Construct camera rig copy translation constraint
	copy_translation_constraint camera_rig_copy_translation;
	camera_rig_copy_translation.target = camera_rig_focus_eid;
	camera_rig_copy_translation.copy_x = true;
	camera_rig_copy_translation.copy_y = true;
	camera_rig_copy_translation.copy_z = true;
	camera_rig_copy_translation.invert_x = false;
	camera_rig_copy_translation.invert_y = false;
	camera_rig_copy_translation.invert_z = false;
	camera_rig_copy_translation.offset = true;
	constraint_stack_node_component camera_rig_copy_translation_node;
	camera_rig_copy_translation_node.active = true;
	camera_rig_copy_translation_node.weight = 1.0f;
	camera_rig_copy_translation_node.next = camera_rig_pivot_eid;
	camera_rig_copy_translation_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<copy_translation_constraint>(camera_rig_copy_translation_eid, camera_rig_copy_translation);
	ctx.entity_registry->emplace<constraint_stack_node_component>(camera_rig_copy_translation_eid, camera_rig_copy_translation_node);
	
	// Construct camera rig spring rotation constraint
	spring_rotation_constraint camera_rig_spring_rotation;
	camera_rig_spring_rotation.spring =
	{
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f},
		1.0f,
		camera_rig_rotation_spring_angular_frequency
	};
	constraint_stack_node_component camera_rig_spring_rotation_node;
	camera_rig_spring_rotation_node.active = true;
	camera_rig_spring_rotation_node.weight = 1.0f;
	camera_rig_spring_rotation_node.next = camera_rig_copy_translation_eid;
	camera_rig_spring_rotation_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<spring_rotation_constraint>(camera_rig_spring_rotation_eid, camera_rig_spring_rotation);
	ctx.entity_registry->emplace<constraint_stack_node_component>(camera_rig_spring_rotation_eid, camera_rig_spring_rotation_node);
	
	// Construct camera rig spring translation constraint
	spring_translation_constraint camera_rig_spring_translation;
	camera_rig_spring_translation.spring =
	{
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f},
		1.0f,
		camera_rig_translation_spring_angular_frequency
	};
	constraint_stack_node_component camera_rig_spring_translation_node;
	camera_rig_spring_translation_node.active = true;
	camera_rig_spring_translation_node.weight = 1.0f;
	camera_rig_spring_translation_node.next = camera_rig_spring_rotation_eid;
	camera_rig_spring_translation_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<spring_translation_constraint>(camera_rig_spring_translation_eid, camera_rig_spring_translation);
	ctx.entity_registry->emplace<constraint_stack_node_component>(camera_rig_spring_translation_eid, camera_rig_spring_translation_node);
	
	// Construct camera rig constraint stack
	constraint_stack_component camera_rig_constraint_stack;
	camera_rig_constraint_stack.priority = 2;
	camera_rig_constraint_stack.head = camera_rig_spring_translation_eid;
	
	// Construct camera rig transform component
	transform_component camera_rig_transform;
	camera_rig_transform.local = math::transform<float>::identity;
	camera_rig_transform.world = camera_rig_transform.local;
	
	// Construct camera rig camera component
	scene_component camera_rig_camera;
	camera_rig_camera.object = ctx.surface_camera;
	
	// Construct camera rig entity
	camera_rig_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<scene_component>(camera_rig_eid, camera_rig_camera);
	ctx.entity_registry->emplace<transform_component>(camera_rig_eid, camera_rig_transform);
	ctx.entity_registry->emplace<constraint_stack_component>(camera_rig_eid, camera_rig_constraint_stack);
	
	// Construct camera rig fov spring
	spring1_component camera_rig_fov_spring;
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
	ctx.entity_registry->emplace<spring1_component>(camera_rig_fov_spring_eid, camera_rig_fov_spring);
	
	set_camera_rig_zoom(0.25f);
}

void nuptial_flight_state::destroy_camera_rig()
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

void nuptial_flight_state::set_camera_rig_zoom(float zoom)
{
	camera_rig_zoom = zoom;
	
	const float distance = math::log_lerp(camera_rig_far_distance, camera_rig_near_distance, camera_rig_zoom);
	ctx.entity_registry->patch<spring_translation_constraint>
	(
		camera_rig_spring_translation_eid,
		[&](auto& component)
		{
			component.spring.x1[2] = distance;
		}
	);
	
	const float fov = math::log_lerp(camera_rig_far_fov, camera_rig_near_fov, camera_rig_zoom);	
	ctx.entity_registry->patch<spring1_component>
	(
		camera_rig_fov_spring_eid,
		[&](auto& component)
		{
			component.spring.x1 = fov;
		}
	);
}

void nuptial_flight_state::satisfy_camera_rig_constraints()
{
	// Satisfy camera rig focus ease to constraint
	ctx.entity_registry->patch<ease_to_constraint>
	(
		camera_rig_focus_ease_to_eid,
		[&](auto& component)
		{
			component.t = component.duration;
		}
	);
	
	// Satisfy camera rig spring translation constraint
	ctx.entity_registry->patch<spring_translation_constraint>
	(
		camera_rig_spring_translation_eid,
		[&](auto& component)
		{
			component.spring.x0 = component.spring.x1;
			component.spring.v *= 0.0f;
		}
	);
	
	// Satisfy camera rig spring rotation constraint
	ctx.entity_registry->patch<spring_rotation_constraint>
	(
		camera_rig_spring_rotation_eid,
		[&](auto& component)
		{
			component.spring.x0 = component.spring.x1;
			component.spring.v *= 0.0f;
		}
	);
	
	// Satisfycamera rig fov spring
	ctx.entity_registry->patch<spring1_component>
	(
		camera_rig_fov_spring_eid,
		[&](auto& component)
		{
			component.spring.x0 = component.spring.x1;
			component.spring.v *= 0.0f;
		}
	);
}

void nuptial_flight_state::setup_controls()
{
	// Enable/toggle mouse look
	action_subscriptions.emplace_back
	(
		ctx.mouse_look_action.get_activated_channel().subscribe
		(
			[&](const auto& event)
			{
				if (ctx.toggle_mouse_look)
				{
					mouse_look = !mouse_look;
				}
				else
				{
					mouse_look = true;
				}
				
				//ctx.input_manager->set_cursor_visible(!mouse_look);
				ctx.input_manager->set_relative_mouse_mode(mouse_look);
			}
		)
	);
	
	// Disable mouse look
	action_subscriptions.emplace_back
	(
		ctx.mouse_look_action.get_deactivated_channel().subscribe
		(
			[&](const auto& event)
			{
				if (!ctx.toggle_mouse_look && mouse_look)
				{
					mouse_look = false;
					//ctx.input_manager->set_cursor_visible(true);
					ctx.input_manager->set_relative_mouse_mode(false);
				}
			}
		)
	);
	
	// Mouse look
	mouse_motion_subscription = ctx.input_manager->get_event_dispatcher().subscribe<input::mouse_moved_event>
	(
		[&](const auto& event)
		{
			if (!mouse_look)
			{
				return;
			}
			
			ctx.entity_registry->patch<spring_rotation_constraint>
			(
				camera_rig_spring_rotation_eid,
				[&](auto& component)
				{
					component.spring.x1[0] += static_cast<float>(ctx.mouse_pan_factor) * static_cast<float>(event.difference.x());					
					component.spring.x1[1] -= static_cast<float>(ctx.mouse_tilt_factor) * static_cast<float>(event.difference.y());
					component.spring.x1[1] = std::min(math::half_pi<float>, std::max(-math::half_pi<float>, component.spring.x1[1]));
				}
			);
		}
	);
	
	// Dolly in control
	action_subscriptions.emplace_back
	(
		ctx.move_up_action.get_active_channel().subscribe
		(
			[&](const auto& event)
			{
				set_camera_rig_zoom(std::min(1.0f, camera_rig_zoom + camera_rig_zoom_speed * event.input_value * static_cast<float>(1.0 / ctx.fixed_update_rate)));
			}
		)
	);
	
	// Dolly out control
	action_subscriptions.emplace_back
	(
		ctx.move_down_action.get_active_channel().subscribe
		(
			[&](const auto& event)
			{
				set_camera_rig_zoom(std::max(0.0f, camera_rig_zoom - camera_rig_zoom_speed * event.input_value * static_cast<float>(1.0 / ctx.fixed_update_rate)));
			}
		)
	);
	
	// Pick alate
	action_subscriptions.emplace_back
	(
		ctx.mouse_pick_action.get_activated_channel().subscribe
		(
			[&](const auto& event)
			{
				// Get window-space mouse coordinates
				const auto& mouse_position = (*ctx.input_manager->get_mice().begin())->get_position();
				
				// Get window viewport size
				const auto& viewport_size = ctx.window->get_viewport_size();
				
				// Transform mouse coordinates from window space to NDC space
				const float2 mouse_ndc =
				{
					static_cast<float>(mouse_position.x()) / static_cast<float>(viewport_size.x() - 1) * 2.0f - 1.0f,
					(1.0f - static_cast<float>(mouse_position.y()) / static_cast<float>(viewport_size.y() - 1)) * 2.0f - 1.0f
				};
				
				// Get picking ray from camera
				const geom::ray<float, 3> ray = ctx.surface_camera->pick(mouse_ndc);
				
				// Pick entity
				entity::id picked_eid = ctx.collision_system->pick_nearest(ray, ~selected_picking_flag);
				if (picked_eid != entt::null)
				{
					select_entity(picked_eid);
				}
			}
		)
	);
}

void nuptial_flight_state::enable_controls()
{
	/*
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
			
			ctx.entity_registry->patch<spring_rotation_constraint>
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
			ctx.entity_registry->patch<spring_rotation_constraint>
			(
				camera_rig_spring_rotation_eid,
				[&, gamepad_pan_factor](auto& component)
				{
					component.spring.x1[0] -= gamepad_pan_factor * value * static_cast<float>(1.0 / ctx.fixed_update_rate);
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
			
			ctx.entity_registry->patch<spring_rotation_constraint>
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
			ctx.entity_registry->patch<spring_rotation_constraint>
			(
				camera_rig_spring_rotation_eid,
				[&, gamepad_pan_factor](auto& component)
				{
					component.spring.x1[0] += gamepad_pan_factor * value * static_cast<float>(1.0 / ctx.fixed_update_rate);
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
			
			ctx.entity_registry->patch<spring_rotation_constraint>
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
			ctx.entity_registry->patch<spring_rotation_constraint>
			(
				camera_rig_spring_rotation_eid,
				[&, gamepad_tilt_factor](auto& component)
				{
					component.spring.x1[1] -= gamepad_tilt_factor * value * static_cast<float>(1.0 / ctx.fixed_update_rate);
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
			
			ctx.entity_registry->patch<spring_rotation_constraint>
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
			ctx.entity_registry->patch<spring_rotation_constraint>
			(
				camera_rig_spring_rotation_eid,
				[&, gamepad_tilt_factor](auto& component)
				{
					component.spring.x1[1] += gamepad_tilt_factor * value * static_cast<float>(1.0 / ctx.fixed_update_rate);
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
			set_camera_rig_zoom(std::min(1.0f, camera_rig_zoom + camera_rig_zoom_speed * static_cast<float>(1.0 / ctx.fixed_update_rate)));
		}
	);
	
	// Dolly out control
	ctx.controls["move_down"]->set_active_callback
	(
		[&](float value)
		{
			set_camera_rig_zoom(std::max(0.0f, camera_rig_zoom - camera_rig_zoom_speed * static_cast<float>(1.0 / ctx.fixed_update_rate)));
		}
	);
	
	// Mouse select control
	ctx.controls["select_mouse"]->set_activated_callback
	(
		[&]()
		{
			// Get window-space mouse coordinates
			auto [mouse_x, mouse_y] = ctx.app->get_mouse()->get_current_position();
			
			// Get window viewport size
			const auto viewport_size = ctx.window->get_viewport_size();
			
			// Transform mouse coordinates from window space to NDC space
			const float2 mouse_ndc =
			{
				static_cast<float>(mouse_x) / static_cast<float>(viewport_size[0] - 1) * 2.0f - 1.0f,
				(1.0f - static_cast<float>(mouse_y) / static_cast<float>(viewport_size[1] - 1)) * 2.0f - 1.0f
			};
			
			// Get picking ray from camera
			const geom::ray<float, 3> ray = ctx.surface_camera->pick(mouse_ndc);
			
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
			ctx.state_machine.emplace(std::make_unique<nest_selection_state>(ctx));
		}
	);
	
	// Fast-forward
	ctx.controls["fast_forward"]->set_activated_callback
	(
		[&ctx = this->ctx, ff_time_scale]()
		{
			::world::set_time_scale(ctx, ff_time_scale);
		}
	);
	ctx.controls["fast_forward"]->set_deactivated_callback
	(
		[&ctx = this->ctx, time_scale]()
		{
			::world::set_time_scale(ctx, time_scale);
		}
	);
	ctx.controls["rewind"]->set_activated_callback
	(
		[&ctx = this->ctx, ff_time_scale]()
		{
			::world::set_time_scale(ctx, -ff_time_scale);
		}
	);
	ctx.controls["rewind"]->set_deactivated_callback
	(
		[&ctx = this->ctx, time_scale]()
		{
			::world::set_time_scale(ctx, time_scale);
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
			ctx.state_machine.emplace(std::make_unique<pause_menu_state>(ctx));
		}
	);
	
	ctx.controls["increase_exposure"]->set_active_callback
	(
		[&ctx = this->ctx](float)
		{
			//ctx.astronomy_system->set_exposure_offset(ctx.astronomy_system->get_exposure_offset() - 1.0f);
			ctx.surface_camera->set_exposure(ctx.surface_camera->get_exposure() + 3.0f * static_cast<float>(1.0 / ctx.fixed_update_rate));
			debug::log::info("EV100: " + std::to_string(ctx.surface_camera->get_exposure()));
		}
	);
	
	ctx.controls["decrease_exposure"]->set_active_callback
	(
		[&ctx = this->ctx](float)
		{
			//ctx.astronomy_system->set_exposure_offset(ctx.astronomy_system->get_exposure_offset() + 1.0f);
			ctx.surface_camera->set_exposure(ctx.surface_camera->get_exposure() - 3.0f * static_cast<float>(1.0 / ctx.fixed_update_rate));
			debug::log::info("EV100: " + std::to_string(ctx.surface_camera->get_exposure()));
		}
	);
	*/
}

void nuptial_flight_state::disable_controls()
{
	/*
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
	*/
}

void nuptial_flight_state::select_entity(entity::id entity_id)
{
	if (entity_id != selected_eid)
	{
		if (ctx.entity_registry->valid(selected_eid) && ctx.entity_registry->all_of<picking_component>(selected_eid))
		{
			// Unset selected bit on picking flags of previously selected entity
			ctx.entity_registry->patch<picking_component>
			(
				selected_eid,
				[&](auto& component)
				{
					component.flags &= ~selected_picking_flag;

				}
			);
		}
		
		selected_eid = entity_id;
		
		if (ctx.entity_registry->valid(selected_eid) && ctx.entity_registry->all_of<picking_component>(selected_eid))
		{
			// Set selected bit on picking flags of current selected entity
			ctx.entity_registry->patch<picking_component>
			(
				selected_eid,
				[&](auto& component)
				{
					component.flags |= selected_picking_flag;
				}
			);
		}
		
		// Update camera rig focus ease to target
		ctx.entity_registry->patch<ease_to_constraint>
		(
			camera_rig_focus_ease_to_eid,
			[&](auto& component)
			{
				component.target = selected_eid;
				component.t = 0.0f;
				
				const transform_component* transform = ctx.entity_registry->try_get<transform_component>(camera_rig_focus_eid);
				if (transform)
					component.start = transform->world.translation;
			}
		);
		
		// Update selection text
		if (ctx.entity_registry->valid(selected_eid) && ctx.entity_registry->all_of<::ant_caste_component>(selected_eid))
		{
			const auto& caste = ctx.entity_registry->get<::ant_caste_component>(selected_eid);
			
			if (ctx.entity_registry->all_of<::name_component>(selected_eid))
			{
				const auto& name = ctx.entity_registry->get<::name_component>(selected_eid).name;
				
				std::string format_string;
				switch (caste.caste_type)
				{
					case ::ant_caste_type::queen:
						format_string = ::get_string(ctx, "named_queen_label_format");
						break;
					
					case ::ant_caste_type::worker:
						format_string = ::get_string(ctx, "named_worker_label_format");
						break;
					
					case ::ant_caste_type::soldier:
						format_string = ::get_string(ctx, "named_soldier_label_format");
						break;
					
					case ::ant_caste_type::male:
						format_string = ::get_string(ctx, "named_male_label_format");
						break;
					
					default:
						//std::unreachable();
						break;
				}
				
				selection_text.set_content(std::vformat(format_string, std::make_format_args(name)));
			}
			else
			{
				switch (caste.caste_type)
				{
					case ::ant_caste_type::queen:
						selection_text.set_content(get_string(ctx, "queen_caste_name"));
						break;
					
					case ::ant_caste_type::worker:
						selection_text.set_content(get_string(ctx, "worker_caste_name"));
						break;
					
					case ::ant_caste_type::soldier:
						selection_text.set_content(get_string(ctx, "soldier_caste_name"));
						break;
					
					case ::ant_caste_type::male:
						selection_text.set_content(get_string(ctx, "male_caste_name"));
						break;
					
					default:
						//std::unreachable();
						break;
				}
			}
			
			const auto& viewport_size = ctx.window->get_viewport_size();
			const auto& text_aabb = selection_text.get_bounds();
			float text_w = text_aabb.max.x() - text_aabb.min.x();
			float text_h = text_aabb.max.y() - text_aabb.min.y();
			selection_text.set_translation({std::round(viewport_size.x() * 0.5f - text_w * 0.5f), std::round(ctx.menu_font.get_font_metrics().size), 0.0f});
		}
	}
}

void nuptial_flight_state::select_nearest_entity(const float3& direction)
{
	if (!ctx.entity_registry->valid(selected_eid))
		return;
	
	const transform_component* selected_eid_transform = ctx.entity_registry->try_get<transform_component>(selected_eid);
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
