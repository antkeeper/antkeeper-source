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

#include "game/state/nest-selection.hpp"
#include "game/state/pause-menu.hpp"
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
#include "math/interpolation.hpp"
#include "physics/light/exposure.hpp"
#include "application.hpp"
#include "input/mouse.hpp"
#include "math/projection.hpp"
#include <iostream>

#include "game/ant/morphogenesis.hpp"
#include "game/ant/phenome.hpp"
#include "game/ant/genome.hpp"
#include "game/ant/cladogenesis.hpp"

using namespace game::ant;

namespace game {
namespace state {

nest_selection::nest_selection(game::context& ctx):
	game::state::base(ctx)
{
	ctx.logger->push_task("Entering nest selection state");
	
	
	ctx.logger->push_task("Generating genome");
	
	std::random_device rng;
	ant::genome* genome = ant::cladogenesis(ctx.active_ecoregion->gene_pools[0], rng);
	
	// genome.antennae = ctx.resource_manager->load<ant::gene::antennae>("pogonomyrmex-antennae.dna");
	// genome.eyes = ctx.resource_manager->load<ant::gene::eyes>("pogonomyrmex-eyes.dna");
	// genome.gaster = ctx.resource_manager->load<ant::gene::gaster>("pogonomyrmex-gaster.dna");
	// genome.head = ctx.resource_manager->load<ant::gene::head>("pogonomyrmex-head.dna");
	// genome.legs = ctx.resource_manager->load<ant::gene::legs>("pogonomyrmex-legs.dna");
	// genome.mandibles = ctx.resource_manager->load<ant::gene::mandibles>("pogonomyrmex-mandibles.dna");
	// genome.mesosoma = ctx.resource_manager->load<ant::gene::mesosoma>("pogonomyrmex-mesosoma.dna");
	// genome.ocelli = ctx.resource_manager->load<ant::gene::ocelli>("ocelli-absent.dna");
	// genome.pigmentation = ctx.resource_manager->load<ant::gene::pigmentation>("rust-pigmentation.dna");
	// genome.sculpturing = ctx.resource_manager->load<ant::gene::sculpturing>("politus-sculpturing.dna");
	// genome.sting = ctx.resource_manager->load<ant::gene::sting>("pogonomyrmex-sting.dna");
	// genome.waist = ctx.resource_manager->load<ant::gene::waist>("pogonomyrmex-waist.dna");
	// genome.wings = ctx.resource_manager->load<ant::gene::wings>("wings-absent.dna");
	ctx.logger->pop_task(EXIT_SUCCESS);
	
	ctx.logger->push_task("Building worker phenome");
	ant::phenome worker_phenome = ant::phenome(*genome, ant::caste::worker);
	ctx.logger->pop_task(EXIT_SUCCESS);
	
	ctx.logger->push_task("Generating worker model");
	render::model* worker_model = ant::morphogenesis(worker_phenome);
	ctx.logger->pop_task(EXIT_SUCCESS);
	
	// Create worker entity(s)
	entity::id worker_eid = ctx.entity_registry->create();
	component::transform worker_transform_component;
	worker_transform_component.local = math::transform<float>::identity;
	worker_transform_component.local.translation = {0, 0, -20};
	worker_transform_component.world = worker_transform_component.local;
	worker_transform_component.warp = true;
	ctx.entity_registry->emplace<component::transform>(worker_eid, worker_transform_component);
	
	component::model worker_model_component;
	worker_model_component.render_model = worker_model;
	worker_model_component.instance_count = 0;
	worker_model_component.layers = ~0;
	ctx.entity_registry->emplace<component::model>(worker_eid, worker_model_component);
	
	
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
	
	// Init time scale
	double time_scale = 1.0;
	
	// Read time scale settings
	if (ctx.config->contains("time_scale"))
		time_scale = (*ctx.config)["time_scale"].get<double>();
	
	// Set time scale
	game::world::set_time_scale(ctx, time_scale);
	
	// Setup and enable sky and ground passes
	ctx.sky_pass->set_enabled(true);
	ctx.ground_pass->set_enabled(true);
	
	// Switch to surface camera
	ctx.underground_camera->set_active(false);
	ctx.surface_camera->set_active(true);
	
	// Set camera exposure
	const float ev100_sunny16 = physics::light::ev::from_settings(16.0f, 1.0f / 100.0f, 100.0f);
	ctx.surface_camera->set_exposure(ev100_sunny16);
	
	const auto& viewport_dimensions = ctx.app->get_viewport_dimensions();
	const float aspect_ratio = static_cast<float>(viewport_dimensions[0]) / static_cast<float>(viewport_dimensions[1]);
	
	// Init first person camera rig parameters
	first_person_camera_rig_translation_spring_angular_frequency = period_to_rads(0.125f);
	first_person_camera_rig_rotation_spring_angular_frequency = period_to_rads(0.125f);
	first_person_camera_rig_fov_spring_angular_frequency = period_to_rads(0.125f);
	first_person_camera_rig_min_elevation = 0.25f;
	first_person_camera_rig_max_elevation = 150.0f;
	first_person_camera_near_fov = math::vertical_fov(math::radians(100.0f), aspect_ratio);
	first_person_camera_far_fov = math::vertical_fov(math::radians(60.0f), aspect_ratio);
	first_person_camera_near_speed = 5.0f;
	first_person_camera_far_speed = 90.0f;
	first_person_camera_rig_pedestal_speed = 2.0f;
	first_person_camera_rig_pedestal = 0.0f;
	
	// Read first person camera rig settings
	if (ctx.config->contains("standing_eye_height"))
		first_person_camera_rig_max_elevation = (*ctx.config)["standing_eye_height"].get<float>();
	if (ctx.config->contains("walking_speed"))
		first_person_camera_far_speed = (*ctx.config)["walking_speed"].get<float>();
	if (ctx.config->contains("near_fov"))
		first_person_camera_near_fov = math::vertical_fov(math::radians((*ctx.config)["near_fov"].get<float>()), aspect_ratio);
	if (ctx.config->contains("far_fov"))
		first_person_camera_far_fov = math::vertical_fov(math::radians((*ctx.config)["far_fov"].get<float>()), aspect_ratio);
	
	// Create first person camera rig
	create_first_person_camera_rig();
	
	// Satisfy first person camera rig constraints
	satisfy_first_person_camera_rig_constraints();
	
	auto color_checker_archetype = ctx.resource_manager->load<entity::archetype>("color-checker.ent");
	color_checker_archetype->create(*ctx.entity_registry);
	
	// auto ruler_archetype = ctx.resource_manager->load<entity::archetype>("ruler-10cm.ent");
	// ruler_archetype->create(*ctx.entity_registry);
	
	// Queue control setup
	ctx.function_queue.push(std::bind(&nest_selection::enable_controls, this));
	
	ctx.logger->pop_task(EXIT_SUCCESS);
}

nest_selection::~nest_selection()
{
	ctx.logger->push_task("Exiting nest selection state");
	
	destroy_first_person_camera_rig();
	
	ctx.logger->pop_task(EXIT_SUCCESS);
}

void nest_selection::create_first_person_camera_rig()
{	
	// Construct first person camera rig spring rotation constraint
	component::constraint::spring_rotation first_person_camera_rig_spring_rotation;
	first_person_camera_rig_spring_rotation.spring =
	{
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f},
		1.0f,
		first_person_camera_rig_rotation_spring_angular_frequency
	};
	component::constraint_stack_node first_person_camera_rig_spring_rotation_node;
	first_person_camera_rig_spring_rotation_node.active = true;
	first_person_camera_rig_spring_rotation_node.weight = 1.0f;
	first_person_camera_rig_spring_rotation_node.next = entt::null;
	first_person_camera_rig_spring_rotation_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<component::constraint::spring_rotation>(first_person_camera_rig_spring_rotation_eid, first_person_camera_rig_spring_rotation);
	ctx.entity_registry->emplace<component::constraint_stack_node>(first_person_camera_rig_spring_rotation_eid, first_person_camera_rig_spring_rotation_node);
	
