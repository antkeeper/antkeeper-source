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

#include "game/states/nest-view-state.hpp"

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

nest_view_state::nest_view_state(::game& ctx):
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
	
	ctx.active_ecoregion = ctx.resource_manager->load<::ecoregion>("seedy-scrub.eco");
	::world::enter_ecoregion(ctx, *ctx.active_ecoregion);
	
	debug::log::trace("Generating genome...");
	std::unique_ptr<ant_genome> genome = ant_cladogenesis(ctx.active_ecoregion->gene_pools[0], ctx.rng);
	debug::log::trace("Generated genome");
	
	debug::log::trace("Building worker phenome...");
	worker_phenome = ant_phenome(*genome, ant_caste_type::worker);
	debug::log::trace("Built worker phenome...");
	
	debug::log::trace("Generating worker model...");
	std::shared_ptr<render::model> worker_model = ant_morphogenesis(worker_phenome);
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
	
	ctx.underground_clear_pass->set_clear_color({0.214f, 0.214f, 0.214f, 1.0f});
	// ctx.underground_clear_pass->set_clear_color({});
	light_probe = std::make_shared<scene::light_probe>();
	light_probe->set_luminance_texture(ctx.resource_manager->load<gl::texture_cube>("grey-furnace.tex"));
	ctx.underground_scene->add_object(*light_probe);
	
	const math::fvec3 light_color{1.0f, 1.0f, 1.0f}; 
	
	// Create rectangle light
	ctx.underground_rectangle_light = std::make_unique<scene::rectangle_light>();
	ctx.underground_rectangle_light->set_color(light_color);
	ctx.underground_rectangle_light->set_luminous_flux(1500.0f);
	ctx.underground_rectangle_light->set_translation({0.0f, 10.0f, 0.0f});
	ctx.underground_rectangle_light->set_rotation(math::fquat::rotate_x(math::radians(90.0f)));
	ctx.underground_rectangle_light->set_scale(7.0f);
	ctx.underground_scene->add_object(*ctx.underground_rectangle_light);
	
	// Create light rectangle
	auto light_rectangle_model = ctx.resource_manager->load<render::model>("light-rectangle.mdl");
	auto light_rectangle_material = std::make_shared<render::material>(*light_rectangle_model->get_groups().front().material);
	light_rectangle_emissive = std::static_pointer_cast<render::matvar_fvec3>(light_rectangle_material->get_variable("emissive"));
	light_rectangle_emissive->set(ctx.underground_rectangle_light->get_colored_luminance());
	auto light_rectangle_static_mesh = std::make_shared<scene::static_mesh>(light_rectangle_model);
	light_rectangle_static_mesh->set_material(0, light_rectangle_material);
	
	auto light_rectangle_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<scene_component>(light_rectangle_eid, std::move(light_rectangle_static_mesh), std::uint8_t{2});
	ctx.entity_registry->patch<scene_component>
	(
		light_rectangle_eid,
		[&](auto& component)
		{
			component.object->set_transform(ctx.underground_rectangle_light->get_transform());
		}
	);
	
	// Create treadmill
	auto treadmill_eid = ctx.entity_registry->create();
	scene_component treadmill_scene_component;
	treadmill_scene_component.object = std::make_shared<scene::static_mesh>(ctx.resource_manager->load<render::model>("treadmill-5cm.mdl"));
	treadmill_scene_component.layer_mask = 2;
	ctx.entity_registry->emplace<scene_component>(treadmill_eid, std::move(treadmill_scene_component));
	
	// Create worker
	auto worker_skeletal_mesh = std::make_unique<scene::skeletal_mesh>(worker_model);
	worker_eid = ctx.entity_registry->create();
	transform_component worker_transform_component;
	worker_transform_component.local = math::transform<float>::identity();
	worker_transform_component.local.translation = {0, 0.1f, 0};
	worker_transform_component.local.scale = math::fvec3::one() * worker_phenome.body_size->mean_mesosoma_length;
	worker_transform_component.world = worker_transform_component.local;
	ctx.entity_registry->emplace<transform_component>(worker_eid, worker_transform_component);
	ctx.entity_registry->emplace<scene_component>(worker_eid, std::move(worker_skeletal_mesh), std::uint8_t{2});
	
	// Create cocoon
	auto cocoon_eid = ctx.entity_registry->create();
	auto cocoon_static_mesh = std::make_shared<scene::static_mesh>(worker_phenome.pupa->cocoon_model);
	cocoon_static_mesh->set_scale(worker_phenome.body_size->mean_mesosoma_length);
	ctx.entity_registry->emplace<scene_component>(cocoon_eid, std::move(cocoon_static_mesh), std::uint8_t{2});
	ctx.entity_registry->patch<scene_component>
	(
		cocoon_eid,
		[&](auto& component)
		{
			component.object->set_translation({-4.0f, 0.0f, 4.0f});
		}
	);
	
	// Create color checker
	auto color_checker_eid = ctx.entity_registry->create();
	scene_component color_checker_scene_component;
	color_checker_scene_component.object = std::make_shared<scene::static_mesh>(ctx.resource_manager->load<render::model>("color-checker.mdl"));
	color_checker_scene_component.object->set_translation({0, 0, 4});
	color_checker_scene_component.layer_mask = 2;
	ctx.entity_registry->emplace<scene_component>(color_checker_eid, std::move(color_checker_scene_component));
	
	// Create larva
	larva_eid = ctx.entity_registry->create();
	auto larva_skeletal_mesh = std::make_shared<scene::skeletal_mesh>(worker_phenome.larva->model);
	larva_skeletal_mesh->set_scale(worker_phenome.body_size->mean_mesosoma_length);
	ctx.entity_registry->emplace<scene_component>(larva_eid, std::move(larva_skeletal_mesh), std::uint8_t{2});
	ctx.entity_registry->patch<scene_component>
	(
		larva_eid,
		[&](auto& component)
		{
			component.object->set_translation({4.0f, 0.0f, 4.0f});
		}
	);
	
	// Create sphere
	// auto sphere_eid = ctx.entity_registry->create();
	// auto sphere_static_mesh = std::make_shared<scene::static_mesh>(ctx.resource_manager->load<render::model>("sphere.mdl"));
	// ctx.entity_registry->emplace<scene_component>(sphere_eid, std::move(sphere_static_mesh), std::uint8_t{2});
	// ctx.entity_registry->patch<scene_component>
	// (
		// sphere_eid,
		// [&](auto& component)
		// {
			// component.object->set_translation({0.0f, 0.0f, 0.0f});
		// }
	// );
	
	// Disable UI color clear
	ctx.ui_clear_pass->set_cleared_buffers(false, true, false);
	
	// Set world time
	::world::set_time(ctx, 2022, 6, 21, 12, 0, 0.0);
	
	// Init time scale
	double time_scale = 60.0;
	
	// Set time scale
	::world::set_time_scale(ctx, time_scale);
	
	// Setup camera
	ctx.underground_camera->set_exposure_value(0.0f);
	
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
		}
	);
	
	// Queue fade in
	ctx.fade_transition_color->set({0, 0, 0});
	ctx.function_queue.push(std::bind(&screen_transition::transition, ctx.fade_transition.get(), 1.0f, true, ease<float>::out_sine, true, nullptr));
	
	// Refresh frame scheduler
	ctx.frame_scheduler.refresh();
	
	// Load navmesh
	navmesh = ctx.resource_manager->load<geom::brep_mesh>("treadmill-5cm.msh");
	
	// Generate navmesh attributes
	geom::generate_face_normals(*navmesh);
	geom::generate_vertex_normals(*navmesh);
	
	// Build navmesh BVH
	debug::log::info("building bvh");
	navmesh_bvh = std::make_unique<geom::bvh>(*navmesh);
	debug::log::info("building bvh done");
	
	debug::log::trace("Entered nest view state");
}

