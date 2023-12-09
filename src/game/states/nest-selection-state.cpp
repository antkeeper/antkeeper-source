// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/ant/ant-cladogenesis.hpp"
#include "game/ant/ant-genome.hpp"
#include "game/ant/ant-morphogenesis.hpp"
#include "game/ant/ant-phenome.hpp"
#include "game/commands/commands.hpp"
#include "game/components/constraint-stack-component.hpp"
#include "game/components/scene-component.hpp"
#include "game/components/picking-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include "game/components/rigid-body-constraint-component.hpp"
#include "game/components/steering-component.hpp"
#include "game/components/terrain-component.hpp"
#include "game/components/legged-locomotion-component.hpp"
#include "game/components/winged-locomotion-component.hpp"
#include "game/components/ik-component.hpp"
#include "game/components/transform-component.hpp"
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
#include "game/controls.hpp"
#include "game/spawn.hpp"
#include "game/states/nest-selection-state.hpp"
#include "game/states/pause-menu-state.hpp"
#include "game/systems/astronomy-system.hpp"
#include "game/systems/atmosphere-system.hpp"
#include "game/systems/camera-system.hpp"
#include "game/systems/collision-system.hpp"
#include "game/world.hpp"
#include <engine/animation/ease.hpp>
#include <engine/animation/screen-transition.hpp>
#include <engine/config.hpp>
#include <engine/entity/archetype.hpp>
#include <engine/input/mouse.hpp>
#include <engine/math/common.hpp>
#include <engine/math/projection.hpp>
#include <engine/physics/light/exposure.hpp>
#include <engine/physics/kinematics/constraints/spring-constraint.hpp>
#include <engine/physics/kinematics/colliders/sphere-collider.hpp>
#include <engine/physics/kinematics/colliders/plane-collider.hpp>
#include <engine/physics/kinematics/colliders/box-collider.hpp>
#include <engine/physics/kinematics/colliders/capsule-collider.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/utility/state-machine.hpp>
#include <engine/scene/static-mesh.hpp>
#include <engine/scene/skeletal-mesh.hpp>
#include <engine/animation/ik/constraints/swing-twist-ik-constraint.hpp>
#include <engine/animation/ik/constraints/euler-ik-constraint.hpp>

