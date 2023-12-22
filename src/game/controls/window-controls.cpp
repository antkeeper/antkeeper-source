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
			[&ctx]([[maybe_unused]] const auto& event)
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
			[&ctx]([[maybe_unused]] const auto& event)
			{
				::graphics::save_screenshot(ctx);
			}
		)
	);
	
	// Toggle terminal
	ctx.event_subscriptions.emplace_back
	(
		ctx.toggle_terminal_action.get_activated_channel().subscribe
		(
			[&]([[maybe_unused]] const auto& event)
			{
				ctx.terminal_enabled = !ctx.terminal_enabled;
				if (ctx.terminal_enabled)
				{
					if (ctx.menu_action_map.is_enabled())
					{
						ctx.reenable_controls.emplace_back
						(
							[&]()
							{
								enable_menu_controls(ctx);
							}
						);
						disable_menu_controls(ctx);
					}
					
					ctx.ui_scene->add_object(*ctx.command_line_text);
					ctx.ui_scene->add_object(*ctx.shell_buffer_text);
					ctx.input_manager->start_text_input({0, 0, 0, 0});
					enable_terminal_controls(ctx);
				}
				else
				{
					ctx.ui_scene->remove_object(*ctx.command_line_text);
					ctx.ui_scene->remove_object(*ctx.shell_buffer_text);
					ctx.input_manager->stop_text_input();
					disable_terminal_controls(ctx);
					
					for (const auto& f: ctx.reenable_controls)
					{
						f();
					}
					ctx.reenable_controls.clear();
				}
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
