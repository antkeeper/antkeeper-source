// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/controls.hpp"
#include "game/menu.hpp"
#include <engine/config.hpp>

void setup_menu_controls(::game& ctx)
{
	// Setup menu controls
	ctx.menu_action_subscriptions.emplace_back
	(
		ctx.menu_up_action.get_activated_channel().subscribe
		(
			[&ctx]([[maybe_unused]] const auto& event)
			{
				ctx.menu_up_sound->play();

				--(*ctx.menu_item_index);
				if (*ctx.menu_item_index < 0)
					*ctx.menu_item_index = static_cast<int>(ctx.menu_item_texts.size()) - 1;
				
				::menu::update_text_color(ctx);
			}
		)
	);
	ctx.menu_action_subscriptions.emplace_back
	(
		ctx.menu_down_action.get_activated_channel().subscribe
		(
			[&ctx]([[maybe_unused]] const auto& event)
			{
				ctx.menu_down_sound->play();

				++(*ctx.menu_item_index);
				if (*ctx.menu_item_index >= ctx.menu_item_texts.size())
					*ctx.menu_item_index = 0;
				
				::menu::update_text_color(ctx);
			}
		)
	);
	ctx.menu_action_subscriptions.emplace_back
	(
		ctx.menu_left_action.get_activated_channel().subscribe
		(
			[&ctx]([[maybe_unused]] const auto& event)
			{
				auto callback = ctx.menu_left_callbacks[*ctx.menu_item_index];
				if (callback != nullptr)
					callback();
			}
		)
	);
	ctx.menu_action_subscriptions.emplace_back
	(
		ctx.menu_right_action.get_activated_channel().subscribe
		(
			[&ctx]([[maybe_unused]] const auto& event)
			{
				auto callback = ctx.menu_right_callbacks[*ctx.menu_item_index];
				if (callback != nullptr)
					callback();
			}
		)
	);
	ctx.menu_action_subscriptions.emplace_back
	(
		ctx.menu_select_action.get_activated_channel().subscribe
		(
			[&ctx]([[maybe_unused]] const auto& event)
			{
				const auto& callback = ctx.menu_select_callbacks[*ctx.menu_item_index];
				if (callback != nullptr)
					callback();
			}
		)
	);
	ctx.menu_action_subscriptions.emplace_back
	(
		ctx.menu_back_action.get_activated_channel().subscribe
		(
			[&ctx]([[maybe_unused]] const auto& event)
			{
				if (ctx.menu_back_callback != nullptr)
					ctx.menu_back_callback();
			}
		)
	);
	
	// Set activation threshold for menu navigation controls to mitigate drifting gamepad axes
	auto menu_action_threshold = [](float x) -> bool
	{
		return x > 0.5f;
	};
	ctx.menu_up_action.set_threshold_function(menu_action_threshold);
	ctx.menu_down_action.set_threshold_function(menu_action_threshold);
	ctx.menu_left_action.set_threshold_function(menu_action_threshold);
	ctx.menu_right_action.set_threshold_function(menu_action_threshold);
}

void enable_menu_controls(::game& ctx)
{
	ctx.menu_action_map.enable();
	
	// Function to select menu item at mouse position
	auto select_menu_item = [&ctx](const math::fvec2& mouse_position) -> bool
	{
		const float padding = config::menu_mouseover_padding * ctx.menu_font->get_metrics().size;
		
		for (std::size_t i = 0; i < ctx.menu_item_texts.size(); ++i)
		{
			auto [name, value] = ctx.menu_item_texts[i];
			
			const auto& name_bounds = name->get_bounds();
			float min_x = name_bounds.min.x();
			float min_y = name_bounds.min.y();
			float max_x = name_bounds.max.x();
			float max_y = name_bounds.max.y();
			
			if (value)
			{
				const auto& value_bounds = value->get_bounds();
				min_x = std::min<float>(min_x, value_bounds.min.x());
				min_y = std::min<float>(min_y, value_bounds.min.y());
				max_x = std::max<float>(max_x, value_bounds.max.x());
				max_y = std::max<float>(max_y, value_bounds.max.y());
			}
			
			min_x -= padding;
			min_y -= padding;
			max_x += padding;
			max_y += padding;
			
			const auto& viewport = ctx.window->get_viewport_size();
			const float x = mouse_position.x();
			const float y = static_cast<float>((viewport[1] - mouse_position.y() + 1));
			
			if (x >= min_x && x <= max_x)
			{
				if (y >= min_y && y <= max_y)
				{
					if (*ctx.menu_item_index > static_cast<int>(i))
					{
						ctx.menu_down_sound->play();
					}
					else if (*ctx.menu_item_index < static_cast<int>(i))
					{
						ctx.menu_up_sound->play();
					}
					
					*ctx.menu_item_index = static_cast<int>(i);
					::menu::update_text_color(ctx);
					return true;
				}
			}
		}
		
		return false;
	};
	
	// Enable menu mouse tracking
	ctx.menu_mouse_subscriptions.clear();
	ctx.menu_mouse_subscriptions.emplace_back
	(
		ctx.input_manager->get_event_dispatcher().subscribe<input::mouse_moved_event>
		(
			[&ctx, select_menu_item](const auto& event)
			{
				// Select menu item at mouse position (if any)
				select_menu_item(math::fvec2(event.position));
			}
		)
	);
	ctx.menu_mouse_subscriptions.emplace_back
	(
		ctx.input_manager->get_event_dispatcher().subscribe<input::mouse_button_pressed_event>
		(
			[&ctx, select_menu_item](const auto& event)
			{
				// Select menu item at mouse position (if any)
				if (select_menu_item(math::fvec2(event.position)))
				{
					// Determine appropriate menu item callback
					auto callback = ctx.menu_select_callbacks[*ctx.menu_item_index];
					if (event.button == input::mouse_button::left)
					{
						if (ctx.menu_left_callbacks[*ctx.menu_item_index])
						{
							callback = ctx.menu_left_callbacks[*ctx.menu_item_index];
						}
					}
					else if (event.button == input::mouse_button::right)
					{
						if (ctx.menu_right_callbacks[*ctx.menu_item_index])
						{
							callback = ctx.menu_right_callbacks[*ctx.menu_item_index];
						}
					}
					
					// Invoke menu item callback
					if (callback)
					{
						callback();
					}
				}
			}
		)
	);
}

void disable_menu_controls(::game& ctx)
{
	ctx.menu_action_map.disable();
	ctx.menu_up_action.reset();
	ctx.menu_down_action.reset();
	ctx.menu_left_action.reset();
	ctx.menu_right_action.reset();
	ctx.menu_select_action.reset();
	ctx.menu_back_action.reset();
	ctx.menu_modifier_action.reset();
	
	ctx.menu_mouse_subscriptions.clear();
}