nest_selection_state::nest_selection_state(::game& ctx):
	game_state(ctx)
{
	debug::log_trace("Entering nest selection state...");	
	
	// Create world if not yet created
	if (ctx.entities.find("earth") == ctx.entities.end())
	{
		// Create cosmos
		::world::cosmogenesis(ctx);
		
		// Create observer
		::world::create_observer(ctx);
	}
	
	ctx.active_ecoregion = ctx.resource_manager->load<::ecoregion>("seedy-scrub.eco");
	::world::enter_ecoregion(ctx, *ctx.active_ecoregion);
	
	debug::log_trace("Generating genome...");
	std::unique_ptr<ant_genome> genome = ant_cladogenesis(ctx.active_ecoregion->gene_pools[0], ctx.rng);
	debug::log_trace("Generated genome");
	
	debug::log_trace("Building worker phenome...");
	ant_phenome worker_phenome = ant_phenome(*genome, ant_caste_type::queen);
	debug::log_trace("Built worker phenome...");
	
	debug::log_trace("Generating worker model...");
	worker_model = ant_morphogenesis(worker_phenome);
	debug::log_trace("Generated worker model");
	
	// Create floor plane
	
	auto floor_archetype = ctx.resource_manager->load<entity::archetype>("desert-scrub-plane.ent");
	auto floor_eid = floor_archetype->create(*ctx.entity_registry);
	
	ctx.entity_registry->patch<transform_component>
	(
		floor_eid,
		[](auto& component)
		{
			component.local.rotation = math::angle_axis(math::radians(3.0f), math::fvec3{1.0f, 0.0f, 0.0f});
		}
	);
	
	auto floor_body = std::make_unique<physics::rigid_body>();
	auto floor_collider = std::make_shared<physics::plane_collider>(math::fvec3{0.0f, 1.0f, 0.0f});
	floor_collider->set_layer_mask(0b11);
	floor_collider->set_material(std::make_shared<physics::collider_material>(1.0f, 0.5f, 1.0f));
	floor_body->set_mass(0.0f);
	floor_body->set_inertia(0.0f);
	floor_body->set_collider(std::move(floor_collider));
	ctx.entity_registry->emplace<rigid_body_component>(floor_eid, std::move(floor_body));
	
	// Create worker entity(s)
	

	auto worker_skeletal_mesh = std::make_unique<scene::skeletal_mesh>(worker_model);
	
	worker_ant_eid = ctx.entity_registry->create();
	transform_component worker_transform_component;
	worker_transform_component.local = math::identity<math::transform<float>>;
	worker_transform_component.local.translation = {0, 0.5f, -4};
	worker_transform_component.world = worker_transform_component.local;
	ctx.entity_registry->emplace<transform_component>(worker_ant_eid, worker_transform_component);
	ctx.entity_registry->emplace<scene_component>(worker_ant_eid, std::move(worker_skeletal_mesh), std::uint8_t{1});
	

	
	auto worker_collider = std::make_shared<physics::box_collider>(math::fvec3{-1.0f, -1.0f, -1.0f}, math::fvec3{1.0f, 1.0f, 1.0f});
	//auto worker_collider = std::make_shared<physics::sphere_collider>(1.0f);
	worker_collider->set_material(std::make_shared<physics::collider_material>(0.4f, 0.1f, 0.2f));
	
	auto worker_body = std::make_unique<physics::rigid_body>();
	worker_body->set_position(worker_transform_component.local.translation);
	worker_body->set_previous_position(worker_transform_component.local.translation);
	worker_body->set_mass(0.1f);
	worker_body->set_inertia(0.05f);
	worker_body->set_angular_damping(0.5f);
	worker_body->set_collider(std::move(worker_collider));
	//ctx.entity_registry->emplace<rigid_body_component>(worker_ant_eid, std::move(worker_body));
	
	
	auto cocoon_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<scene_component>(cocoon_eid, std::make_shared<scene::static_mesh>(worker_phenome.pupa->cocoon_model), std::uint8_t{1});
	
	
	larva_eid = ctx.entity_registry->create();
	
	auto larva_skeletal_mesh = std::make_shared<scene::skeletal_mesh>(worker_phenome.larva->model);
	//auto larva_skeletal_mesh = std::make_shared<scene::skeletal_mesh>(ctx.resource_manager->load<render::model>("snake.mdl"));
	const auto& larva_skeleton = larva_skeletal_mesh->get_model()->get_skeleton();
	
	auto larva_ik_rig = std::make_shared<ik_rig>(*larva_skeletal_mesh);
	
	auto no_twist_constraint = std::make_shared<swing_twist_ik_constraint>();
	no_twist_constraint->set_twist_limit(0.0f, 0.0f);
	
	//auto euler_constraint = std::make_shared<euler_ik_constraint>();
	//euler_constraint->set_limits({0.0f, -math::radians(90.0f), 0.0f}, {math::radians(90.0f), math::radians(90.0f), 0.0f});
	
	for (std::size_t i = 0; i < larva_skeleton.get_bone_count(); ++i)
	{
		larva_ik_rig->set_constraint(static_cast<bone_index_type>(i), no_twist_constraint);
	}
	
	const auto larva_ik_root_bone_index = *larva_skeleton.get_bone_index("abdomen3");
	const auto larva_ik_effector_bone_index = *larva_skeleton.get_bone_index("head");
	
	const auto& larva_head_absolute_transform = larva_skeletal_mesh->get_pose().get_absolute_transform(larva_ik_effector_bone_index);
	const auto& larva_head_relative_transform = larva_skeletal_mesh->get_pose().get_relative_transform(larva_ik_effector_bone_index);
	
	larva_ik_solver = std::make_shared<ccd_ik_solver>(*larva_ik_rig, larva_ik_root_bone_index, larva_ik_effector_bone_index);
	larva_ik_solver->set_effector_position(larva_head_relative_transform * math::fvec3{0.0f, 0.0f, -0.0f});
	larva_ik_solver->set_goal_center(larva_head_absolute_transform.translation + math::fvec3{0.2f, 0.2f, 0.5f});
	

	
	larva_ik_rig->add_solver(larva_ik_solver);
	
	//larva_skeletal_mesh->get_pose().set_relative_rotation(larva_ik_root_bone_index, math::angle_axis(math::radians(45.0f), math::fvec3{1.0f, 0.0f, 0.0f}));
	//larva_skeletal_mesh->get_pose().update();
	
	ctx.entity_registry->emplace<scene_component>(larva_eid, std::move(larva_skeletal_mesh), std::uint8_t{1});
	ctx.entity_registry->emplace<ik_component>(larva_eid, std::move(larva_ik_rig));
	
	
	// Set world time
	::world::set_time(ctx, 2022, 6, 21, 12, 0, 0.0);
	
	// Init time scale
	double time_scale = 60.0;
	
	// Set time scale
	::world::set_time_scale(ctx, time_scale);
	
	// Setup and enable sky and ground passes
	ctx.sky_pass->set_enabled(true);
	
	// Set camera exposure
	const float ev100_sunny16 = physics::light::ev::from_settings(16.0f, 1.0f / 100.0f, 100.0f);
	ctx.surface_camera->set_exposure_value(ev100_sunny16);
	
	const auto& viewport_size = ctx.window->get_viewport_size();
	const float aspect_ratio = static_cast<float>(viewport_size[0]) / static_cast<float>(viewport_size[1]);
	
	// Init first person camera rig parameters
	first_person_camera_rig_translation_spring_angular_frequency = physics::s_to_rads(0.125f);
	first_person_camera_rig_rotation_spring_angular_frequency = physics::s_to_rads(0.125f);
	first_person_camera_rig_fov_spring_angular_frequency = physics::s_to_rads(0.125f);
	first_person_camera_rig_min_elevation = 0.25f;
	first_person_camera_rig_max_elevation = 150.0f;
	first_person_camera_near_fov = math::vertical_fov(math::radians(100.0f), aspect_ratio);
	first_person_camera_far_fov = math::vertical_fov(math::radians(60.0f), aspect_ratio);
	first_person_camera_near_speed = 5.0f;
	first_person_camera_far_speed = 140.0f;
	first_person_camera_rig_pedestal_speed = 2.0f;
	first_person_camera_rig_pedestal = 0.0f;
	
	// Create first person camera rig
	create_first_person_camera_rig();
	
	// Satisfy first person camera rig constraints
	satisfy_first_person_camera_rig_constraints();
	
	// Setup controls
	setup_controls();
	
	auto color_checker_archetype = ctx.resource_manager->load<entity::archetype>("color-checker.ent");
	color_checker_archetype->create(*ctx.entity_registry);
	// auto ruler_archetype = ctx.resource_manager->load<entity::archetype>("ruler-10cm.ent");
	// ruler_archetype->create(*ctx.entity_registry);
	

	
	auto yucca_archetype = ctx.resource_manager->load<entity::archetype>("yucca-plant-l.ent");
	auto yucca_eid = yucca_archetype->create(*ctx.entity_registry);
	::command::warp_to(*ctx.entity_registry, yucca_eid, {0, 0, 70});
	
	yucca_archetype = ctx.resource_manager->load<entity::archetype>("yucca-plant-m.ent");
	yucca_eid = yucca_archetype->create(*ctx.entity_registry);
	::command::warp_to(*ctx.entity_registry, yucca_eid, {400, 0, 200});
	
	yucca_archetype = ctx.resource_manager->load<entity::archetype>("yucca-plant-s.ent");
	yucca_eid = yucca_archetype->create(*ctx.entity_registry);
	::command::warp_to(*ctx.entity_registry, yucca_eid, {-300, 0, -300});
	
	auto cactus_plant_archetype = ctx.resource_manager->load<entity::archetype>("barrel-cactus-plant-l.ent");
	auto cactus_plant_eid = cactus_plant_archetype->create(*ctx.entity_registry);
	::command::warp_to(*ctx.entity_registry, cactus_plant_eid, {-100, 0, -200});
	
	cactus_plant_archetype = ctx.resource_manager->load<entity::archetype>("barrel-cactus-plant-m.ent");
	cactus_plant_eid = cactus_plant_archetype->create(*ctx.entity_registry);
	::command::warp_to(*ctx.entity_registry, cactus_plant_eid, {100, 0, -70});
	
	cactus_plant_archetype = ctx.resource_manager->load<entity::archetype>("barrel-cactus-plant-s.ent");
	cactus_plant_eid = cactus_plant_archetype->create(*ctx.entity_registry);
	::command::warp_to(*ctx.entity_registry, cactus_plant_eid, {50, 0, 80});
	
	// auto cactus_seed_archetype = ctx.resource_manager->load<entity::archetype>("barrel-cactus-seed.ent");
	// auto cactus_seed_eid = cactus_seed_archetype->create(*ctx.entity_registry);
	// ::command::warp_to(*ctx.entity_registry, cactus_seed_eid, {0, 1, -5});
	
	
	// Create spring
	/*
	auto spring_eid = ctx.entity_registry->create();
	auto spring = std::make_unique<physics::spring_constraint>();
	spring->attach_a();
	spring->attach_b();
	spring->set_resting_length(10.0f);
	spring->set_stiffness(2.0f);
	spring->set_damping(1.0f);
	ctx.entity_registry->emplace<rigid_body_constraint_component>(spring_eid, std::move(spring));
	*/
	
	sky_probe = std::make_shared<scene::light_probe>();
	const std::uint32_t sky_probe_face_size = 128;
	const auto sky_probe_mip_levels = static_cast<std::uint32_t>(std::bit_width(sky_probe_face_size));
	sky_probe->set_luminance_texture
	(
		std::make_shared<gl::texture_cube>
		(
			std::make_shared<gl::image_view_cube>
			(
				std::make_shared<gl::image_cube>
				(
					gl::format::r16g16b16_sfloat,
					sky_probe_face_size,
					sky_probe_mip_levels
				),
				gl::format::undefined,
				0,
				sky_probe_mip_levels
			),
			std::make_shared<gl::sampler>
			(
				gl::sampler_filter::linear,
				gl::sampler_filter::linear,
				gl::sampler_mipmap_mode::linear,
				gl::sampler_address_mode::clamp_to_edge,
				gl::sampler_address_mode::clamp_to_edge
			)
		)
	);
	ctx.sky_pass->set_sky_probe(sky_probe);
	ctx.surface_scene->add_object(*sky_probe);
	
	// Create sphere
	auto sphere_eid = ctx.entity_registry->create();
	auto sphere_static_mesh = std::make_shared<scene::static_mesh>(ctx.resource_manager->load<render::model>("diffuse-spheres.mdl"));
	ctx.entity_registry->emplace<scene_component>(sphere_eid, std::move(sphere_static_mesh), std::uint8_t{1});
	ctx.entity_registry->patch<scene_component>
	(
		sphere_eid,
		[&](auto& component)
		{
			component.object->set_translation({0.0f, 10.0f, 0.0f});
		}
	);
	
	// Queue enable game controls
	ctx.function_queue.push
	(
		[&ctx]()
		{
			::enable_game_controls(ctx);
		}
	);
	
	// Queue fade in
	ctx.fade_transition_color->set({0, 0, 0});
	ctx.function_queue.push(std::bind(&screen_transition::transition, ctx.fade_transition.get(), 1.0f, true, ease<float>::out_sine, true, nullptr));
	
	// Refresh frame scheduler
	ctx.frame_scheduler.refresh();
	
	debug::log_trace("Entered nest selection state");
}

