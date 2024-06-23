// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/states/nuptial-flight-state.hpp"
#include "game/states/pause-menu-state.hpp"
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
#include "game/screen-transition.hpp"
#include <engine/animation/ease.hpp>
#include <engine/resources/resource-manager.hpp>
#include "game/world.hpp"
#include "game/strings.hpp"
#include <engine/utility/state-machine.hpp>
#include <engine/config.hpp>
#include <engine/math/functions.hpp>
#include <engine/physics/light/exposure.hpp>
#include <engine/color/color.hpp>
#include <engine/input/mouse.hpp>
#include <engine/hash/fnv1a.hpp>
#include <random>

using namespace hash::literals;

nuptial_flight_state::nuptial_flight_state(::game& ctx):
	game_state(ctx)
{
	debug::log_trace("Entering nuptial flight state...");
	
	// Init selected picking flag
	selected_picking_flag = std::uint32_t{1} << (sizeof(std::uint32_t) * 8 - 1);
	selected_eid = entt::null;
	
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
	
	// Create mating swarm
	swarm_eid = create_ant_swarm(ctx);
	
	// Load name pool
	name_pool = ctx.resource_manager->load<text_file>("name-pool-en.txt");
	
	// Assign random ant names
	std::uniform_int_distribution<> name_pool_distribution(0, static_cast<int>(name_pool->lines.size() - 1));
	ctx.entity_registry->view<ant_caste_component>().each
	(
		[&](entity::id entity_id, [[maybe_unused]] const auto& caste)
		{
			ctx.entity_registry->emplace_or_replace<name_component>
			(
				entity_id,
				name_pool->lines[name_pool_distribution(ctx.rng)]
			);
		}
	);
	
	// Set camera exposure
	const float ev100_sunny16 = physics::light::ev::from_settings(16.0f, 1.0f / 100.0f, 100.0f);
	ctx.exterior_camera->set_exposure_value(ev100_sunny16);
	
	const auto& viewport_size = ctx.window->get_viewport_size();
	const float aspect_ratio = static_cast<float>(viewport_size[0]) / static_cast<float>(viewport_size[1]);
	
	// Init camera rig params
	camera_rig_near_distance = 1.0f;
	camera_rig_far_distance = 150.0f;
	camera_rig_near_fov = math::vertical_fov(math::radians(100.0f), aspect_ratio);
	camera_rig_far_fov = math::vertical_fov(math::radians(60.0f), aspect_ratio);
	camera_rig_zoom_speed = 4.0f;
	camera_rig_translation_spring_angular_frequency = physics::s_to_rads(0.125f);
	camera_rig_rotation_spring_angular_frequency = physics::s_to_rads(0.125f);
	camera_rig_fov_spring_angular_frequency = physics::s_to_rads(0.125f);
	camera_rig_focus_ease_to_duration = 1.0f;
	
	// Create camera rig
	create_camera_rig();
	
	// Construct selection text
	selection_text.set_material(ctx.menu_font_material);
	selection_text.set_color({1.0f, 1.0f, 1.0f, 1.0f});
	selection_text.set_font(ctx.menu_font);
	const auto& text_aabb = selection_text.get_bounds();
	float text_w = text_aabb.max.x() - text_aabb.min.x();
	// float text_h = text_aabb.max.y() - text_aabb.min.y();
	selection_text.set_translation({std::round(viewport_size.x() * 0.5f - text_w * 0.5f), std::round(ctx.menu_font->get_metrics().size), 0.0f});
	
	// Add text to UI
	ctx.ui_canvas->get_scene().add_object(selection_text);
	
	// Select random alate
	entity::id random_alate_eid;
	ctx.entity_registry->view<transform_component, steering_component>().each
	(
		[&](entity::id entity_id, [[maybe_unused]] auto& transform, [[maybe_unused]] auto& steering)
		{
			random_alate_eid = entity_id;
		}
	);
	select_entity(random_alate_eid);
	
	// Queue enable controls
	ctx.function_queue.push
	(
		[&ctx]()
		{
			::enable_game_controls(ctx);
		}
	);
	
	// Queue fade in
	fade_in_to(ctx, nullptr);
	
	// Refresh frame scheduler
	ctx.frame_scheduler.refresh();
	
	debug::log_trace("Entered nuptial flight state");
}

