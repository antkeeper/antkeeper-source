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

#include "game/controls.hpp"
#include "game/components/orbit-camera-component.hpp"
#include <engine/debug/log.hpp>

namespace {
	
	/*
	void orbit_camera(::game& ctx, float yaw_factor, float pitch_factor)
	{
		if (ctx.active_camera_eid == entt::null)
		{
			return;
		}

		auto& orbit_cam = ctx.entity_registry->get<orbit_camera_component>(ctx.active_camera_eid);
		
		// Adjust yaw and pitch angles according to mouse motion
		orbit_cam.yaw -= ctx.mouse_pan_factor * static_cast<double>(event.difference.x());
		orbit_cam.pitch += ctx.mouse_tilt_factor * static_cast<double>(event.difference.y());
		
		// Limit pitch
		orbit_cam.pitch = std::min(math::half_pi<double>, std::max(-math::half_pi<double>, orbit_cam.pitch));
	}
	*/
	
	void handle_mouse_motion(::game& ctx, const input::mouse_moved_event& event)
	{
		if (ctx.active_camera_eid == entt::null)
		{
			return;
		}
		
		auto& orbit_cam = ctx.entity_registry->get<orbit_camera_component>(ctx.active_camera_eid);
		
		// Rotate camera
		if (ctx.camera_mouse_look_action.is_active())
		{
			// Adjust yaw and pitch angles according to mouse motion
			orbit_cam.yaw -= ctx.mouse_pan_factor * static_cast<double>(event.difference.x());
			orbit_cam.pitch += ctx.mouse_tilt_factor * static_cast<double>(event.difference.y());
			
			// Limit pitch
			orbit_cam.pitch = std::min(math::half_pi<double>, std::max(-math::half_pi<double>, orbit_cam.pitch));
		}
		
		// Zoom camera
		if (ctx.camera_mouse_zoom_action.is_active())
		{
			// Adjust zoom factor
			orbit_cam.zoom -= static_cast<double>(event.difference.y()) / static_cast<double>(ctx.window->get_viewport_size().y());
			
			// Limit zoom factor
			orbit_cam.zoom = std::min<double>(std::max<double>(orbit_cam.zoom, 0.0), 1.0);
		}
	}
	
	void load_camera_preset(::game& ctx,std::uint8_t index)
	{
		
	}
	
	void save_camera_preset(::game& ctx,std::uint8_t index)
	{
		
	}
	
	void step_camera_zoom(::game& ctx, double scale)
	{
		if (ctx.active_camera_eid == entt::null)
		{
			return;
		}
		
		auto& orbit_cam = ctx.entity_registry->get<orbit_camera_component>(ctx.active_camera_eid);
		
		// Step zoom factor
		constexpr double zoom_step = 1.0 / 6.0;
		orbit_cam.zoom += zoom_step * scale;
		
		// Limit zoom factor
		orbit_cam.zoom = std::min<double>(std::max<double>(orbit_cam.zoom, 0.0), 1.0);
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
	
	void load_or_save_camera_preset(::game& ctx, std::uint8_t index)
	{
		if (ctx.camera_save_preset_action.is_active())
		{
			save_camera_preset(ctx, index);
		}
		else
		{
			load_camera_preset(ctx, index);
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
			[&](const auto& event)
			{
			}
		)
	);
	
	// Camera mouse look
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_mouse_look_action.get_activated_channel().subscribe
		(
			[&](const auto& event)
			{
				update_relative_mouse_mode(ctx);
			}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_mouse_look_action.get_deactivated_channel().subscribe
		(
			[&](const auto& event)
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
			[&](const auto& event)
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
			[&](const auto& event)
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
			[&](const auto& event)
			{
				update_relative_mouse_mode(ctx);
			}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_mouse_zoom_action.get_deactivated_channel().subscribe
		(
			[&](const auto& event)
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
			[&](const auto& event)
			{
				step_camera_zoom(ctx, 1.0 * static_cast<double>(ctx.camera_zoom_in_action.get_input_value()));
			}
		)
	);
	
	// Camera zoom out
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_zoom_out_action.get_activated_channel().subscribe
		(
			[&](const auto& event)
			{
				step_camera_zoom(ctx, -1.0 * static_cast<double>(ctx.camera_zoom_out_action.get_input_value()));
			}
		)
	);
	
	// Camera orbit left
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_orbit_left_action.get_active_channel().subscribe
		(
			[&](const auto& event)
			{
				
			}
		)
	);
	
	// Camera orbit right
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_orbit_right_action.get_active_channel().subscribe
		(
			[&](const auto& event)
			{
				
			}
		)
	);
	
	// Camera orbit up
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_orbit_up_action.get_active_channel().subscribe
		(
			[&](const auto& event)
			{
				
			}
		)
	);
	
	// Camera orbit down
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_orbit_down_action.get_active_channel().subscribe
		(
			[&](const auto& event)
			{
				
			}
		)
	);
	
	// Camera presets
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_preset_1_action.get_activated_channel().subscribe
		(
			[&](const auto& event) {load_or_save_camera_preset(ctx, 0);}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_preset_2_action.get_activated_channel().subscribe
		(
			[&](const auto& event) {load_or_save_camera_preset(ctx, 1);}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_preset_3_action.get_activated_channel().subscribe
		(
			[&](const auto& event) {load_or_save_camera_preset(ctx, 2);}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_preset_4_action.get_activated_channel().subscribe
		(
			[&](const auto& event) {load_or_save_camera_preset(ctx, 3);}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_preset_5_action.get_activated_channel().subscribe
		(
			[&](const auto& event) {load_or_save_camera_preset(ctx, 4);}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_preset_6_action.get_activated_channel().subscribe
		(
			[&](const auto& event) {load_or_save_camera_preset(ctx, 5);}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_preset_7_action.get_activated_channel().subscribe
		(
			[&](const auto& event) {load_or_save_camera_preset(ctx, 6);}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_preset_8_action.get_activated_channel().subscribe
		(
			[&](const auto& event) {load_or_save_camera_preset(ctx, 7);}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_preset_9_action.get_activated_channel().subscribe
		(
			[&](const auto& event) {load_or_save_camera_preset(ctx, 8);}
		)
	);
	ctx.event_subscriptions.emplace_back
	(
		ctx.camera_preset_10_action.get_activated_channel().subscribe
		(
			[&](const auto& event) {load_or_save_camera_preset(ctx, 9);}
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
