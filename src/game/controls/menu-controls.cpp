// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/menu.hpp"
#include "game/strings.hpp"
#include "game/controls.hpp"
import engine.ui.range;
import engine.config;

void setup_menu_controls(::game& ctx)
{
	// Setup menu controls
	ctx.menu_action_subscriptions.emplace_back
	(
		ctx.menu_up_action.get_activated_channel().subscribe
		(
			[&ctx](const auto&)
			{
				if (auto focus_current = ctx.ui_canvas->find_focus())
				{
					auto focus_up = focus_current->get_focus_up().lock();
					if (focus_up)
					{
						focus_up->set_focus(true);
					}
				}
			}
		)
	);
	ctx.menu_action_subscriptions.emplace_back
	(
		ctx.menu_down_action.get_activated_channel().subscribe
		(
			[&ctx](const auto&)
			{
				if (auto focus_current = ctx.ui_canvas->find_focus())
				{
					auto focus_down = focus_current->get_focus_down().lock();
					if (focus_down)
					{
						focus_down->set_focus(true);
					}
				}
			}
		)
	);
	ctx.menu_action_subscriptions.emplace_back
	(
		ctx.menu_left_action.get_activated_channel().subscribe
		(
			[&ctx](const auto&)
			{
				if (auto focus_current = ctx.ui_canvas->find_focus())
				{
					if (focus_current->get_type() == ui::element_type::range)
					{
						auto& range = static_cast<ui::range&>(*focus_current);

						if (range.is_boolean())
						{
							range.set_value(!range.get_value());
						}
						else
						{
							range.decrement();
						}
					}
				}
			}
		)
	);
	ctx.menu_action_subscriptions.emplace_back
	(
		ctx.menu_right_action.get_activated_channel().subscribe
		(
			[&ctx](const auto&)
			{
				if (auto focus_current = ctx.ui_canvas->find_focus())
				{
					if (focus_current->get_type() == ui::element_type::range)
					{
						auto& range = static_cast<ui::range&>(*focus_current);

						if (range.is_boolean())
						{
							range.set_value(!range.get_value());
						}
						else
						{
							range.increment();
						}
					}
				}
			}
		)
	);
	ctx.menu_action_subscriptions.emplace_back
	(
		ctx.menu_select_action.get_activated_channel().subscribe
		(
			[&ctx](const auto&)
			{
				if (auto focus = ctx.ui_canvas->find_focus())
				{
					focus->click(input::mouse_button::left);
				}
			}
		)
	);
	ctx.menu_action_subscriptions.emplace_back
	(
		ctx.menu_back_action.get_activated_channel().subscribe
		(
			// HACK: Find the back or resume UI element based on its string and click it
			[&ctx](const auto&)
			{
				const auto back_string = get_string(ctx, "back");
				const auto resume_string = get_string(ctx, "pause_menu_resume");

				ctx.m_root_menu_container->visit_descendants
				(
					[&](auto& descendant)
					{
						if (descendant.get_type() == ui::element_type::label)
						{
							auto& label = static_cast<ui::label&>(descendant);
							if (label.get_text() == back_string || label.get_text() == resume_string)
							{
								label.click(input::mouse_button::left);
							}
						}
					}
				);
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
