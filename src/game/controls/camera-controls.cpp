// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/controls.hpp"
#include "game/components/spring-arm-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include "game/components/scene-component.hpp"
#include <engine/debug/log.hpp>

namespace {
	
	void handle_mouse_motion(::game& ctx, const input::mouse_moved_event& event)
	{
		if (ctx.active_camera_eid == entt::null)
		{
			return;
		}
		
		auto& spring_arm = ctx.entity_registry->get<spring_arm_component>(ctx.active_camera_eid);
		
		// Rotate camera
		if (ctx.camera_mouse_look_action.is_active())
		{
			// Adjust target pitch and yaw angles according to mouse motion
			auto target_angles = spring_arm.angles_spring.get_target_value();
			target_angles.x() -= ctx.mouse_tilt_factor * static_cast<double>(event.difference.y());
			target_angles.y() -= ctx.mouse_pan_factor * static_cast<double>(event.difference.x());
			
			// Apply angular constraints
			target_angles = math::clamp(target_angles, spring_arm.min_angles, spring_arm.max_angles);
			
			// Update spring arm target angles
			spring_arm.angles_spring.set_target_value(target_angles);
		}
		
		// Zoom camera
		if (ctx.camera_mouse_zoom_action.is_active())
		{
			// Adjust zoom factor
			spring_arm.zoom -= static_cast<double>(event.difference.y()) / static_cast<double>(ctx.window->get_viewport_size().y());
			
			// Limit zoom factor
			spring_arm.zoom = std::min<double>(std::max<double>(spring_arm.zoom, 0.0), 1.0);
		}
	}
	
	void step_camera_zoom(::game& ctx, double scale)
	{
		if (ctx.active_camera_eid == entt::null)
		{
			return;
		}
		
		auto& spring_arm = ctx.entity_registry->get<spring_arm_component>(ctx.active_camera_eid);
		auto target_angles = spring_arm.angles_spring.get_target_value();
		
		// Modulate target pitch angle
		target_angles.x() += (spring_arm.max_angles.x() - spring_arm.min_angles.x()) / ctx.zoom_steps * scale;
		
		// Apply angular constraints
		target_angles = math::clamp(target_angles, spring_arm.min_angles, spring_arm.max_angles);
		
		// Update spring arm target angles
		spring_arm.angles_spring.set_target_value(target_angles);
	}
	
	void update_relative_mouse_mode(::game& ctx)
	{
		if (ctx.camera_mouse_look_action.is_active() ||
			ctx.camera_mouse_drag_action.is_active() ||
			ctx.camera_mouse_zoom_action.is_active())
		{
			ctx.input_manager->set_relative_mouse_mode(true);
		}
		else
		{
			ctx.input_manager->set_relative_mouse_mode(false);
		}
	}
}