	// Construct first person camera rig spring translation constraint
	component::constraint::spring_translation first_person_camera_rig_spring_translation;
	first_person_camera_rig_spring_translation.spring =
	{
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f},
		1.0f,
		first_person_camera_rig_translation_spring_angular_frequency
	};
	component::constraint_stack_node first_person_camera_rig_spring_translation_node;
	first_person_camera_rig_spring_translation_node.active = true;
	first_person_camera_rig_spring_translation_node.weight = 1.0f;
	first_person_camera_rig_spring_translation_node.next = first_person_camera_rig_spring_rotation_eid;
	first_person_camera_rig_spring_translation_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<component::constraint::spring_translation>(first_person_camera_rig_spring_translation_eid, first_person_camera_rig_spring_translation);
	ctx.entity_registry->emplace<component::constraint_stack_node>(first_person_camera_rig_spring_translation_eid, first_person_camera_rig_spring_translation_node);
	
	// Construct first person camera rig constraint stack
	component::constraint_stack first_person_camera_rig_constraint_stack;
	first_person_camera_rig_constraint_stack.priority = 2;
	first_person_camera_rig_constraint_stack.head = first_person_camera_rig_spring_translation_eid;
	
	// Construct first person camera rig transform component
	component::transform first_person_camera_rig_transform;
	first_person_camera_rig_transform.local = math::transform<float>::identity;
	first_person_camera_rig_transform.world = first_person_camera_rig_transform.local;
	first_person_camera_rig_transform.warp = true;
	