nuptial_flight_state::~nuptial_flight_state()
{
	debug::log_trace("Exiting nuptial flight state...");
	
	// Disable game controls
	::disable_game_controls(ctx);
	
	// Remove text from UI
	ctx.ui_canvas->get_scene().remove_object(selection_text);
	
	// Deselect selected entity
	select_entity(entt::null);
	
	
	destroy_camera_rig();
	destroy_ant_swarm(ctx, swarm_eid);
	
	debug::log_trace("Exited nuptial flight state");
}

void nuptial_flight_state::create_camera_rig()
{
	// Construct camera rig focus ease to constraint
	ease_to_constraint camera_rig_focus_ease_to;
	camera_rig_focus_ease_to.target = selected_eid;
	camera_rig_focus_ease_to.start = {0, 0, 0};
	camera_rig_focus_ease_to.duration = camera_rig_focus_ease_to_duration;
	camera_rig_focus_ease_to.t = camera_rig_focus_ease_to.duration;
	camera_rig_focus_ease_to.function = &ease<math::fvec3, float>::out_expo;
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
	camera_rig_focus_transform.local = math::identity<math::transform<float>>;
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
	camera_rig_spring_rotation.spring.set_angular_frequency(camera_rig_rotation_spring_angular_frequency);
	constraint_stack_node_component camera_rig_spring_rotation_node;
	camera_rig_spring_rotation_node.active = true;
	camera_rig_spring_rotation_node.weight = 1.0f;
	camera_rig_spring_rotation_node.next = camera_rig_copy_translation_eid;
	camera_rig_spring_rotation_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<spring_rotation_constraint>(camera_rig_spring_rotation_eid, camera_rig_spring_rotation);
	ctx.entity_registry->emplace<constraint_stack_node_component>(camera_rig_spring_rotation_eid, camera_rig_spring_rotation_node);
	
	// Construct camera rig spring translation constraint
	spring_translation_constraint camera_rig_spring_translation;
	camera_rig_spring_translation.spring.set_angular_frequency(camera_rig_translation_spring_angular_frequency);
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
	camera_rig_transform.local = math::identity<math::transform<float>>;
	camera_rig_transform.world = camera_rig_transform.local;
	
	// Construct camera rig camera component
	scene_component camera_rig_camera;
	camera_rig_camera.object = ctx.exterior_camera;
	
	// Construct camera rig entity
	camera_rig_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<scene_component>(camera_rig_eid, camera_rig_camera);
	ctx.entity_registry->emplace<transform_component>(camera_rig_eid, camera_rig_transform);
	ctx.entity_registry->emplace<constraint_stack_component>(camera_rig_eid, camera_rig_constraint_stack);
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
				switch (caste.type)
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
				switch (caste.type)
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
			// float text_h = text_aabb.max.y() - text_aabb.min.y();
			selection_text.set_translation({std::round(viewport_size.x() * 0.5f - text_w * 0.5f), std::round(ctx.menu_font->get_metrics().size), 0.0f});
		}
	}
}

void nuptial_flight_state::select_nearest_entity(const math::fvec3& direction)
{
	if (!ctx.entity_registry->valid(selected_eid))
		return;
	
	const transform_component* selected_eid_transform = ctx.entity_registry->try_get<transform_component>(selected_eid);
	if (!selected_eid_transform)
		return;
	
	// Construct picking plane
	const math::fvec3 picking_normal = math::normalize(ctx.exterior_camera->get_rotation() * direction);
	const math::fvec3 picking_origin = selected_eid_transform->world.translation;
	
	// Pick entity
	entity::id picked_eid = ctx.collision_system->pick_nearest(picking_origin, picking_normal, ~selected_picking_flag);
	if (picked_eid != entt::null)
	{
		select_entity(picked_eid);
	}
}