nest_selection_state::~nest_selection_state()
{
	debug::log_trace("Exiting nest selection state...");
	
	// Disable game controls
	::disable_game_controls(ctx);
	
	destroy_first_person_camera_rig();
	
	debug::log_trace("Exited nest selection state");
}

void nest_selection_state::create_first_person_camera_rig()
{
	// Construct first person camera rig transform component
	transform_component first_person_camera_rig_transform;
	first_person_camera_rig_transform.local = math::identity<math::transform<float>>;
	first_person_camera_rig_transform.local.translation = {0, 10, 0};
	first_person_camera_rig_transform.world = first_person_camera_rig_transform.local;
	
	// Construct first person camera rig locomotion component
	winged_locomotion_component first_person_camera_rig_locomotion;
	
	// Construct first person camera rig physics component
	auto first_person_camera_rig_body = std::make_unique<physics::rigid_body>();
	first_person_camera_rig_body->set_position({0, 10, 0});
	first_person_camera_rig_body->set_mass(1.0f);
	first_person_camera_rig_body->set_linear_damping(10.0f);
	first_person_camera_rig_body->set_angular_damping(0.5f);
	auto first_person_camera_rig_collider = std::make_shared<physics::sphere_collider>(1.0f);
	auto camera_collider_material = std::make_shared<physics::collider_material>(0.0f, 0.0f, 0.0f);
	camera_collider_material->set_restitution_combine_mode(physics::restitution_combine_mode::minimum);
	camera_collider_material->set_friction_combine_mode(physics::friction_combine_mode::minimum);
	
	first_person_camera_rig_collider->set_layer_mask(0b10);
	first_person_camera_rig_collider->set_material(std::move(camera_collider_material));
	first_person_camera_rig_body->set_collider(std::move(first_person_camera_rig_collider));
	
	// Construct first person camera rig scene component
	scene_component first_person_camera_rig_camera;
	first_person_camera_rig_camera.object = ctx.surface_camera;
	
	// Construct first person camera rig entity
	first_person_camera_rig_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<scene_component>(first_person_camera_rig_eid, first_person_camera_rig_camera);
	ctx.entity_registry->emplace<transform_component>(first_person_camera_rig_eid, first_person_camera_rig_transform);
	ctx.entity_registry->emplace<rigid_body_component>(first_person_camera_rig_eid, std::move(first_person_camera_rig_body));
	ctx.entity_registry->emplace<winged_locomotion_component>(first_person_camera_rig_eid, first_person_camera_rig_locomotion);
}