nest_view_state::~nest_view_state()
{
	debug::log::trace("Exiting nest view state...");
	
	// Disable game controls
	::disable_game_controls(ctx);
	
	destroy_third_person_camera_rig();
	
	debug::log::trace("Exited nest view state");
}

void nest_view_state::create_third_person_camera_rig()
{
	// Construct third person camera rig scene component
	scene_component third_person_camera_rig_camera;
	third_person_camera_rig_camera.object = ctx.underground_camera;
	third_person_camera_rig_camera.layer_mask = 2;
	
	// Construct third person camera rig entity
	third_person_camera_rig_eid = ctx.entity_registry->create();
	ctx.entity_registry->emplace<scene_component>(third_person_camera_rig_eid, third_person_camera_rig_camera);
	
	set_third_person_camera_zoom(third_person_camera_zoom);
	set_third_person_camera_rotation(third_person_camera_yaw, third_person_camera_pitch);
	update_third_person_camera();
}

void nest_view_state::destroy_third_person_camera_rig()
{
	ctx.entity_registry->destroy(third_person_camera_rig_eid);
}

void nest_view_state::set_third_person_camera_zoom(double zoom)
{
	// Clamp zoom
	third_person_camera_zoom = std::min<double>(std::max<double>(zoom, 0.0), 1.0);
	
	// Update FoV
	third_person_camera_hfov = ease<double, double>::out_sine(third_person_camera_far_hfov, third_person_camera_near_hfov, third_person_camera_zoom);
	third_person_camera_vfov = math::vertical_fov(third_person_camera_hfov, static_cast<double>(ctx.underground_camera->get_aspect_ratio()));
	
	// Update focal plane size
	third_person_camera_focal_plane_height = ease<double, double>::out_sine(third_person_camera_far_focal_plane_height, third_person_camera_near_focal_plane_height, third_person_camera_zoom);
	third_person_camera_focal_plane_width = third_person_camera_focal_plane_height * ctx.underground_camera->get_aspect_ratio();
	
	// Update focal distance
	third_person_camera_focal_distance = third_person_camera_focal_plane_height * 0.5 / std::tan(third_person_camera_vfov * 0.5);
	
	// update_third_person_camera();
}

