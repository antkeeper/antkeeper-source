// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/states/experiments/test-state.hpp"

#include "game/ant/ant-cladogenesis.hpp"
#include "game/ant/ant-genome.hpp"
#include "game/ant/ant-morphogenesis.hpp"
#include "game/ant/ant-skeleton.hpp"
#include "game/ant/ant-phenome.hpp"
#include "game/commands/commands.hpp"
#include "game/components/pose-component.hpp"
#include "game/components/constraint-stack-component.hpp"
#include "game/components/scene-component.hpp"
#include "game/components/picking-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include "game/components/rigid-body-constraint-component.hpp"
#include "game/components/steering-component.hpp"
#include "game/components/terrain-component.hpp"
#include "game/components/navmesh-agent-component.hpp"
#include "game/components/legged-locomotion-component.hpp"
#include "game/components/winged-locomotion-component.hpp"
#include "game/components/ant-caste-component.hpp"
#include "game/components/ik-component.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/ovary-component.hpp"
#include "game/components/spring-arm-component.hpp"
#include "game/components/ant-genome-component.hpp"
#include "game/components/animation-component.hpp"
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
#include "game/screen-transition.hpp"
#include "game/states/pause-menu-state.hpp"
#include "game/systems/astronomy-system.hpp"
#include "game/systems/atmosphere-system.hpp"
#include "game/systems/camera-system.hpp"
#include "game/systems/collision-system.hpp"
#include "game/systems/physics-system.hpp"
#include "game/systems/terrain-system.hpp"
#include "game/world.hpp"
#include <engine/animation/ease.hpp>
#include <engine/config.hpp>
#include <engine/entity/archetype.hpp>
#include <engine/input/mouse.hpp>
#include <engine/math/functions.hpp>
#include <engine/math/projection.hpp>
#include <engine/physics/light/exposure.hpp>
#include <engine/physics/kinematics/constraints/spring-constraint.hpp>
#include <engine/physics/kinematics/colliders/sphere-collider.hpp>
#include <engine/physics/kinematics/colliders/plane-collider.hpp>
#include <engine/physics/kinematics/colliders/box-collider.hpp>
#include <engine/physics/kinematics/colliders/capsule-collider.hpp>
#include <engine/physics/kinematics/colliders/mesh-collider.hpp>
#include <engine/render/passes/material-pass.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/utility/state-machine.hpp>
#include <engine/scene/static-mesh.hpp>
#include <engine/scene/skeletal-mesh.hpp>
#include <engine/scene/rectangle-light.hpp>
#include <engine/scene/point-light.hpp>
#include <engine/geom/intersection.hpp>
#include <engine/animation/ease.hpp>
#include <engine/color/color.hpp>
#include <engine/geom/brep/brep-operations.hpp>
#include <engine/geom/coordinates.hpp>
#include <engine/ai/navmesh.hpp>
#include <engine/animation/ik/constraints/euler-ik-constraint.hpp>