void nest_selection_state::destroy_first_person_camera_rig()
{
	ctx.entity_registry->destroy(first_person_camera_rig_eid);
}

void nest_selection_state::set_first_person_camera_rig_pedestal(float pedestal)
{

}

void nest_selection_state::move_first_person_camera_rig(const math::fvec2& direction, float factor)
{

}

void nest_selection_state::satisfy_first_person_camera_rig_constraints()
{

}

void nest_selection_state::setup_controls()
{
	
	constexpr float movement_speed = 200.0f;
	
	auto move_first_person_camera_rig = [&](const math::fvec2& direction, float speed)
	{
		const transform_component& first_person_camera_rig_transform = ctx.entity_registry->get<transform_component>(first_person_camera_rig_eid);
		
		const math::fquat yaw_rotation = math::angle_axis(static_cast<float>(first_person_camera_yaw), math::fvec3{0.0f, 1.0f, 0.0f});
		
		const math::fvec3 rotated_direction = yaw_rotation * math::fvec3{direction[0], 0.0f, direction[1]};
		
		const math::fvec3 force = rotated_direction * speed;
		
		
		moving = true;
		movement_direction = direction;
		this->movement_speed = speed;
		
		ctx.entity_registry->patch<winged_locomotion_component>
		(
			first_person_camera_rig_eid,
			[&](auto& component)
			{
				component.force = force;
			}
		);
	};
	
	auto stop_first_person_camera_rig = [&]()
	{
		moving = false;
		
		ctx.entity_registry->patch<winged_locomotion_component>
		(
			first_person_camera_rig_eid,
			[&](auto& component)
			{
				component.force = {0.0f, 0.0f, 0.0f};
			}
		);
	};
	
	// Mouse look
	mouse_motion_subscription = ctx.input_manager->get_event_dispatcher().subscribe<input::mouse_moved_event>
	(
		[&, move_first_person_camera_rig](const auto& event)
		{
			if (adjust_time)
			{
				const double sensitivity = 1.0 / static_cast<double>(ctx.window->get_viewport_size().x());
				const double t = ctx.astronomy_system->get_time();
				::world::set_time(ctx, t + static_cast<double>(event.difference.x()) * sensitivity);
				
				/*
				sky_probe->update_illuminance_matrices();
				
				const auto matrices = sky_probe->get_illuminance_matrices();
				for (std::size_t i = 0; i < 3; ++i)
				{
					const auto m = matrices[i];
					debug::log_warning("\nmat4({},{},{},{},\n{},{},{},{},\n{},{},{},{},\n{},{},{},{});", m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1], m[1][2], m[1][3], m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3]);
				}
				*/
			}
			
			if (adjust_exposure)
			{
				const float sensitivity = 8.0f / static_cast<float>(ctx.window->get_viewport_size().y());
				ctx.surface_camera->set_exposure_value(ctx.surface_camera->get_exposure_value() + static_cast<float>(event.difference.y()) * sensitivity);
			}
			
			if (!mouse_look)
			{
				return;
			}
			
			first_person_camera_yaw -= ctx.mouse_pan_factor * static_cast<double>(event.difference.x());
			first_person_camera_pitch += ctx.mouse_tilt_factor * static_cast<double>(event.difference.y());
			first_person_camera_pitch = std::min(math::half_pi<double>, std::max(-math::half_pi<double>, first_person_camera_pitch));
			
			const math::dquat yaw_rotation = math::angle_axis(first_person_camera_yaw, {0.0f, 1.0, 0.0});
			const math::dquat pitch_rotation = math::angle_axis(first_person_camera_pitch, {-1.0, 0.0, 0.0});
			const math::fquat first_person_camera_orientation = math::fquat(math::normalize(yaw_rotation * pitch_rotation));
			
			ctx.entity_registry->patch<scene_component>
			(
				first_person_camera_rig_eid,
				[&](auto& component)
				{
					component.object->set_rotation(first_person_camera_orientation);
				}
			);
			
			ctx.entity_registry->patch<rigid_body_component>
			(
				first_person_camera_rig_eid,
				[&](auto& component)
				{
					component.body->set_previous_orientation(first_person_camera_orientation);
					component.body->set_orientation(first_person_camera_orientation);
				}
			);
			
			ctx.entity_registry->patch<transform_component>
			(
				first_person_camera_rig_eid,
				[&](auto& component)
				{
					component.local.rotation = first_person_camera_orientation;
					component.world.rotation = first_person_camera_orientation;
				}
			);
			
			if (moving)
			{
				move_first_person_camera_rig(movement_direction, this->movement_speed);
			}
		}
	);
	
	// Move forward
	action_subscriptions.emplace_back
	(
		ctx.move_forward_action.get_active_channel().subscribe
		(
			[&, move_first_person_camera_rig](const auto& event)
			{
				move_first_person_camera_rig({0.0f, -1.0f}, movement_speed * event.input_value);
			}
		)
	);
	action_subscriptions.emplace_back
	(
		ctx.move_forward_action.get_deactivated_channel().subscribe
		(
			[&, stop_first_person_camera_rig](const auto& event)
			{
				stop_first_person_camera_rig();
			}
		)
	);
	
	// Move back
	action_subscriptions.emplace_back
	(
		ctx.move_back_action.get_active_channel().subscribe
		(
			[&, move_first_person_camera_rig](const auto& event)
			{
				move_first_person_camera_rig({0, 1}, movement_speed * event.input_value);
			}
		)
	);
	action_subscriptions.emplace_back
	(
		ctx.move_back_action.get_deactivated_channel().subscribe
		(
			[&, stop_first_person_camera_rig](const auto& event)
			{
				stop_first_person_camera_rig();
			}
		)
	);
	
	// Move left
	action_subscriptions.emplace_back
	(
		ctx.move_left_action.get_active_channel().subscribe
		(
			[&, move_first_person_camera_rig](const auto& event)
			{
				move_first_person_camera_rig({-1, 0}, movement_speed * event.input_value);
			}
		)
	);
	action_subscriptions.emplace_back
	(
		ctx.move_left_action.get_deactivated_channel().subscribe
		(
			[&, stop_first_person_camera_rig](const auto& event)
			{
				stop_first_person_camera_rig();
			}
		)
	);
	
	// Move right
	action_subscriptions.emplace_back
	(
		ctx.move_right_action.get_active_channel().subscribe
		(
			[&, move_first_person_camera_rig](const auto& event)
			{
				move_first_person_camera_rig({1, 0}, movement_speed * event.input_value);
			}
		)
	);
	action_subscriptions.emplace_back
	(
		ctx.move_right_action.get_deactivated_channel().subscribe
		(
			[&, stop_first_person_camera_rig](const auto& event)
			{
				stop_first_person_camera_rig();
			}
		)
	);
	
	// Move up
	action_subscriptions.emplace_back
	(
		ctx.move_up_action.get_activated_channel().subscribe
		(
			[&](const auto& event)
			{
				ctx.entity_registry->patch<rigid_body_component>
				(
					first_person_camera_rig_eid,
					[&](auto& component)
					{
						component.body->apply_central_impulse({0, 5.0f, 0});
					}
				);
			}
		)
	);
	
	// Move down
	action_subscriptions.emplace_back
	(
		ctx.move_down_action.get_activated_channel().subscribe
		(
			[&](const auto& event)
			{
				ctx.entity_registry->patch<rigid_body_component>
				(
					first_person_camera_rig_eid,
					[&](auto& component)
					{
						component.body->apply_central_impulse({0, -5.0f, 0});
					}
				);
			}
		)
	);
	
	/*
	// Focus
	action_subscriptions.emplace_back
	(
		ctx.focus_action.get_activated_channel().subscribe
		(
			[&](const auto& event)
			{
				auto projectile_eid = ctx.entity_registry->create();
				
				const auto& camera_transform = ctx.entity_registry->get<transform_component>(first_person_camera_rig_eid);
				
				scene_component projectile_scene;
				
				
				transform_component projectile_transform;
				projectile_transform.local = camera_transform.world;
				projectile_transform.world = projectile_transform.local;
				
				auto projectile_body = std::make_unique<physics::rigid_body>();
				projectile_body->set_position(camera_transform.world.translation);
				projectile_body->set_previous_position(camera_transform.world.translation);
				projectile_body->set_mass(0.1f);
				projectile_body->set_inertia(0.05f);
				projectile_body->set_angular_damping(0.5f);
				
				if (ctx.mouse_look_action.is_active())
				{
					auto projectile_collider = std::make_shared<physics::sphere_collider>(1.0f);
					//auto projectile_collider = std::make_shared<physics::box_collider>(math::fvec3{-1.0f, -1.0f, -1.0f}, math::fvec3{1.0f, 1.0f, 1.0f});
					projectile_collider->set_material(std::make_shared<physics::collider_material>(0.4f, 0.1f, 0.2f));
					projectile_body->set_collider(std::move(projectile_collider));
					
					projectile_scene.object = std::make_shared<scene::static_mesh>(ctx.resource_manager->load<render::model>("sphere.mdl"));
					//projectile_scene.object = std::make_shared<scene::static_mesh>(ctx.resource_manager->load<render::model>("cube.mdl"));
				}
				else
				{
					auto projectile_collider = std::make_shared<physics::capsule_collider>(geom::capsule<float>{{{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}}, 0.6f});
					projectile_collider->set_material(std::make_shared<physics::collider_material>(0.4f, 0.1f, 0.2f));
					projectile_body->set_collider(std::move(projectile_collider));
					
					
					//projectile_scene.object = std::make_shared<scene::static_mesh>(ctx.resource_manager->load<render::model>("capsule.mdl"));
					
					auto projectile_mesh = std::make_shared<scene::skeletal_mesh>(worker_model);
					projectile_mesh->get_pose() = *worker_model->get_skeleton().get_pose("pupa");
					projectile_scene.object = projectile_mesh;
				}
				
				projectile_body->apply_central_impulse(camera_transform.world.rotation * math::fvec3{0.0f, 0.0f, -10.0f});
				
				
				// auto spring_eid = ctx.entity_registry->create();
				// auto spring = std::make_unique<physics::spring_constraint>();
				// spring->attach_a(*ctx.entity_registry->get<rigid_body_component>(first_person_camera_rig_eid).body, {0.0f, 0.0f, 0.0f});
				// spring->attach_b(*projectile_body, {0.0f, 0.0f, 0.0f});
				// spring->set_resting_length(10.0f);
				// spring->set_stiffness(2.0f);
				// spring->set_damping(1.0f);
				// ctx.entity_registry->emplace<rigid_body_constraint_component>(spring_eid, std::move(spring));
				
				ctx.entity_registry->emplace<transform_component>(projectile_eid, projectile_transform);
				ctx.entity_registry->emplace<scene_component>(projectile_eid, std::move(projectile_scene));
				ctx.entity_registry->emplace<rigid_body_component>(projectile_eid, std::move(projectile_body));
				
				
				// body.linear_momentum = {};
				// body.angular_momentum = {};
				// body.linear_velocity = {};
				// body.angular_velocity = {};
				
				//body.apply_central_impulse({0.0f, 100.5f, 0.0f});
				
				// ctx.entity_registry->patch<constraint_stack_node_component>
				// (
					// first_person_camera_rig_track_to_eid,
					// [&](auto& component)
					// {
						// component.active = true;
					// }
				// );
			}
		)
	);
	action_subscriptions.emplace_back
	(
		ctx.focus_action.get_deactivated_channel().subscribe
		(
			[&](const auto& event)
			{
				
			}
		)
	);
	*/
}

void nest_selection_state::enable_controls()
{

}

void nest_selection_state::disable_controls()
{
}