void nest_view_state::set_third_person_camera_rotation(double yaw, double pitch)
{
	third_person_camera_yaw = yaw;
	third_person_camera_pitch = std::min(math::half_pi<double>, std::max(-math::half_pi<double>, pitch));
	
	third_person_camera_yaw_rotation = math::angle_axis(third_person_camera_yaw, {0.0, 1.0, 0.0});
	third_person_camera_pitch_rotation = math::angle_axis(third_person_camera_pitch, {-1.0, 0.0, 0.0});
	third_person_camera_orientation = math::normalize(third_person_camera_yaw_rotation * third_person_camera_pitch_rotation);
}

void nest_view_state::zoom_third_person_camera(double zoom)
{
	set_third_person_camera_zoom(third_person_camera_zoom + zoom);
}

void nest_view_state::translate_third_person_camera(const math::dvec3& direction, double magnitude)
{
	// Scale translation magnitude by factor of focal plane height
	magnitude *= third_person_camera_focal_plane_height * third_person_camera_speed;
	
	// Rotate translation direction according to camera yaw
	const math::dvec3 rotated_direction = third_person_camera_yaw_rotation * direction;
	
	third_person_camera_focal_point += rotated_direction * magnitude;
	
	// update_third_person_camera();
}

void nest_view_state::rotate_third_person_camera(const input::mouse_moved_event& event)
{
	const double yaw = third_person_camera_yaw - ctx.mouse_pan_factor * static_cast<double>(event.difference.x());
	const double pitch = third_person_camera_pitch + ctx.mouse_tilt_factor * static_cast<double>(event.difference.y());
	
	set_third_person_camera_rotation(yaw, pitch);
}

