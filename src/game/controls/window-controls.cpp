// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
