// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/controls.hpp"
#include "game/debug/shell.hpp"
#include <engine/input/keyboard-events.hpp>
#include <engine/debug/log.hpp>
#include <engine/type/unicode.hpp>

void setup_terminal_controls(::game& ctx)
{
	// Enter
	ctx.event_subscriptions.emplace_back
	(
		ctx.terminal_enter_action.get_activated_channel().subscribe
		(
			[&ctx]([[maybe_unused]] const auto& event)
			{
				ctx.shell->interpret(ctx.command_line);
				ctx.command_line.clear();
				ctx.command_line_text->set_content(ctx.shell->prompt());
				ctx.command_line_cursor = 0;
			}
		)
	);

	// Up
	ctx.event_subscriptions.emplace_back
	(
		ctx.terminal_up_action.get_activated_channel().subscribe
		(
			[&ctx]([[maybe_unused]] const auto& event)
			{
				if (ctx.command_line.empty())
				{
					const auto& history = ctx.shell->get_history();
					if (!history.empty())
					{
						ctx.command_line = history.back();
						ctx.command_line_cursor = ctx.command_line.length();
						ctx.command_line_text->set_content(ctx.shell->prompt() + ctx.command_line);
					}
				}
			}
		)
	);
	
	// Left
	ctx.event_subscriptions.emplace_back
	(
		ctx.terminal_left_action.get_activated_channel().subscribe
		(
			[&ctx]([[maybe_unused]] const auto& event)
			{
				if (!ctx.command_line.empty() && ctx.command_line_cursor)
				{
					auto substring_utf8 = ctx.command_line.substr(0, ctx.command_line_cursor);
					auto substring_utf32 = type::to_utf32(substring_utf8);
					if (!substring_utf32.empty())
					{
						ctx.command_line_cursor -= type::to_utf8(substring_utf32.substr(substring_utf32.length() - 1)).length();
					}
				}
			}
		)
	);
	
	// Right
	ctx.event_subscriptions.emplace_back
	(
		ctx.terminal_right_action.get_activated_channel().subscribe
		(
			[&ctx]([[maybe_unused]] const auto& event)
			{
				if (!ctx.command_line.empty() && ctx.command_line_cursor < ctx.command_line.length())
				{
					auto substring_utf8 = ctx.command_line.substr(ctx.command_line_cursor);
					auto substring_utf32 = type::to_utf32(substring_utf8);
					if (!substring_utf32.empty())
					{
						ctx.command_line_cursor += type::to_utf8(substring_utf32.substr(0, 1)).length();
					}
				}
			}
		)
	);
	
	// Backspace
	ctx.event_subscriptions.emplace_back
	(
		ctx.terminal_backspace_action.get_activated_channel().subscribe
		(
			[&ctx]([[maybe_unused]] const auto& event)
			{
				if (!ctx.command_line.empty() && ctx.command_line_cursor)
				{
					// Get UTF-8 substring from beginning of line to cursor
					auto substring_u8 = ctx.command_line.substr(0, ctx.command_line_cursor);
					
					// Convert substring from UTF-8 to UTF-32
					auto substring_u32 = type::to_utf32(substring_u8);
					if (!substring_u32.empty())
					{
						// Erase last character in the substring
						substring_u32.pop_back();
					}
					
					// Convert substring back to UTF-8
					substring_u8 = type::to_utf8(substring_u32);
					
					// Rebuild line
					ctx.command_line = substring_u8 + ctx.command_line.substr(ctx.command_line_cursor);
					
					// Update cursor position
					ctx.command_line_cursor = substring_u8.length();
					
					ctx.command_line_text->set_content(ctx.shell->prompt() + ctx.command_line);
				}
			}
		)
	);
	
	// Paste
	ctx.event_subscriptions.emplace_back
	(
		ctx.terminal_paste_action.get_activated_channel().subscribe
		(
			[&ctx]([[maybe_unused]] const auto& event)
			{
				auto text = ctx.input_manager->get_clipboard_text();
				if (!text.empty())
				{
					ctx.command_line.insert(ctx.command_line_cursor, text);
					ctx.command_line_cursor += text.length();
					ctx.command_line_text->set_content(ctx.shell->prompt() + ctx.command_line);
				}
			}
		)
	);
	
	// Clear line
	ctx.event_subscriptions.emplace_back
	(
		ctx.terminal_clear_line_action.get_activated_channel().subscribe
		(
			[&ctx]([[maybe_unused]] const auto& event)
			{
				if (!ctx.command_line.empty())
				{
					ctx.command_line.clear();
					ctx.command_line_cursor = 0;
					ctx.command_line_text->set_content(ctx.shell->prompt());
				}
			}
		)
	);
	
	// Input text
	ctx.event_subscriptions.emplace_back
	(
		ctx.input_manager->get_event_dispatcher().subscribe<input::text_input_event>
		(
			[&]([[maybe_unused]] const auto& event)
			{
				if (ctx.terminal_enabled && !ctx.toggle_terminal_action.is_active())
				{
					ctx.command_line.insert(ctx.command_line_cursor, event.text);
					ctx.command_line_cursor += event.text.length();
					ctx.command_line_text->set_content(ctx.shell->prompt() + ctx.command_line);
				}
			}
		)
	);
	
	// Edit text
	ctx.event_subscriptions.emplace_back
	(
		ctx.input_manager->get_event_dispatcher().subscribe<input::text_edit_event>
		(
			[&]([[maybe_unused]] const auto& event)
			{
				// debug::log_info("edit text: {} {} {}", event.text, event.position, event.length);
				
				ctx.command_line_text->set_content
				(
					ctx.shell->prompt() +
					ctx.command_line.substr(0, ctx.command_line_cursor) +
					event.text +
					ctx.command_line.substr(ctx.command_line_cursor)
				);
			}
		)
	);
}

void enable_terminal_controls(::game& ctx)
{
	ctx.terminal_action_map.enable();
}

void disable_terminal_controls(::game& ctx)
{
	ctx.terminal_action_map.disable();
	ctx.terminal_action_map.reset();
}