test_state::test_state(::game& ctx):
	game_state(ctx)
{
	debug::log_trace("Entering test state...");

	ctx.active_scene = ctx.exterior_scene.get();
	
	ctx.active_ecoregion = ctx.resource_manager->load<::ecoregion>("debug.eco");
	::world::enter_ecoregion(ctx, *ctx.active_ecoregion);
	
	debug::log_trace("Generating genome...");
	std::shared_ptr<ant_genome> genome = ant_cladogenesis(ctx.active_ecoregion->gene_pools[0], ctx.rng);
	debug::log_trace("Generated genome");
	
	debug::log_trace("Building worker phenome...");
	worker_phenome = std::make_shared<ant_phenome>(*genome, ant_caste_type::worker);
	debug::log_trace("Built worker phenome...");
	
	debug::log_trace("Generating worker model...");
	std::shared_ptr<render::model> worker_model = ant_morphogenesis(*worker_phenome);
	debug::log_trace("Generated worker model");
	
	// Create nest exterior
	{
		scene_component nest_exterior_scene_component;
		nest_exterior_scene_component.object = std::make_shared<scene::static_mesh>(ctx.resource_manager->load<render::model>("sphere-nest-200mm-exterior.mdl"));
		nest_exterior_scene_component.layer_mask = 1;
		
		auto nest_exterior_mesh = ctx.resource_manager->load<geom::brep_mesh>("sphere-nest-200mm-exterior.msh");
		
		auto nest_exterior_rigid_body = std::make_unique<physics::rigid_body>();
		nest_exterior_rigid_body->set_mass(0.0f);
		nest_exterior_rigid_body->set_collider(std::make_shared<physics::mesh_collider>(std::move(nest_exterior_mesh)));
		nest_exterior_rigid_body->set_position({10, -20, -5});
		nest_exterior_rigid_body->set_orientation(math::angle_axis(math::radians(30.0f), math::fvec3{1, 0, 0}));
		nest_exterior_rigid_body->set_scale({0.5f, 1.0f, 0.75f});
		
		auto nest_exterior_eid = ctx.entity_registry->create();
		ctx.entity_registry->emplace<scene_component>(nest_exterior_eid, std::move(nest_exterior_scene_component));
		ctx.entity_registry->emplace<rigid_body_component>(nest_exterior_eid, std::move(nest_exterior_rigid_body));
	}
	
	// Create nest interior
	{
		scene_component nest_interior_scene_component;
		nest_interior_scene_component.object = std::make_shared<scene::static_mesh>(ctx.resource_manager->load<render::model>("soil-nest.mdl"));
		nest_interior_scene_component.object->set_layer_mask(0b10);
		nest_interior_scene_component.layer_mask = 1;

		auto nest_interior_mesh = ctx.resource_manager->load<geom::brep_mesh>("soil-nest.msh");

		auto nest_interior_rigid_body = std::make_unique<physics::rigid_body>();
		nest_interior_rigid_body->set_mass(0.0f);
		nest_interior_rigid_body->set_collider(std::make_shared<physics::mesh_collider>(std::move(nest_interior_mesh)));
		nest_interior_rigid_body->get_collider()->set_layer_mask(0b10);
		
		auto nest_interior_eid = ctx.entity_registry->create();
		ctx.entity_registry->emplace<scene_component>(nest_interior_eid, std::move(nest_interior_scene_component));
		ctx.entity_registry->emplace<rigid_body_component>(nest_interior_eid, std::move(nest_interior_rigid_body));
	}
	
	// Generate terrain
	{
		auto heightmap = ctx.resource_manager->load<gl::image_2d>("grid-heightmap.tga");
		auto subdivisions = math::uvec2{0, 0};
		// auto subdivisions = math::uvec2{3, 3};
		auto transform = math::identity<math::transform<float>>;
		transform.scale.x() = 100.0f;
		transform.scale.y() = 100.0f;
		transform.scale.z() = transform.scale.x();
		transform.translation.y() = -transform.scale.y() * 0.5f;
		auto material = ctx.resource_manager->load<render::material>("grid-terrain-cm-middle-gray.mtl");
		ctx.terrain_system->generate(heightmap, subdivisions, transform, material);
	}
	
	// Create worker
	auto worker_skeletal_mesh = std::make_unique<scene::skeletal_mesh>(worker_model);
	worker_skeletal_mesh->set_layer_mask(0b11);
	
	// Create worker IK rig
	const auto& worker_skeleton = worker_model->skeleton();
	worker_ik_rig = std::make_shared<ik_rig>(*worker_skeletal_mesh);
	auto mesocoxa_ik_constraint = std::make_shared<euler_ik_constraint>();
	mesocoxa_ik_constraint->set_min_angles({-math::pi<float>, -math::pi<float>, -math::pi<float>});
	mesocoxa_ik_constraint->set_max_angles({ math::pi<float>,  math::pi<float>,  math::pi<float>});
	worker_ik_rig->set_constraint(worker_skeleton->bones().at("mesocoxa_l").index(), std::move(mesocoxa_ik_constraint));
	
	// Pose worker
	worker_skeletal_mesh->get_pose() = worker_model->skeleton()->rest_pose();
	
	worker_eid = ctx.entity_registry->create();


	pose_component worker_pose_component;
	worker_pose_component.current_pose = worker_skeletal_mesh->get_skeleton()->rest_pose();
	worker_pose_component.previous_pose = worker_pose_component.current_pose;
	

	
	ant_caste_component worker_caste_component;
	worker_caste_component.type = ant_caste_type::worker;
	worker_caste_component.phenome = worker_phenome;
	
	rigid_body_component worker_rigid_body_component;
	worker_rigid_body_component.body = std::make_unique<physics::rigid_body>();
	worker_rigid_body_component.body->set_mass(0.0f);
	auto rigid_body_transform = worker_rigid_body_component.body->get_transform();
	rigid_body_transform.scale = math::one<math::fvec3> * worker_phenome->body_size->mean_mesosoma_length;
	worker_rigid_body_component.body->set_transform(rigid_body_transform);
	
	legged_locomotion_component worker_locomotion_component;
	worker_locomotion_component.midstance_pose = generate_ant_midstance_pose(*worker_model->skeleton());
	worker_locomotion_component.midswing_pose = generate_ant_midswing_pose(*worker_model->skeleton());
	worker_locomotion_component.liftoff_pose = generate_ant_liftoff_pose(*worker_model->skeleton());
	worker_locomotion_component.touchdown_pose = generate_ant_touchdown_pose(*worker_model->skeleton());
	worker_locomotion_component.body_bone = worker_skeleton->bones().at("mesosoma").index();
	worker_locomotion_component.tip_bones =
	{
		worker_skeleton->bones().at("protarsomere1_l").index(),
		worker_skeleton->bones().at("mesotarsomere1_l").index(),
		worker_skeleton->bones().at("metatarsomere1_l").index(),
		worker_skeleton->bones().at("protarsomere1_r").index(),
		worker_skeleton->bones().at("mesotarsomere1_r").index(),
		worker_skeleton->bones().at("metatarsomere1_r").index(),
	};
	worker_locomotion_component.leg_bone_count = 4;
	worker_locomotion_component.gait = std::make_shared<::gait>();
	worker_locomotion_component.gait->frequency = 4.0f;
	worker_locomotion_component.gait->steps.resize(6);
	for (std::size_t i = 0; i < 6; ++i)
	{
		const float duty_factors[3] = {0.52f, 0.62f, 0.54f};
		auto& step = worker_locomotion_component.gait->steps[i];
		step.duty_factor = duty_factors[i % 3];
		step.delay = (i % 2) ? 0.5f : 0.0f;
	}
	worker_locomotion_component.standing_height = worker_phenome->legs->standing_height;
	worker_locomotion_component.stride_length = worker_phenome->legs->stride_length * worker_rigid_body_component.body->get_transform().scale.x();
	worker_locomotion_component.max_angular_frequency = worker_phenome->legs->max_angular_frequency;
	
	navmesh_agent_component worker_navmesh_agent_component;
	
	ovary_component worker_ovary_component;
	worker_ovary_component.egg_capacity = 4;
	worker_ovary_component.egg_production_duration = 1.0f;
	worker_ovary_component.oviposition_duration = 3.0f;
	worker_ovary_component.ovipositor_bone = worker_skeleton->bones().at("gaster").index();
	worker_ovary_component.oviposition_path = {{0.0f, -0.141708f, -0.799793f}, {0.0f, -0.187388f, -1.02008f}};
	
	ctx.entity_registry->emplace<scene_component>(worker_eid, std::move(worker_skeletal_mesh), std::uint8_t{1});
	ctx.entity_registry->emplace<navmesh_agent_component>(worker_eid, std::move(worker_navmesh_agent_component));
	ctx.entity_registry->emplace<pose_component>(worker_eid, std::move(worker_pose_component));
	ctx.entity_registry->emplace<legged_locomotion_component>(worker_eid, std::move(worker_locomotion_component));
	ctx.entity_registry->emplace<ant_caste_component>(worker_eid, std::move(worker_caste_component));
	ctx.entity_registry->emplace<rigid_body_component>(worker_eid, std::move(worker_rigid_body_component));
	ctx.entity_registry->emplace<ovary_component>(worker_eid, std::move(worker_ovary_component));
	ctx.entity_registry->emplace<ant_genome_component>(worker_eid, genome);
	
	// Set ant as controlled ant
	ctx.controlled_ant_eid = worker_eid;
	
	// Create color checker
	auto color_checker_eid = ctx.entity_registry->create();
	scene_component color_checker_scene_component;
	color_checker_scene_component.object = std::make_shared<scene::static_mesh>(ctx.resource_manager->load<render::model>("color-checker.mdl"));
	color_checker_scene_component.object->set_translation({0, 0, 4});
	color_checker_scene_component.layer_mask = 1;
	ctx.entity_registry->emplace<scene_component>(color_checker_eid, std::move(color_checker_scene_component));
	
	// Set world time
	::world::set_time(ctx, 2022, 6, 21, 12, 0, 0.0);
	
	// Init time scale
	double time_scale = 0.0;
	
	// Set time scale
	::world::set_time_scale(ctx, time_scale);
	
	// Setup and enable sky and ground passes
	ctx.sky_pass->set_enabled(true);
	
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
	ctx.exterior_scene->add_object(*sky_probe);
	
	// Set camera exposure
	const float ev100_sunny16 = physics::light::ev::from_settings(16.0f, 1.0f / 100.0f, 100.0f);
	ctx.exterior_camera->set_exposure_value(ev100_sunny16);
	
	// Create third person camera rig
	create_third_person_camera_rig();
	
	// Setup controls
	setup_controls();
	
	// Queue enable game controls
	ctx.function_queue.push
	(
		[&ctx]()
		{
			::enable_game_controls(ctx);
			::enable_camera_controls(ctx);
			::enable_ant_controls(ctx);
		}
	);
	
	// Queue fade in
	fade_in_to(ctx, nullptr);
	
	// Refresh frame scheduler
	ctx.frame_scheduler.refresh();

	debug::log_trace("Entered test experiment state");
}

