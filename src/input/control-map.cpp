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

#include "input/control-map.hpp"
#include <algorithm>
#include <cmath>
#include <type_traits>
#include <utility>

namespace input {

void control_map::connect(::event::queue& queue)
{
	subscriptions.emplace_back(queue.subscribe<gamepad_axis_moved_event>(std::bind_front(&control_map::handle_gamepad_axis_moved, this)));
	subscriptions.emplace_back(queue.subscribe<gamepad_button_pressed_event>(std::bind_front(&control_map::handle_gamepad_button_pressed, this)));
	subscriptions.emplace_back(queue.subscribe<gamepad_button_released_event>(std::bind_front(&control_map::handle_gamepad_button_released, this)));
	subscriptions.emplace_back(queue.subscribe<key_pressed_event>(std::bind_front(&control_map::handle_key_pressed, this)));
	subscriptions.emplace_back(queue.subscribe<key_released_event>(std::bind_front(&control_map::handle_key_released, this)));
	subscriptions.emplace_back(queue.subscribe<mouse_button_pressed_event>(std::bind_front(&control_map::handle_mouse_button_pressed, this)));
	subscriptions.emplace_back(queue.subscribe<mouse_button_released_event>(std::bind_front(&control_map::handle_mouse_button_released, this)));
	subscriptions.emplace_back(queue.subscribe<mouse_moved_event>(std::bind_front(&control_map::handle_mouse_moved, this)));
	subscriptions.emplace_back(queue.subscribe<mouse_scrolled_event>(std::bind_front(&control_map::handle_mouse_scrolled, this)));
}

void control_map::disconnect()
{
	subscriptions.clear();
}

void control_map::add_mapping(control& control, const mapping& mapping)
{
	switch (mapping.get_mapping_type())
	{
		case mapping_type::gamepad_axis:
			add_mapping(control, static_cast<const gamepad_axis_mapping&>(mapping));
			break;
		
		case mapping_type::gamepad_button:
			add_mapping(control, static_cast<const gamepad_button_mapping&>(mapping));
			break;
		
		case mapping_type::key:
			add_mapping(control, static_cast<const key_mapping&>(mapping));
			break;
		
		case mapping_type::mouse_button:
			add_mapping(control, static_cast<const mouse_button_mapping&>(mapping));
			break;
		
		case mapping_type::mouse_motion:
			add_mapping(control, static_cast<const mouse_motion_mapping&>(mapping));
			break;
		
		case mapping_type::mouse_scroll:
			add_mapping(control, static_cast<const mouse_scroll_mapping&>(mapping));
			break;
		
		default:
			//std::unreachable();
			break;
	}
}

void control_map::add_mapping(control& control, gamepad_axis_mapping&& mapping)
{
	gamepad_axis_mappings.emplace_back(&control, mapping);
}

void control_map::add_mapping(control& control, gamepad_button_mapping&& mapping)
{
	gamepad_button_mappings.emplace_back(&control, mapping);
}

void control_map::add_mapping(control& control, key_mapping&& mapping)
{
	key_mappings.emplace_back(&control, mapping);
}

void control_map::add_mapping(control& control, mouse_button_mapping&& mapping)
{
	mouse_button_mappings.emplace_back(&control, mapping);
}

void control_map::add_mapping(control& control, mouse_motion_mapping&& mapping)
{
	mouse_motion_mappings.emplace_back(&control, mapping);
}

void control_map::add_mapping(control& control, mouse_scroll_mapping&& mapping)
{
	mouse_scroll_mappings.emplace_back(&control, mapping);
}

void control_map::remove_mappings(control& control, mapping_type type)
{
	auto predicate = [&](const auto& tuple) -> bool
	{
		return std::get<0>(tuple) == &control;
	};
	
	switch (type)
	{
		case mapping_type::gamepad_axis:
			std::erase_if(gamepad_axis_mappings, predicate);
			break;
		
		case mapping_type::gamepad_button:
			std::erase_if(gamepad_button_mappings, predicate);
			break;
		
		case mapping_type::key:
			std::erase_if(key_mappings, predicate);
			break;
		
		case mapping_type::mouse_button:
			std::erase_if(mouse_button_mappings, predicate);
			break;
		
		case mapping_type::mouse_motion:
			std::erase_if(mouse_motion_mappings, predicate);
			break;
		
		case mapping_type::mouse_scroll:
			std::erase_if(mouse_scroll_mappings, predicate);
			break;
		
		default:
			//std::unreachable();
			break;
	}
}

void control_map::remove_mappings(control& control)
{
	auto predicate = [&](const auto& tuple) -> bool
	{
		return std::get<0>(tuple) == &control;
	};
	
	std::erase_if(gamepad_axis_mappings, predicate);
	std::erase_if(gamepad_button_mappings, predicate);
	std::erase_if(key_mappings, predicate);
	std::erase_if(mouse_button_mappings, predicate);
	std::erase_if(mouse_motion_mappings, predicate);
	std::erase_if(mouse_scroll_mappings, predicate);
}

void control_map::remove_mappings()
{
	gamepad_axis_mappings.clear();
	gamepad_button_mappings.clear();
	key_mappings.clear();
	mouse_button_mappings.clear();
	mouse_motion_mappings.clear();
	mouse_scroll_mappings.clear();
}

void control_map::handle_gamepad_axis_moved(const gamepad_axis_moved_event& event)
{
	for (const auto& [control, mapping]: gamepad_axis_mappings)
	{
		if (mapping.axis == event.axis &&
			(!mapping.gamepad || mapping.gamepad == event.gamepad))
		{
			if (std::signbit(event.position) == mapping.direction)
			{
				control->evaluate(std::abs(event.position));
			}
			else
			{
				control->evaluate(0.0f);
			}
		}
	}
}

void control_map::handle_gamepad_button_pressed(const gamepad_button_pressed_event& event)
{
	for (const auto& [control, mapping]: gamepad_button_mappings)
	{
		if (mapping.button == event.button &&
			(!mapping.gamepad || mapping.gamepad == event.gamepad))
		{
			control->evaluate(1.0f);
		}
	}
}

void control_map::handle_gamepad_button_released(const gamepad_button_released_event& event)
{
	for (const auto& [control, mapping]: gamepad_button_mappings)
	{
		if (mapping.button == event.button &&
			(!mapping.gamepad || mapping.gamepad == event.gamepad))
		{
			control->evaluate(0.0f);
		}
	}
}

void control_map::handle_key_pressed(const key_pressed_event& event)
{
	for (const auto& [control, mapping]: key_mappings)
	{
		if (mapping.scancode == event.scancode &&
			(!mapping.keyboard || mapping.keyboard == event.keyboard))
		{
			control->evaluate(1.0f);
		}
	}
}

void control_map::handle_key_released(const key_released_event& event)
{
	for (const auto& [control, mapping]: key_mappings)
	{
		if (mapping.scancode == event.scancode &&
			(!mapping.keyboard || mapping.keyboard == event.keyboard))
		{
			control->evaluate(0.0f);
		}
	}
}

void control_map::handle_mouse_moved(const mouse_moved_event& event)
{
	for (const auto& [control, mapping]: mouse_motion_mappings)
	{
		if (!mapping.mouse || mapping.mouse == event.mouse)
		{
			const float difference = static_cast<float>(event.difference[static_cast<std::underlying_type_t<mouse_motion_axis>>(mapping.axis)]);
			
			if (difference && std::signbit(difference) == mapping.direction)
			{
				control->evaluate(std::abs(difference));
				control->evaluate(0.0f);
			}
		}
	}
}

void control_map::handle_mouse_scrolled(const mouse_scrolled_event& event)
{
	for (const auto& [control, mapping]: mouse_scroll_mappings)
	{
		if (!mapping.mouse || mapping.mouse == event.mouse)
		{
			const auto velocity = event.velocity[static_cast<std::underlying_type_t<mouse_scroll_axis>>(mapping.axis)];
			
			if (velocity && std::signbit(velocity) == mapping.direction)
			{
				control->evaluate(std::abs(velocity));
				control->evaluate(0.0f);
			}
		}
	}
}

void control_map::handle_mouse_button_pressed(const mouse_button_pressed_event& event)
{
	for (const auto& [control, mapping]: mouse_button_mappings)
	{
		if (mapping.button == event.button &&
			(!mapping.mouse || mapping.mouse == event.mouse))
		{
			control->evaluate(1.0f);
		}
	}
}

void control_map::handle_mouse_button_released(const mouse_button_released_event& event)
{
	for (const auto& [control, mapping]: mouse_button_mappings)
	{
		if (mapping.button == event.button &&
			(!mapping.mouse || mapping.mouse == event.mouse))
		{
			control->evaluate(0.0f);
		}
	}
}

} // namespace input