	// Construct first person camera rig camera component
	component::camera first_person_camera_rig_camera;
	first_person_camera_rig_camera.object = ctx.surface_camera;
	
	// Construct first person camera rig entity
	first_person_camera_rig_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<component::camera>(first_person_camera_rig_eid, first_person_camera_rig_camera);
	ctx.entity_registry->emplace<component::transform>(first_person_camera_rig_eid, first_person_camera_rig_transform);
	ctx.entity_registry->emplace<component::constraint_stack>(first_person_camera_rig_eid, first_person_camera_rig_constraint_stack);
	
	// Construct first person camera rig fov spring
	component::spring1 first_person_camera_rig_fov_spring;
	first_person_camera_rig_fov_spring.spring =
	{
		0.0f,
		0.0f,
		0.0f,
		1.0f,
		first_person_camera_rig_fov_spring_angular_frequency
	};
	first_person_camera_rig_fov_spring.callback = [&](float fov)
	{
		ctx.surface_camera->set_perspective(fov, ctx.surface_camera->get_aspect_ratio(), ctx.surface_camera->get_clip_near(), ctx.surface_camera->get_clip_far());
	};
	
	// Construct first person camera rig fov spring entity
	first_person_camera_rig_fov_spring_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<component::spring1>(first_person_camera_rig_fov_spring_eid, first_person_camera_rig_fov_spring);
	
	set_first_person_camera_rig_pedestal(first_person_camera_rig_pedestal);
}

void nest_selection::destroy_first_person_camera_rig()
{
	ctx.entity_registry->destroy(first_person_camera_rig_eid);
	ctx.entity_registry->destroy(first_person_camera_rig_spring_translation_eid);
	ctx.entity_registry->destroy(first_person_camera_rig_spring_rotation_eid);
	ctx.entity_registry->destroy(first_person_camera_rig_fov_spring_eid);
}

void nest_selection::set_first_person_camera_rig_pedestal(float pedestal)
{
	first_person_camera_rig_pedestal = pedestal;
	const float elevation = math::log_lerp(first_person_camera_rig_min_elevation, first_person_camera_rig_max_elevation, first_person_camera_rig_pedestal);
	const float fov = math::log_lerp(first_person_camera_near_fov, first_person_camera_far_fov, first_person_camera_rig_pedestal);
	
	ctx.entity_registry->patch<component::constraint::spring_translation>
	(
		first_person_camera_rig_spring_translation_eid,
		[&](auto& component)
		{
			component.spring.x1[1] = elevation;
		}
	);
	
	ctx.entity_registry->patch<component::spring1>
	(
		first_person_camera_rig_fov_spring_eid,
		[&](auto& component)
		{
			component.spring.x1 = fov;
		}
	);
}

