// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/graphics.hpp"
#include "game/controls.hpp"

void setup_window_controls(::game& ctx)
{
	// Toggle fullscreen
	ctx.event_subscriptions.emplace_back
	(
		ctx.fullscreen_action.get_activated_channel().subscribe
		(
			[&ctx](const auto&)
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
			[&ctx](const auto&)
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
			[&](const auto&)
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
					
					ctx.ui_canvas->get_scene().add_object(*ctx.command_line_text);
					ctx.ui_canvas->get_scene().add_object(*ctx.shell_buffer_text);

					const auto& text_box_bounds = ctx.command_line_text->get_bounds();

					const auto& viewport_size = ctx.window->get_viewport_size();
					geom::rectangle<int> text_box_rect;
					text_box_rect.min.x() = static_cast<int>(text_box_bounds.max.x());
					text_box_rect.max.x() = static_cast<int>(viewport_size.x());
					text_box_rect.min.y() = static_cast<int>(viewport_size.y() - text_box_bounds.max.y());
					text_box_rect.max.y() = static_cast<int>(viewport_size.y() - text_box_bounds.min.y());

					ctx.window->start_text_input(text_box_rect);
					enable_terminal_controls(ctx);
				}
				else
				{
					ctx.ui_canvas->get_scene().remove_object(*ctx.command_line_text);
					ctx.ui_canvas->get_scene().remove_object(*ctx.shell_buffer_text);
					ctx.window->stop_text_input();
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