test_state::~test_state()
{
	debug::log_trace("Exiting test state...");
	
	// Disable game controls
	::disable_game_controls(ctx);
	::disable_camera_controls(ctx);
	::disable_ant_controls(ctx);
	
	ctx.controlled_ant_eid = entt::null;
	
	destroy_third_person_camera_rig();
	
	debug::log_trace("Exited test state");
}

void test_state::create_third_person_camera_rig()
{
	const auto& subject_rigid_body = *ctx.entity_registry->get<rigid_body_component>(ctx.controlled_ant_eid).body;
	const auto subject_scale = static_cast<double>(subject_rigid_body.get_transform().scale.x());
	
	spring_arm_component spring_arm;
	spring_arm.parent_eid = ctx.controlled_ant_eid;
	spring_arm.near_focal_plane_height = 8.0 * subject_scale;
	spring_arm.far_focal_plane_height = 80.0 * subject_scale;
	spring_arm.near_hfov = math::radians(90.0);
	// spring_arm.far_hfov = math::radians(45.0);
	spring_arm.far_hfov = math::radians(90.0);
	spring_arm.zoom = 0.25;
	spring_arm.focal_point_offset = {0, static_cast<double>(worker_phenome->legs->standing_height) * subject_scale, 0};
	
	spring_arm.focal_point_spring.set_damping_ratio(1.0);
	spring_arm.focal_point_spring.set_period(0.01);
	
	spring_arm.angles_spring.set_damping_ratio(1.0);
	spring_arm.angles_spring.set_period(0.25);
	
	spring_arm.min_angles.x() = -math::half_pi<double>;
	spring_arm.max_angles.x() = 0.0;
	
	third_person_camera_rig_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace_or_replace<scene_component>(third_person_camera_rig_eid, ctx.exterior_camera, std::uint8_t{1});
	ctx.entity_registry->emplace<spring_arm_component>(third_person_camera_rig_eid, std::move(spring_arm));
	ctx.active_camera_eid = third_person_camera_rig_eid;
}