void nest_selection::move_first_person_camera_rig(const float2& direction, float factor)
{
	const float speed = math::log_lerp(first_person_camera_near_speed, first_person_camera_far_speed, first_person_camera_rig_pedestal) * factor;
	
	const component::constraint::spring_rotation& first_person_camera_rig_spring_rotation = ctx.entity_registry->get<component::constraint::spring_rotation>(first_person_camera_rig_spring_rotation_eid);
	
	const math::quaternion<float> yaw_rotation = math::angle_axis(first_person_camera_rig_spring_rotation.spring.x0[0], float3{0.0f, 1.0f, 0.0f});
	const float3 rotated_direction = math::normalize(yaw_rotation * float3{direction[0], 0.0f, direction[1]});
	const float3 velocity = rotated_direction * speed;
	
	ctx.entity_registry->patch<component::constraint::spring_translation>
	(
		first_person_camera_rig_spring_translation_eid,
		[&](auto& component)
		{
			component.spring.x1 += velocity * static_cast<float>(ctx.loop.get_update_period());
		}
	);
}

void nest_selection::satisfy_first_person_camera_rig_constraints()
{
	// Satisfy first person camera rig spring translation constraint
	ctx.entity_registry->patch<component::constraint::spring_translation>
	(
		first_person_camera_rig_spring_translation_eid,
		[&](auto& component)
		{
			component.spring.x0 = component.spring.x1;
			component.spring.v *= 0.0f;
		}
	);
	
	// Satisfy first person camera rig spring rotation constraint
	ctx.entity_registry->patch<component::constraint::spring_rotation>
	(
		first_person_camera_rig_spring_rotation_eid,
		[&](auto& component)
		{
			component.spring.x0 = component.spring.x1;
			component.spring.v *= 0.0f;
		}
	);
	
	// Satisfy first person camera rig fov spring
	ctx.entity_registry->patch<component::spring1>
	(
		first_person_camera_rig_fov_spring_eid,
		[&](auto& component)
		{
			component.spring.x0 = component.spring.x1;
			component.spring.v *= 0.0f;
		}
	);
}

