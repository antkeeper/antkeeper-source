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

#include <engine/input/action-map.hpp>
#include <algorithm>
#include <cmath>
#include <type_traits>
#include <utility>

namespace input {

void action_map::enable()
{
	if (!enabled)
	{
		if (event_dispatcher)
		{
			subscribe();
		}
		
		enabled = true;
	}
}

void action_map::disable()
{
	if (enabled)
	{
		if (event_dispatcher)
		{
			unsubscribe();
		}
		
		enabled = false;
	}
}

void action_map::set_event_dispatcher(event::dispatcher* dispatcher)
{
	if (event_dispatcher != dispatcher)
	{
		if (enabled)
		{
			if (event_dispatcher)
			{
				unsubscribe();
			}
			
			event_dispatcher = dispatcher;
			
			if (event_dispatcher)
			{
				subscribe();
			}
		}
		else
		{
			event_dispatcher = dispatcher;
		}
	}
}

void action_map::add_mapping(action& action, const mapping& mapping)
{
	switch (mapping.get_mapping_type())
	{
		case mapping_type::gamepad_axis:
			add_gamepad_axis_mapping(action, static_cast<const gamepad_axis_mapping&>(mapping));
			break;
		
		case mapping_type::gamepad_button:
			add_gamepad_button_mapping(action, static_cast<const gamepad_button_mapping&>(mapping));
			break;
		
		case mapping_type::key:
			add_key_mapping(action, static_cast<const key_mapping&>(mapping));
			break;
		
		case mapping_type::mouse_button:
			add_mouse_button_mapping(action, static_cast<const mouse_button_mapping&>(mapping));
			break;
		
		case mapping_type::mouse_motion:
			add_mouse_motion_mapping(action, static_cast<const mouse_motion_mapping&>(mapping));
			break;
		
		case mapping_type::mouse_scroll:
			add_mouse_scroll_mapping(action, static_cast<const mouse_scroll_mapping&>(mapping));
			break;
		
		default:
			//std::unreachable();
			break;
	}
}

void action_map::add_gamepad_axis_mapping(action& action, gamepad_axis_mapping mapping)
{
	gamepad_axis_mappings.emplace_back(&action, std::move(mapping));
}

void action_map::add_gamepad_button_mapping(action& action, gamepad_button_mapping mapping)
{
	gamepad_button_mappings.emplace_back(&action, std::move(mapping));
}

void action_map::add_key_mapping(action& action, key_mapping mapping)
{
	key_mappings.emplace_back(&action, std::move(mapping));
}

void action_map::add_mouse_button_mapping(action& action, mouse_button_mapping mapping)
{
	mouse_button_mappings.emplace_back(&action, std::move(mapping));
}

void action_map::add_mouse_motion_mapping(action& action, mouse_motion_mapping mapping)
{
	mouse_motion_mappings.emplace_back(&action, std::move(mapping));
}

void action_map::add_mouse_scroll_mapping(action& action, mouse_scroll_mapping mapping)
{
	mouse_scroll_mappings.emplace_back(&action, std::move(mapping));
}

void action_map::remove_mappings(const action& action, mapping_type type)
{
	auto predicate = [&](const auto& tuple) -> bool
	{
		return std::get<0>(tuple) == &action;
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

void action_map::remove_mappings(const action& action)
{
	auto predicate = [&](const auto& tuple) -> bool
	{
		return std::get<0>(tuple) == &action;
	};
	
	std::erase_if(gamepad_axis_mappings, predicate);
	std::erase_if(gamepad_button_mappings, predicate);
	std::erase_if(key_mappings, predicate);
	std::erase_if(mouse_button_mappings, predicate);
	std::erase_if(mouse_motion_mappings, predicate);
	std::erase_if(mouse_scroll_mappings, predicate);
}

void action_map::remove_mappings()
{
	gamepad_axis_mappings.clear();
	gamepad_button_mappings.clear();
	key_mappings.clear();
	mouse_button_mappings.clear();
	mouse_motion_mappings.clear();
	mouse_scroll_mappings.clear();
}

void action_map::handle_gamepad_axis_moved(const gamepad_axis_moved_event& event)
{
	for (const auto& [action, mapping]: gamepad_axis_mappings)
	{
		if (mapping.axis == event.axis &&
			(!mapping.gamepad || mapping.gamepad == event.gamepad))
		{
			if (std::signbit(event.position) == mapping.direction)
			{
				action->evaluate(std::abs(event.position));
			}
			else
			{
				action->evaluate(0.0f);
			}
		}
	}
}

void action_map::handle_gamepad_button_pressed(const gamepad_button_pressed_event& event)
{
	for (const auto& [action, mapping]: gamepad_button_mappings)
	{
		if (mapping.button == event.button &&
			(!mapping.gamepad || mapping.gamepad == event.gamepad))
		{
			action->evaluate(1.0f);
		}
	}
}

void action_map::handle_gamepad_button_released(const gamepad_button_released_event& event)
{
	for (const auto& [action, mapping]: gamepad_button_mappings)
	{
		if (mapping.button == event.button &&
			(!mapping.gamepad || mapping.gamepad == event.gamepad))
		{
			action->evaluate(0.0f);
		}
	}
}

void action_map::handle_key_pressed(const key_pressed_event& event)
{
	for (const auto& [action, mapping]: key_mappings)
	{
		if (mapping.scancode == event.scancode &&
			(!mapping.keyboard || mapping.keyboard == event.keyboard) &&
			(!mapping.modifiers || (mapping.modifiers & event.modifiers)))
		{
			if (!event.repeat)
			{
				action->evaluate(1.0f);
			}
			else if (mapping.repeat)
			{
				action->evaluate(0.0f);
				action->evaluate(1.0f);
			}
		}
	}
}

void action_map::handle_key_released(const key_released_event& event)
{
	for (const auto& [action, mapping]: key_mappings)
	{
		if (mapping.scancode == event.scancode &&
			(!mapping.keyboard || mapping.keyboard == event.keyboard))
		{
			action->evaluate(0.0f);
		}
	}
}

void action_map::handle_mouse_moved(const mouse_moved_event& event)
{
	for (const auto& [action, mapping]: mouse_motion_mappings)
	{
		if (!mapping.mouse || mapping.mouse == event.mouse)
		{
			const float difference = static_cast<float>(event.difference[static_cast<std::underlying_type_t<mouse_motion_axis>>(mapping.axis)]);
			
			if (difference && std::signbit(difference) == mapping.direction)
			{
				action->evaluate(std::abs(difference));
				action->evaluate(0.0f);
			}
		}
	}
}

void action_map::handle_mouse_scrolled(const mouse_scrolled_event& event)
{
	for (const auto& [action, mapping]: mouse_scroll_mappings)
	{
		if (!mapping.mouse || mapping.mouse == event.mouse)
		{
			const auto velocity = event.velocity[static_cast<std::underlying_type_t<mouse_scroll_axis>>(mapping.axis)];
			
			if (velocity && std::signbit(velocity) == mapping.direction)
			{
				action->evaluate(std::abs(velocity));
				action->evaluate(0.0f);
			}
		}
	}
}

void action_map::handle_mouse_button_pressed(const mouse_button_pressed_event& event)
{
	for (const auto& [action, mapping]: mouse_button_mappings)
	{
		if (mapping.button == event.button &&
			(!mapping.mouse || mapping.mouse == event.mouse))
		{
			action->evaluate(1.0f);
		}
	}
}

void action_map::handle_mouse_button_released(const mouse_button_released_event& event)
{
	for (const auto& [action, mapping]: mouse_button_mappings)
	{
		if (mapping.button == event.button &&
			(!mapping.mouse || mapping.mouse == event.mouse))
		{
			action->evaluate(0.0f);
		}
	}
}

std::vector<gamepad_axis_mapping> action_map::get_gamepad_axis_mappings(const action& action) const
{
	std::vector<gamepad_axis_mapping> mappings;
	
	for (const auto& [mapped_action, mapping]: gamepad_axis_mappings)
	{
		if (mapped_action == &action)
		{
			mappings.emplace_back(mapping);
		}
	}
	
	return mappings;
}
	
std::vector<gamepad_button_mapping> action_map::get_gamepad_button_mappings(const action& action) const
{
	std::vector<gamepad_button_mapping> mappings;
	
	for (const auto& [mapped_action, mapping]: gamepad_button_mappings)
	{
		if (mapped_action == &action)
		{
			mappings.emplace_back(mapping);
		}
	}
	
	return mappings;
}

std::vector<key_mapping> action_map::get_key_mappings(const action& action) const
{
	std::vector<key_mapping> mappings;
	
	for (const auto& [mapped_action, mapping]: key_mappings)
	{
		if (mapped_action == &action)
		{
			mappings.emplace_back(mapping);
		}
	}
	
	return mappings;
}

std::vector<mouse_button_mapping> action_map::get_mouse_button_mappings(const action& action) const
{
	std::vector<mouse_button_mapping> mappings;
	
	for (const auto& [mapped_action, mapping]: mouse_button_mappings)
	{
		if (mapped_action == &action)
		{
			mappings.emplace_back(mapping);
		}
	}
	
	return mappings;
}

std::vector<mouse_motion_mapping> action_map::get_mouse_motion_mappings(const action& action) const
{
	std::vector<mouse_motion_mapping> mappings;
	
	for (const auto& [mapped_action, mapping]: mouse_motion_mappings)
	{
		if (mapped_action == &action)
		{
			mappings.emplace_back(mapping);
		}
	}
	
	return mappings;
}

std::vector<mouse_scroll_mapping> action_map::get_mouse_scroll_mappings(const action& action) const
{
	std::vector<mouse_scroll_mapping> mappings;
	
	for (const auto& [mapped_action, mapping]: mouse_scroll_mappings)
	{
		if (mapped_action == &action)
		{
			mappings.emplace_back(mapping);
		}
	}
	
	return mappings;
}

void action_map::subscribe()
{
	subscriptions.emplace_back(event_dispatcher->subscribe<gamepad_axis_moved_event>(std::bind_front(&action_map::handle_gamepad_axis_moved, this)));
	subscriptions.emplace_back(event_dispatcher->subscribe<gamepad_button_pressed_event>(std::bind_front(&action_map::handle_gamepad_button_pressed, this)));
	subscriptions.emplace_back(event_dispatcher->subscribe<gamepad_button_released_event>(std::bind_front(&action_map::handle_gamepad_button_released, this)));
	subscriptions.emplace_back(event_dispatcher->subscribe<key_pressed_event>(std::bind_front(&action_map::handle_key_pressed, this)));
	subscriptions.emplace_back(event_dispatcher->subscribe<key_released_event>(std::bind_front(&action_map::handle_key_released, this)));
	subscriptions.emplace_back(event_dispatcher->subscribe<mouse_button_pressed_event>(std::bind_front(&action_map::handle_mouse_button_pressed, this)));
	subscriptions.emplace_back(event_dispatcher->subscribe<mouse_button_released_event>(std::bind_front(&action_map::handle_mouse_button_released, this)));
	subscriptions.emplace_back(event_dispatcher->subscribe<mouse_moved_event>(std::bind_front(&action_map::handle_mouse_moved, this)));
	subscriptions.emplace_back(event_dispatcher->subscribe<mouse_scrolled_event>(std::bind_front(&action_map::handle_mouse_scrolled, this)));
}

void action_map::unsubscribe()
{
	subscriptions.clear();
}

} // namespace input