void test_state::destroy_third_person_camera_rig()
{
	ctx.entity_registry->destroy(third_person_camera_rig_eid);
}

void test_state::handle_mouse_motion(const input::mouse_moved_event& event)
{
	ctx.scene_material_pass->set_mouse_position(math::fvec2(event.position));
	
	if (!mouse_look && !mouse_grip && !mouse_zoom)
	{
		return;
	}
	
	if (mouse_grip)
	{
		const math::dvec2 viewport_size = math::dvec2(ctx.window->get_viewport_size());
		
		math::dvec3 translation
		{
			third_person_camera_focal_plane_width * (static_cast<double>(-event.difference.x()) / (viewport_size.x() - 1.0)),
			0.0,
			third_person_camera_focal_plane_height * (static_cast<double>(-event.difference.y()) / (viewport_size.y() - 1.0)),
		};
		
		if (third_person_camera_pitch < 0.0)
		{
			translation.z() *= -1.0;
		}
		
		third_person_camera_focal_point += third_person_camera_yaw_rotation * translation;
	}
}

geom::ray<float, 3> test_state::get_mouse_ray(const math::vec2<std::int32_t>& mouse_position) const
{
	// Get window viewport size
	const auto& viewport_size = ctx.window->get_viewport_size();
	
	// Transform mouse coordinates from window space to NDC space
	const math::fvec2 mouse_ndc =
	{
		static_cast<float>(mouse_position.x()) / static_cast<float>(viewport_size.x() - 1) * 2.0f - 1.0f,
		(1.0f - static_cast<float>(mouse_position.y()) / static_cast<float>(viewport_size.y() - 1)) * 2.0f - 1.0f
	};
	
	const auto& scene_component = ctx.entity_registry->get<::scene_component>(third_person_camera_rig_eid);
	const auto& camera = static_cast<const scene::camera&>(*scene_component.object);
	
	return camera.pick(mouse_ndc);
}

