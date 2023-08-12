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

#include "game/states/experiments/treadmill-experiment-state.hpp"

#include "game/ant/ant-cladogenesis.hpp"
#include "game/ant/ant-genome.hpp"
#include "game/ant/ant-morphogenesis.hpp"
#include "game/ant/ant-phenome.hpp"
#include "game/commands/commands.hpp"
#include "game/components/pose-component.hpp"
#include "game/components/constraint-stack-component.hpp"
#include "game/components/scene-component.hpp"
#include "game/components/picking-component.hpp"
#include "game/components/spring-component.hpp"
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
#include "game/components/orbit-camera-component.hpp"
#include "game/components/ant-genome-component.hpp"
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
#include <engine/math/interpolation.hpp>
#include <engine/math/projection.hpp>
#include <engine/physics/light/exposure.hpp>
#include <engine/physics/kinematics/constraints/spring-constraint.hpp>
#include <engine/physics/kinematics/colliders/sphere-collider.hpp>
#include <engine/physics/kinematics/colliders/plane-collider.hpp>
#include <engine/physics/kinematics/colliders/box-collider.hpp>
#include <engine/physics/kinematics/colliders/capsule-collider.hpp>
#include <engine/render/passes/clear-pass.hpp>
#include <engine/render/passes/ground-pass.hpp>
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