void nest_view_state::handle_mouse_motion(const input::mouse_moved_event& event)
{
	ctx.underground_material_pass->set_mouse_position(math::fvec2(event.position));
	
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

void nest_view_state::update_third_person_camera()
{

}

geom::ray<float, 3> nest_view_state::get_mouse_ray(const math::vec2<std::int32_t>& mouse_position) const
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

void nest_view_state::setup_controls()
{
	/*
	// Enable/toggle mouse look
	action_subscriptions.emplace_back
	(
		ctx.mouse_look_action.get_activated_channel().subscribe
		(
			[&](const auto& event)
			{
				mouse_look = ctx.toggle_mouse_look ? !mouse_look : true;
				
				//ctx.input_manager->set_cursor_visible(!mouse_look);
				ctx.input_manager->set_relative_mouse_mode(mouse_look || mouse_grip || mouse_zoom);
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
					ctx.input_manager->set_relative_mouse_mode(mouse_look || mouse_grip || mouse_zoom);
				}
			}
		)
	);
	
	// Enable/toggle mouse grip
	action_subscriptions.emplace_back
	(
		ctx.mouse_grip_action.get_activated_channel().subscribe
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
				
				ctx.input_manager->set_relative_mouse_mode(mouse_look || mouse_grip || mouse_zoom);
				
				// BVH picking test
				const auto& mouse_position = (*ctx.input_manager->get_mice().begin())->get_position();
				const auto mouse_ray = get_mouse_ray(mouse_position);
				
				debug::log::info("pick:");
				float nearest_hit = std::numeric_limits<float>::infinity();
				bool hit = false;
				std::uint32_t hit_index;
				const auto& vertex_positions = navmesh->vertices().attributes().at<math::fvec3>("position");
				std::size_t test_count = 0;
				
				
				int box_test_passed = 0;
				navmesh_bvh->visit
				(
					mouse_ray,
					[&](std::uint32_t index)
					{
						++box_test_passed;
						
						geom::brep_face* face = navmesh->faces()[index];
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
					
					navmesh_agent_face = navmesh->faces()[hit_index];
					navmesh_agent_position = mouse_ray.extrapolate(nearest_hit);
					navmesh_agent_normal = navmesh_face_normals[hit_index];
					
					
					const auto& hit_normal = navmesh_face_normals[hit_index];
					
					const float standing_height = worker_phenome.legs->standing_height * worker_phenome.body_size->mean_mesosoma_length;
					const math::fvec3 translation = navmesh_agent_position + hit_normal * standing_height;
					
					const math::fquat rotation = math::rotation(math::fvec3{0, 1, 0}, hit_normal);
					
					ctx.entity_registry->patch<scene_component>
					(
						worker_eid,
						[&](auto& component)
						{
							component.object->set_translation(translation);
							component.object->set_rotation(rotation);
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
	
	// Disable mouse grip
	action_subscriptions.emplace_back
	(
		ctx.mouse_grip_action.get_deactivated_channel().subscribe
		(
			[&](const auto& event)
			{
				mouse_grip = false;
				ctx.input_manager->set_relative_mouse_mode(mouse_look || mouse_grip || mouse_zoom);
			}
		)
	);
	*/
	
	// Mouse look
	mouse_motion_subscription = ctx.input_manager->get_event_dispatcher().subscribe<input::mouse_moved_event>
	(
		std::bind_front(&nest_view_state::handle_mouse_motion, this)
	);
	
	
	// Move back
	action_subscriptions.emplace_back
	(
		ctx.move_back_action.get_active_channel().subscribe
		(
			[&](const auto& event)
			{
				translate_third_person_camera({0.0, 0.0, 1.0}, event.input_value / ctx.fixed_update_rate);
				update_third_person_camera();
			}
		)
	);
	
	// Move left
	action_subscriptions.emplace_back
	(
		ctx.move_left_action.get_active_channel().subscribe
		(
			[&](const auto& event)
			{
				// translate_third_person_camera({-1.0, 0.0, 0.0}, event.input_value / ctx.fixed_update_rate);
				// update_third_person_camera();
				
				ctx.entity_registry->patch<::scene_component>
				(
					worker_eid,
					[&](auto& component)
					{
						auto rotation = math::angle_axis(math::radians(30.0f) * event.input_value / ctx.fixed_update_rate, navmesh_agent_normal);
						component.object->set_rotation(math::normalize(rotation * component.object->get_rotation()));
					}
				);
			}
		)
	);
	
	// Move right
	action_subscriptions.emplace_back
	(
		ctx.move_right_action.get_active_channel().subscribe
		(
			[&](const auto& event)
			{
				// translate_third_person_camera({1.0, 0.0, 0.0}, event.input_value / ctx.fixed_update_rate);
				// update_third_person_camera();
				
				ctx.entity_registry->patch<::scene_component>
				(
					worker_eid,
					[&](auto& component)
					{
						auto rotation = math::angle_axis(math::radians(-30.0f) * event.input_value / ctx.fixed_update_rate, navmesh_agent_normal);
						component.object->set_rotation(math::normalize(rotation * component.object->get_rotation()));
					}
				);
			}
		)
	);
	
	// Zoom in
	action_subscriptions.emplace_back
	(
		ctx.move_up_action.get_activated_channel().subscribe
		(
			[&](const auto& event)
			{
				zoom_third_person_camera(1.0 / static_cast<double>(third_person_camera_zoom_step_count));
				update_third_person_camera();
			}
		)
	);
	
	// Zoom out
	action_subscriptions.emplace_back
	(
		ctx.move_down_action.get_activated_channel().subscribe
		(
			[&](const auto& event)
			{
				zoom_third_person_camera(-1.0 / static_cast<double>(third_person_camera_zoom_step_count));
				update_third_person_camera();
			}
		)
	);
}