void setup_camera_controls(::game& ctx)
{
	// Camera mouse motion
	ctx.event_subscriptions.emplace_back
	(
		ctx.input_manager->get_event_dispatcher().subscribe<input::mouse_moved_event>
		(
			[&](const auto& event)
			{
				if (ctx.camera_action_map.is_enabled())
				{
					handle_mouse_motion(ctx, event);
				}
			}
		)
	);
	
	// Camera mouse pick
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_mouse_pick_action.get_activated_channel().subscribe
		(
			[&]([[maybe_unused]] const auto& event)
			{
			}
		)
	);
	
	// Camera mouse look
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_mouse_look_action.get_activated_channel().subscribe
		(
			[&]([[maybe_unused]] const auto& event)
			{
				update_relative_mouse_mode(ctx);
			}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_mouse_look_action.get_deactivated_channel().subscribe
		(
			[&]([[maybe_unused]] const auto& event)
			{
				update_relative_mouse_mode(ctx);
			}
		)
	);
	
	// Camera mouse drag
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_mouse_drag_action.get_activated_channel().subscribe
		(
			[&]([[maybe_unused]] const auto& event)
			{
				update_relative_mouse_mode(ctx);
				
				/*
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
				*/
			}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_mouse_drag_action.get_deactivated_channel().subscribe
		(
			[&]([[maybe_unused]] const auto& event)
			{
				update_relative_mouse_mode(ctx);
			}
		)
	);
	
	// Camera mouse zoom
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_mouse_zoom_action.get_activated_channel().subscribe
		(
			[&]([[maybe_unused]] const auto& event)
			{
				update_relative_mouse_mode(ctx);
			}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_mouse_zoom_action.get_deactivated_channel().subscribe
		(
			[&]([[maybe_unused]] const auto& event)
			{
				update_relative_mouse_mode(ctx);
			}
		)
	);
	
	// Camera zoom in
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_zoom_in_action.get_activated_channel().subscribe
		(
			[&]([[maybe_unused]] const auto& event)
			{
				step_camera_zoom(ctx, static_cast<double>(ctx.camera_zoom_in_action.get_input_value()));
			}
		)
	);
	
	// Camera zoom out
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_zoom_out_action.get_activated_channel().subscribe
		(
			[&]([[maybe_unused]] const auto& event)
			{
				step_camera_zoom(ctx, -static_cast<double>(ctx.camera_zoom_out_action.get_input_value()));
			}
		)
	);
	
	// Camera orbit left
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_orbit_left_action.get_active_channel().subscribe
		(
			[&]([[maybe_unused]] const auto& event)
			{
				if (ctx.active_camera_eid == entt::null)
				{
					return;
				}
				
				auto& spring_arm = ctx.entity_registry->get<spring_arm_component>(ctx.active_camera_eid);
				spring_arm.angular_velocities.y() = -ctx.gamepad_pan_factor * static_cast<double>(event.input_value);
			}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_orbit_left_action.get_deactivated_channel().subscribe
		(
			[&]([[maybe_unused]] const auto& event)
			{
				if (ctx.active_camera_eid == entt::null)
				{
					return;
				}
				
				auto& spring_arm = ctx.entity_registry->get<spring_arm_component>(ctx.active_camera_eid);
				spring_arm.angular_velocities.y() = 0.0;
			}
		)
	);
	
	// Camera orbit right
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_orbit_right_action.get_active_channel().subscribe
		(
			[&]([[maybe_unused]] const auto& event)
			{
				if (ctx.active_camera_eid == entt::null)
				{
					return;
				}
				
				auto& spring_arm = ctx.entity_registry->get<spring_arm_component>(ctx.active_camera_eid);
				spring_arm.angular_velocities.y() = ctx.gamepad_pan_factor * static_cast<double>(event.input_value);
			}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_orbit_right_action.get_deactivated_channel().subscribe
		(
			[&]([[maybe_unused]] const auto& event)
			{
				if (ctx.active_camera_eid == entt::null)
				{
					return;
				}
				
				auto& spring_arm = ctx.entity_registry->get<spring_arm_component>(ctx.active_camera_eid);
				spring_arm.angular_velocities.y() = 0.0;
			}
		)
	);
	
	// Camera orbit up
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_orbit_up_action.get_active_channel().subscribe
		(
			[&]([[maybe_unused]] const auto& event)
			{
				if (ctx.active_camera_eid == entt::null)
				{
					return;
				}
				
				auto& spring_arm = ctx.entity_registry->get<spring_arm_component>(ctx.active_camera_eid);
				spring_arm.angular_velocities.x() = ctx.gamepad_tilt_factor * static_cast<double>(event.input_value);
			}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_orbit_up_action.get_deactivated_channel().subscribe
		(
			[&]([[maybe_unused]] const auto& event)
			{
				if (ctx.active_camera_eid == entt::null)
				{
					return;
				}
				
				auto& spring_arm = ctx.entity_registry->get<spring_arm_component>(ctx.active_camera_eid);
				spring_arm.angular_velocities.x() = 0.0;
			}
		)
	);
	
	// Camera orbit down
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_orbit_down_action.get_active_channel().subscribe
		(
			[&]([[maybe_unused]] const auto& event)
			{
				if (ctx.active_camera_eid == entt::null)
				{
					return;
				}
				
				auto& spring_arm = ctx.entity_registry->get<spring_arm_component>(ctx.active_camera_eid);
				spring_arm.angular_velocities.x() = -ctx.gamepad_tilt_factor * static_cast<double>(event.input_value);
			}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_orbit_down_action.get_deactivated_channel().subscribe
		(
			[&]([[maybe_unused]] const auto& event)
			{
				if (ctx.active_camera_eid == entt::null)
				{
					return;
				}
				
				auto& spring_arm = ctx.entity_registry->get<spring_arm_component>(ctx.active_camera_eid);
				spring_arm.angular_velocities.x() = 0.0;
			}
		)
	);
	
	// Camera look ahead
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_look_ahead_action.get_activated_channel().subscribe
		(
			[&]([[maybe_unused]] const auto& event)
			{
				if (ctx.active_camera_eid == entt::null || ctx.controlled_ant_eid == entt::null)
				{
					return;
				}
				
				auto& spring_arm = ctx.entity_registry->get<spring_arm_component>(ctx.active_camera_eid);
				const auto& subject_rigid_body = *ctx.entity_registry->get<rigid_body_component>(ctx.controlled_ant_eid).body;
				
				
				// Determine camera up direction
				const auto camera_up = math::fvec3(spring_arm.up_rotation * math::dvec3{0, 1, 0});

				// Get spring arm target angles
				auto target_angles = spring_arm.angles_spring.get_target_value();

				
				// Determine camera forward direction
				const auto camera_yaw_rotation = math::angle_axis(target_angles.y(), {0.0, 1.0, 0.0});
				const auto camera_pitchless_orientation = math::normalize(spring_arm.up_rotation * camera_yaw_rotation);
				const auto camera_forward = math::fvec3(camera_pitchless_orientation * math::dvec3{0, 0, -1});
				
				// Determine subject forward direction
				const auto subject_forward = subject_rigid_body.get_transform().rotation * math::fvec3{0, 0, 1};
				
				// Find signed angle between the two forward directions about camera up axis
				const auto angular_difference = math::signed_angle(camera_forward, subject_forward, camera_up);
				
				// Add angular difference to spring arm target yaw angle
				target_angles.y() += angular_difference;
				
				// Update spring arm target angles
				spring_arm.angles_spring.set_target_value(target_angles);
			}
		)
	);
}

void enable_camera_controls(::game& ctx)
{
	ctx.camera_action_map.enable();
}

void disable_camera_controls(::game& ctx)
{
	ctx.camera_action_map.disable();
	ctx.camera_action_map.reset();
}