treadmill_experiment_state::treadmill_experiment_state(::game& ctx):
	game_state(ctx)
{
	debug::log::trace("Entering nest view state...");	
	
	// Create world if not yet created
	if (ctx.entities.find("earth") == ctx.entities.end())
	{
		// Create cosmos
		::world::cosmogenesis(ctx);
		
		// Create observer
		::world::create_observer(ctx);
	}
	
	ctx.active_scene = ctx.surface_scene.get();
	
	ctx.active_ecoregion = ctx.resource_manager->load<::ecoregion>("seedy-scrub.eco");
	::world::enter_ecoregion(ctx, *ctx.active_ecoregion);
	
	debug::log::trace("Generating genome...");
	std::shared_ptr<ant_genome> genome = ant_cladogenesis(ctx.active_ecoregion->gene_pools[0], ctx.rng);
	debug::log::trace("Generated genome");
	
	debug::log::trace("Building worker phenome...");
	worker_phenome = std::make_shared<ant_phenome>(*genome, ant_caste_type::worker);
	debug::log::trace("Built worker phenome...");
	
	debug::log::trace("Generating worker model...");
	std::shared_ptr<render::model> worker_model = ant_morphogenesis(*worker_phenome);
	debug::log::trace("Generated worker model");
	
	// Create directional light
	// ctx.underground_directional_light = std::make_unique<scene::directional_light>();
	// ctx.underground_directional_light->set_color({1.0f, 1.0f, 1.0f});
	// ctx.underground_directional_light->set_illuminance(2.0f);
	// ctx.underground_directional_light->set_direction(math::normalize(math::fvec3{0, -1, 0}));
	// ctx.underground_directional_light->set_shadow_caster(true);
	// ctx.underground_directional_light->set_shadow_framebuffer(ctx.shadow_map_framebuffer);
	// ctx.underground_directional_light->set_shadow_bias(0.005f);
	// ctx.underground_directional_light->set_shadow_cascade_count(4);
	// ctx.underground_directional_light->set_shadow_cascade_coverage(0.15f);
	// ctx.underground_directional_light->set_shadow_cascade_distribution(0.8f);
	// ctx.underground_scene->add_object(*ctx.underground_directional_light);
	
	// ctx.underground_clear_pass->set_clear_color({0.214f, 0.214f, 0.214f, 1.0f});
	// ctx.underground_clear_pass->set_clear_color({});
	// light_probe = std::make_shared<scene::light_probe>();
	// light_probe->set_luminance_texture(ctx.resource_manager->load<gl::texture_cube>("grey-furnace.tex"));
	// ctx.underground_scene->add_object(*light_probe);
	
	//const float color_temperature = 5000.0f;
	//const math::fvec3 light_color = color::aces::ap1<float>.from_xyz * color::cat::matrix(color::illuminant::deg2::d50<float>, color::aces::white_point<float>) * color::cct::to_xyz(color_temperature);
	// const math::fvec3 light_color{1.0f, 1.0f, 1.0f}; 
	
	// Create rectangle light
	// ctx.underground_rectangle_light = std::make_unique<scene::rectangle_light>();
	// ctx.underground_rectangle_light->set_color(light_color);
	// ctx.underground_rectangle_light->set_luminous_flux(1500.0f);
	// ctx.underground_rectangle_light->set_translation({0.0f, 10.0f, 0.0f});
	// ctx.underground_rectangle_light->set_rotation(math::fquat::rotate_x(math::radians(90.0f)));
	// ctx.underground_rectangle_light->set_scale(7.0f);
	// ctx.underground_scene->add_object(*ctx.underground_rectangle_light);
	
	// Create light rectangle
	// auto light_rectangle_model = ctx.resource_manager->load<render::model>("light-rectangle.mdl");
	// auto light_rectangle_material = std::make_shared<render::material>(*light_rectangle_model->get_groups().front().material);
	// light_rectangle_emissive = std::static_pointer_cast<render::matvar_fvec3>(light_rectangle_material->get_variable("emissive"));
	// light_rectangle_emissive->set(ctx.underground_rectangle_light->get_colored_luminance());
	// auto light_rectangle_static_mesh = std::make_shared<scene::static_mesh>(light_rectangle_model);
	// light_rectangle_static_mesh->set_material(0, light_rectangle_material);
	
	// auto light_rectangle_eid = ctx.entity_registry->create();
	// ctx.entity_registry->emplace<scene_component>(light_rectangle_eid, std::move(light_rectangle_static_mesh), std::uint8_t{1});
	// ctx.entity_registry->patch<scene_component>
	// (
		// light_rectangle_eid,
		// [&](auto& component)
		// {
			// component.object->set_transform(ctx.underground_rectangle_light->get_transform());
		// }
	// );
	
	// Create treadmill
	auto treadmill_eid = ctx.entity_registry->create();
	scene_component treadmill_scene_component;
	treadmill_scene_component.object = std::make_shared<scene::static_mesh>(ctx.resource_manager->load<render::model>("mobius-strip-nest-400mm.mdl"));
	treadmill_scene_component.layer_mask = 1;
	ctx.entity_registry->emplace<scene_component>(treadmill_eid, std::move(treadmill_scene_component));
	
	// Load navmesh
	navmesh = ctx.resource_manager->load<geom::brep_mesh>("mobius-strip-nest-400mm.msh");
	
	// Generate navmesh attributes
	geom::generate_vertex_normals(*navmesh);
	
	// Build navmesh BVH
	debug::log::info("Building BVH...");
	navmesh_bvh = std::make_unique<geom::bvh>(*navmesh);
	debug::log::info("Built BVH");
	
	// Create worker
	auto worker_skeletal_mesh = std::make_unique<scene::skeletal_mesh>(worker_model);
	
	// Create worker IK rig
	const auto& worker_skeleton = worker_model->get_skeleton();
	worker_ik_rig = std::make_shared<ik_rig>(*worker_skeletal_mesh);
	auto mesocoxa_ik_constraint = std::make_shared<euler_ik_constraint>();
	mesocoxa_ik_constraint->set_min_angles({-math::pi<float>, -math::pi<float>, -math::pi<float>});
	mesocoxa_ik_constraint->set_max_angles({ math::pi<float>,  math::pi<float>,  math::pi<float>});
	worker_ik_rig->set_constraint(*worker_skeleton.get_bone_index("mesocoxa_l"), std::move(mesocoxa_ik_constraint));
	
	// Pose worker
	worker_skeletal_mesh->get_pose() = *worker_model->get_skeleton().get_pose("midswing");
	
	worker_eid = ctx.entity_registry->create();


	pose_component worker_pose_component;
	worker_pose_component.current_pose = worker_skeletal_mesh->get_skeleton()->get_rest_pose();
	worker_pose_component.previous_pose = worker_pose_component.current_pose;
	

	
	ant_caste_component worker_caste_component;
	worker_caste_component.type = ant_caste_type::worker;
	worker_caste_component.phenome = worker_phenome;
	
	rigid_body_component worker_rigid_body_component;
	worker_rigid_body_component.body = std::make_unique<physics::rigid_body>();
	worker_rigid_body_component.body->set_mass(0.0f);
	auto rigid_body_transform = worker_rigid_body_component.body->get_transform();
	rigid_body_transform.scale = math::fvec3::one() * worker_phenome->body_size->mean_mesosoma_length;
	worker_rigid_body_component.body->set_transform(rigid_body_transform);
	
	legged_locomotion_component worker_locomotion_component;
	worker_locomotion_component.midstance_pose = worker_model->get_skeleton().get_pose("midstance");
	worker_locomotion_component.midswing_pose = worker_model->get_skeleton().get_pose("midswing");
	worker_locomotion_component.liftoff_pose = worker_model->get_skeleton().get_pose("liftoff");
	worker_locomotion_component.touchdown_pose = worker_model->get_skeleton().get_pose("touchdown");
	worker_locomotion_component.body_bone = *worker_skeleton.get_bone_index("mesosoma");
	worker_locomotion_component.tip_bones =
	{
		*worker_skeleton.get_bone_index("protarsomere1_l"),
		*worker_skeleton.get_bone_index("mesotarsomere1_l"),
		*worker_skeleton.get_bone_index("metatarsomere1_l"),
		*worker_skeleton.get_bone_index("protarsomere1_r"),
		*worker_skeleton.get_bone_index("mesotarsomere1_r"),
		*worker_skeleton.get_bone_index("metatarsomere1_r")
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
	
	navmesh_agent_component worker_navmesh_agent_component;
	
	ovary_component worker_ovary_component;
	worker_ovary_component.egg_capacity = 4;
	worker_ovary_component.egg_production_duration = 1.0f;
	worker_ovary_component.oviposition_duration = 3.0f;
	worker_ovary_component.ovipositor_bone = *worker_skeleton.get_bone_index("gaster");
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
	
	// Disable UI color clear
	ctx.ui_clear_pass->set_cleared_buffers(false, true, false);
	
	// Set world time
	::world::set_time(ctx, 2022, 6, 21, 12, 0, 0.0);
	
	// Init time scale
	double time_scale = 60.0;
	
	// Set time scale
	::world::set_time_scale(ctx, time_scale);
	
	// Setup and enable sky and ground passes
	ctx.sky_pass->set_enabled(true);
	ctx.ground_pass->set_enabled(true);
	
	sky_probe = std::make_shared<scene::light_probe>();
	sky_probe->set_luminance_texture(std::make_shared<gl::texture_cube>(384, 512, gl::pixel_type::float_16, gl::pixel_format::rgb));
	ctx.sky_pass->set_sky_probe(sky_probe);
	ctx.surface_scene->add_object(*sky_probe);
	
	// Set camera exposure
	const float ev100_sunny16 = physics::light::ev::from_settings(16.0f, 1.0f / 100.0f, 100.0f);
	ctx.surface_camera->set_exposure_value(ev100_sunny16);
	
	const auto& viewport_size = ctx.window->get_viewport_size();
	const float aspect_ratio = static_cast<float>(viewport_size[0]) / static_cast<float>(viewport_size[1]);
	
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
	ctx.fade_transition_color->set({0, 0, 0});
	ctx.function_queue.push(std::bind(&screen_transition::transition, ctx.fade_transition.get(), 1.0f, true, ease<float>::out_sine, true, nullptr));
	
	// Refresh frame scheduler
	ctx.frame_scheduler.refresh();
	
	debug::log::trace("Entered nest view state");
}

treadmill_experiment_state::~treadmill_experiment_state()
{
	debug::log::trace("Exiting nest view state...");
	
	// Disable game controls
	::disable_game_controls(ctx);
	::disable_camera_controls(ctx);
	::disable_ant_controls(ctx);
	
	ctx.controlled_ant_eid = entt::null;
	
	destroy_third_person_camera_rig();
	
	debug::log::trace("Exited nest view state");
}

void treadmill_experiment_state::create_third_person_camera_rig()
{
	const auto& subject_rigid_body = *ctx.entity_registry->get<rigid_body_component>(ctx.controlled_ant_eid).body;
	orbit_camera_component orbit_cam;
	orbit_cam.subject_eid = ctx.controlled_ant_eid;
	orbit_cam.yaw = math::radians(0.0);
	orbit_cam.pitch = math::radians(45.0);
	orbit_cam.near_focal_plane_height = {1.0};
	orbit_cam.far_focal_plane_height = {50.0};
	orbit_cam.near_hfov = math::radians(90.0);
	orbit_cam.far_hfov = math::radians(45.0);
	orbit_cam.zoom = 0.25;
	orbit_cam.focal_point = {0, worker_phenome->legs->standing_height * subject_rigid_body.get_transform().scale.x(), 0};
	
	third_person_camera_rig_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<scene_component>(third_person_camera_rig_eid, ctx.surface_camera, std::uint8_t{1});
	ctx.entity_registry->emplace<orbit_camera_component>(third_person_camera_rig_eid, std::move(orbit_cam));
	
	ctx.active_camera_eid = third_person_camera_rig_eid;
	
	/*
	// Construct third person camera rig entity
	third_person_camera_rig_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<scene_component>(third_person_camera_rig_eid, third_person_camera_rig_camera);
	
	set_third_person_camera_zoom(third_person_camera_zoom);
	set_third_person_camera_rotation(third_person_camera_yaw, third_person_camera_pitch);
	update_third_person_camera();
	*/
}

void treadmill_experiment_state::destroy_third_person_camera_rig()
{
	ctx.entity_registry->destroy(third_person_camera_rig_eid);
}

void treadmill_experiment_state::set_third_person_camera_zoom(double zoom)
{
	zoom = std::min<double>(std::max<double>(zoom, 0.0), 1.0);
	
	auto& orbit_cam = ctx.entity_registry->get<orbit_camera_component>(third_person_camera_rig_eid);
	orbit_cam.zoom = zoom;
}

void treadmill_experiment_state::set_third_person_camera_rotation(double yaw, double pitch)
{
	pitch = std::min(math::half_pi<double>, std::max(-math::half_pi<double>, pitch));
	
	auto& orbit_cam = ctx.entity_registry->get<orbit_camera_component>(third_person_camera_rig_eid);
	orbit_cam.yaw = yaw;
	orbit_cam.pitch = pitch;
}

void treadmill_experiment_state::zoom_third_person_camera(double zoom)
{
	const auto& orbit_cam = ctx.entity_registry->get<orbit_camera_component>(third_person_camera_rig_eid);

	set_third_person_camera_zoom(orbit_cam.zoom + zoom);
}

void treadmill_experiment_state::translate_third_person_camera(const math::dvec3& direction, double magnitude)
{
	auto& orbit_cam = ctx.entity_registry->get<orbit_camera_component>(third_person_camera_rig_eid);
	
	// Scale translation magnitude by factor of focal plane height
	const auto scaled_magnitude = magnitude * orbit_cam.focal_plane_height;
	
	// Rotate translation direction according to camera yaw
	const math::dvec3 rotated_direction = orbit_cam.yaw_rotation * direction;
	
	orbit_cam.focal_point += rotated_direction * scaled_magnitude * third_person_camera_speed;
}

void treadmill_experiment_state::rotate_third_person_camera(const input::mouse_moved_event& event)
{
	const auto& orbit_cam = ctx.entity_registry->get<orbit_camera_component>(third_person_camera_rig_eid);
	
	const double yaw = orbit_cam.yaw - ctx.mouse_pan_factor * static_cast<double>(event.difference.x());
	const double pitch = orbit_cam.pitch + ctx.mouse_tilt_factor * static_cast<double>(event.difference.y());
	
	set_third_person_camera_rotation(yaw, pitch);
}

void treadmill_experiment_state::handle_mouse_motion(const input::mouse_moved_event& event)
{
	ctx.surface_material_pass->set_mouse_position(math::fvec2(event.position));
	
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
	
	if (mouse_look)
	{
		rotate_third_person_camera(event);
	}
	
	if (mouse_zoom)
	{
		const double zoom_speed = -1.0 / static_cast<double>(ctx.window->get_viewport_size().y());
		zoom_third_person_camera(static_cast<double>(event.difference.y()) * zoom_speed);
	}
	
	update_third_person_camera();
}

void treadmill_experiment_state::update_third_person_camera()
{
}

void treadmill_experiment_state::load_camera_preset(std::uint8_t index)
{

}

void treadmill_experiment_state::save_camera_preset(std::uint8_t index)
{

}

void treadmill_experiment_state::load_or_save_camera_preset(std::uint8_t index)
{

}

geom::ray<float, 3> treadmill_experiment_state::get_mouse_ray(const math::vec2<std::int32_t>& mouse_position) const
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

void treadmill_experiment_state::setup_controls()
{
	// Enable/toggle mouse grip
	action_subscriptions.emplace_back
	(
		ctx.camera_mouse_pick_action.get_activated_channel().subscribe
		(
			[&](const auto& event)
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
				
				debug::log::info("pick:");
				float nearest_hit = std::numeric_limits<float>::infinity();
				bool hit = false;
				std::uint32_t hit_index;
				const auto& vertex_positions = navmesh->vertices().attributes().at<math::fvec3>("position");
				const auto& face_normals = navmesh->faces().attributes().at<math::fvec3>("normal");
				std::size_t test_count = 0;
				
				
				int box_test_passed = 0;
				navmesh_bvh->visit
				(
					mouse_ray,
					[&](std::uint32_t index)
					{
						++box_test_passed;
						
						geom::brep_face* face = navmesh->faces()[index];
						const auto& n = face_normals[index];
						if (math::dot(n, mouse_ray.direction) > 0.0f)
						{
							return;
						}
						
						auto loop = face->loops().begin();
						const auto& a = vertex_positions[loop->vertex()->index()];
						const auto& b = vertex_positions[(++loop)->vertex()->index()];
						const auto& c = vertex_positions[(++loop)->vertex()->index()];
						
						if (auto intersection = geom::intersection(mouse_ray, a, b, c))
						{
							++test_count;
							float t = std::get<0>(*intersection);
							if (t < nearest_hit)
							{
								hit = true;
								nearest_hit = t;
								hit_index = index;
							}
						}
					}
				);
				
				debug::log::info("box tests passed: {}", box_test_passed);
				
				if (hit)
				{
					const auto& navmesh_face_normals = navmesh->faces().attributes().at<math::fvec3>("normal");
					
					// Update agent transform
					auto& agent_rigid_body = *ctx.entity_registry->get<::rigid_body_component>(worker_eid).body;
					auto agent_transform = agent_rigid_body.get_transform();
					agent_transform.translation = mouse_ray.extrapolate(nearest_hit);
					agent_transform.rotation = math::rotation(math::fvec3{0, 1, 0}, navmesh_face_normals[hit_index]);
					agent_rigid_body.set_transform(agent_transform);
					agent_rigid_body.set_previous_transform(agent_transform);
					
					// Update agent navmesh
					ctx.entity_registry->patch<navmesh_agent_component>
					(
						worker_eid,
						[&](auto& component)
						{
							component.mesh = navmesh.get();
							component.face = navmesh->faces()[hit_index];
							component.surface_normal = navmesh_face_normals[hit_index];
						}
					);
					
					debug::log::info("hit! test count: {}", test_count);
				}
				else
				{
					debug::log::info("no hit");
				}
			}
		)
	);
}
