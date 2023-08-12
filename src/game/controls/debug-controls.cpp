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
#include "game/systems/astronomy-system.hpp"
#include "game/world.hpp"

void setup_debug_controls(::game& ctx)
{
	// Toggle debug UI
	ctx.event_subscriptions.emplace_back
	(
		ctx.toggle_debug_ui_action.get_activated_channel().subscribe
		(
			[&](const auto& event)
			{
				ctx.debug_ui_visible = !ctx.debug_ui_visible;
				if (ctx.debug_ui_visible)
				{
					ctx.ui_scene->add_object(*ctx.frame_time_text);
				}
				else
				{
					ctx.ui_scene->remove_object(*ctx.frame_time_text);
				}
			}
		)
	);
	
	ctx.event_subscriptions.emplace_back
	(
		ctx.input_manager->get_event_dispatcher().subscribe<input::mouse_moved_event>
		(
			[&](const auto& event)
			{
				if (ctx.adjust_time_action.is_active())
				{
					const double sensitivity = 1.0 / static_cast<double>(ctx.window->get_viewport_size().x());
					const double t = ctx.astronomy_system->get_time();
					::world::set_time(ctx, t + static_cast<double>(event.difference.x()) * sensitivity);
				}
				
				if (ctx.adjust_exposure_action.is_active())
				{
					scene::camera* camera{};
					if (ctx.active_scene == ctx.surface_scene.get())
					{
						camera = ctx.surface_camera.get();
					}
					else if (ctx.active_scene == ctx.underground_scene.get())
					{
						camera = ctx.underground_camera.get();
					}
					
					if (camera)
					{
						const float sensitivity = 8.0f / static_cast<float>(ctx.window->get_viewport_size().y());
						ctx.surface_camera->set_exposure_value(ctx.surface_camera->get_exposure_value() + static_cast<float>(event.difference.y()) * sensitivity);
					}
				}
			}
		)
	);
}

void enable_debug_controls(::game& ctx)
{
	ctx.debug_action_map.enable();
}

void disable_debug_controls(::game& ctx)
{
	ctx.debug_action_map.disable();
	ctx.toggle_debug_ui_action.reset();
	ctx.adjust_exposure_action.reset();
	ctx.adjust_time_action.reset();
}