void nest_selection::enable_controls()
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
	
	// Mouse look control
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
	
	// Look right control
	ctx.controls["look_right_mouse"]->set_active_callback
	(
		[&, mouse_pan_factor](float value)
		{
			if (!mouse_look)
				return;
			
			ctx.entity_registry->patch<component::constraint::spring_rotation>
			(
				first_person_camera_rig_spring_rotation_eid,
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
				first_person_camera_rig_spring_rotation_eid,
				[&, gamepad_pan_factor](auto& component)
				{
					component.spring.x1[0] -= gamepad_pan_factor * value * static_cast<float>(ctx.loop.get_update_period());
				}
			);
		}
	);
	
	// Look left control
	ctx.controls["look_left_mouse"]->set_active_callback
	(
		[&, mouse_pan_factor](float value)
		{
			if (!mouse_look)
				return;
			
			ctx.entity_registry->patch<component::constraint::spring_rotation>
			(
				first_person_camera_rig_spring_rotation_eid,
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
				first_person_camera_rig_spring_rotation_eid,
				[&, gamepad_pan_factor](auto& component)
				{
					component.spring.x1[0] += gamepad_pan_factor * value * static_cast<float>(ctx.loop.get_update_period());
				}
			);
		}
	);
	
	// Look up control
	ctx.controls["look_up_mouse"]->set_active_callback
	(
		[&, mouse_tilt_factor](float value)
		{
			if (!mouse_look)
				return;
			
			ctx.entity_registry->patch<component::constraint::spring_rotation>
			(
				first_person_camera_rig_spring_rotation_eid,
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
				first_person_camera_rig_spring_rotation_eid,
				[&, gamepad_tilt_factor](auto& component)
				{
					component.spring.x1[1] -= gamepad_tilt_factor * value * static_cast<float>(ctx.loop.get_update_period());
					component.spring.x1[1] = std::max(-math::half_pi<float>, component.spring.x1[1]);
				}
			);
		}
	);
	
	// Look down control
	ctx.controls["look_down_mouse"]->set_active_callback
	(
		[&, mouse_tilt_factor](float value)
		{
			if (!mouse_look)
				return;
			
			ctx.entity_registry->patch<component::constraint::spring_rotation>
			(
				first_person_camera_rig_spring_rotation_eid,
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
				first_person_camera_rig_spring_rotation_eid,
				[&, gamepad_tilt_factor](auto& component)
				{
					component.spring.x1[1] += gamepad_tilt_factor * value * static_cast<float>(ctx.loop.get_update_period());
					component.spring.x1[1] = std::min(math::half_pi<float>, component.spring.x1[1]);
				}
			);
		}
	);
	
	// Pedestal up control
	ctx.controls["move_up"]->set_active_callback
	(
		[&](float value)
		{
			set_first_person_camera_rig_pedestal(std::min(1.0f, first_person_camera_rig_pedestal + first_person_camera_rig_pedestal_speed * static_cast<float>(ctx.loop.get_update_period())));
		}
	);
	
	// Pedestal down control
	ctx.controls["move_down"]->set_active_callback
	(
		[&](float value)
		{
			set_first_person_camera_rig_pedestal(std::max(0.0f, first_person_camera_rig_pedestal - first_person_camera_rig_pedestal_speed * static_cast<float>(ctx.loop.get_update_period())));
		}
	);
	
	// Mouse select control
	ctx.controls["select_mouse"]->set_activated_callback
	(
		[&]()
		{
			
		}
	);
	
	// Move forward control
	ctx.controls["move_forward"]->set_active_callback
	(
		[&](float value)
		{
			move_first_person_camera_rig({0, -1}, value);
		}
	);
	
	// Move back control
	ctx.controls["move_back"]->set_active_callback
	(
		[&](float value)
		{
			move_first_person_camera_rig({0, 1}, value);
		}
	);
	
	// Move right control
	ctx.controls["move_right"]->set_active_callback
	(
		[&](float value)
		{
			move_first_person_camera_rig({1, 0}, value);
		}
	);
	
	// Move left control
	ctx.controls["move_left"]->set_active_callback
	(
		[&](float value)
		{
			move_first_person_camera_rig({-1, 0}, value);
		}
	);
	
	// Action control
	ctx.controls["action"]->set_activated_callback
	(
		[&]()
		{
			
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
			ctx.surface_camera->set_exposure(ctx.surface_camera->get_exposure() + 0.5f * static_cast<float>(ctx.loop.get_update_period()));
			ctx.logger->log("EV100: " + std::to_string(ctx.surface_camera->get_exposure()));
		}
	);
	
	ctx.controls["decrease_exposure"]->set_active_callback
	(
		[&ctx = this->ctx](float)
		{
			//ctx.astronomy_system->set_exposure_offset(ctx.astronomy_system->get_exposure_offset() + 1.0f);
			ctx.surface_camera->set_exposure(ctx.surface_camera->get_exposure() - 0.5f * static_cast<float>(ctx.loop.get_update_period()));
			ctx.logger->log("EV100: " + std::to_string(ctx.surface_camera->get_exposure()));
		}
	);
}

void nest_selection::disable_controls()
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
	ctx.controls["move_forward"]->set_active_callback(nullptr);
	ctx.controls["move_back"]->set_active_callback(nullptr);
	ctx.controls["move_right"]->set_active_callback(nullptr);
	ctx.controls["move_left"]->set_active_callback(nullptr);
	ctx.controls["action"]->set_activated_callback(nullptr);
	ctx.controls["fast_forward"]->set_activated_callback(nullptr);
	ctx.controls["fast_forward"]->set_deactivated_callback(nullptr);
	ctx.controls["rewind"]->set_activated_callback(nullptr);
	ctx.controls["rewind"]->set_deactivated_callback(nullptr);
	ctx.controls["pause"]->set_activated_callback(nullptr);
	ctx.controls["increase_exposure"]->set_active_callback(nullptr);
	ctx.controls["decrease_exposure"]->set_active_callback(nullptr);
}

} // namespace state
} // namespace game