void test_state::setup_controls()
{
	// Enable/toggle mouse grip
	action_subscriptions.emplace_back
	(
		ctx.camera_mouse_pick_action.get_activated_channel().subscribe
		(
			[&]([[maybe_unused]] const auto& event)
			{
				mouse_grip = ctx.toggle_mouse_grip ? !mouse_grip : true;
				
				if (mouse_grip)
				{
					const auto& mouse_position = (*ctx.input_manager->get_mice().begin())->get_position();
					
					// Cast ray to plane
					const auto mouse_ray = get_mouse_ray(mouse_position);
					const auto intersection = geom::intersection(mouse_ray, mouse_grip_plane);
					if (intersection)
					{
						mouse_grip_point = mouse_ray.origin + mouse_ray.direction * (*intersection);
					}
				}
				
				// BVH picking test
				const auto& mouse_position = (*ctx.input_manager->get_mice().begin())->get_position();
				const auto mouse_ray = get_mouse_ray(mouse_position);
				
				const auto& camera_object = *ctx.entity_registry->get<::scene_component>(ctx.active_camera_eid).object;
				
				if (auto trace = ctx.physics_system->trace(mouse_ray, entt::null, camera_object.get_layer_mask()))
				{
					// debug::log_debug("HIT! EID: {}; distance: {}; face: {}", static_cast<int>(std::get<0>(*trace)), std::get<1>(*trace), std::get<2>(*trace));
					
					const auto& hit_rigid_body = *ctx.entity_registry->get<rigid_body_component>(std::get<0>(*trace)).body;
					const auto& hit_collider = *hit_rigid_body.get_collider();
					const auto hit_distance = std::get<1>(*trace);
					const auto& hit_normal = std::get<3>(*trace);
					
					geom::brep_mesh* hit_mesh = nullptr;
					geom::brep_face* hit_face = nullptr;
					if (hit_collider.type() == physics::collider_type::mesh)
					{
						hit_mesh = static_cast<const physics::mesh_collider&>(hit_collider).get_mesh().get();
						hit_face = hit_mesh->faces()[std::get<2>(*trace)];
					}
					
					// Update agent transform
					auto& agent_rigid_body = *ctx.entity_registry->get<rigid_body_component>(worker_eid).body;
					auto agent_transform = agent_rigid_body.get_transform();
					agent_transform.translation = mouse_ray.extrapolate(hit_distance);
					agent_transform.rotation = math::rotation(math::fvec3{0, 1, 0}, hit_normal);
					agent_rigid_body.set_transform(agent_transform);
					agent_rigid_body.set_previous_transform(agent_transform);
					
					// Update agent navmesh
					ctx.entity_registry->patch<navmesh_agent_component>
					(
						worker_eid,
						[&](auto& component)
						{
							component.navmesh_eid = std::get<0>(*trace);
							component.mesh = hit_mesh;
							component.feature = hit_face;
							component.surface_normal = hit_normal;
						}
					);
				}
			}
		)
	);
}
