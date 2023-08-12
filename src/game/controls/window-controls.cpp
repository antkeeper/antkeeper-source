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
#include "game/graphics.hpp"

void setup_window_controls(::game& ctx)
{
	// Toggle fullscreen
	ctx.event_subscriptions.emplace_back
	(
		ctx.fullscreen_action.get_activated_channel().subscribe
		(
			[&ctx](const auto& event)
			{
				ctx.window->set_fullscreen(!ctx.window->is_fullscreen());
			}
		)
	);
	
	// Take screenshot
	ctx.event_subscriptions.emplace_back
	(
		ctx.screenshot_action.get_activated_channel().subscribe
		(
			[&ctx](const auto& event)
			{
				::graphics::save_screenshot(ctx);
			}
		)
	);
}

void enable_window_controls(::game& ctx)
{
	ctx.window_action_map.enable();
}

void disable_window_controls(::game& ctx)
{
	ctx.window_action_map.disable();
	ctx.fullscreen_action.reset();
	ctx.screenshot_action.reset();
}
