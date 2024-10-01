// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/input/action-map.hpp>
#include <algorithm>
#include <cmath>
#include <type_traits>
#include <utility>

namespace input {

void action_map::enable()
{
	if (!m_enabled)
	{
		if (m_event_dispatcher)
		{
			subscribe();
		}
		
		m_enabled = true;
	}
}

void action_map::disable()
{
	if (m_enabled)
	{
		if (m_event_dispatcher)
		{
			unsubscribe();
		}
		
		m_enabled = false;
	}
}

void action_map::reset()
{
	for (auto action: m_actions)
	{
		action->reset();
	}
}

void action_map::set_event_dispatcher(event::dispatcher* dispatcher)
{
	if (m_event_dispatcher != dispatcher)
	{
		if (m_enabled)
		{
			if (m_event_dispatcher)
			{
				unsubscribe();
			}
			
			m_event_dispatcher = dispatcher;
			
			if (m_event_dispatcher)
			{
				subscribe();
			}
		}
		else
		{
			m_event_dispatcher = dispatcher;
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
	m_gamepad_axis_mappings.emplace_back(&action, std::move(mapping));
	m_actions.emplace(&action);
}

void action_map::add_gamepad_button_mapping(action& action, gamepad_button_mapping mapping)
{
	m_gamepad_button_mappings.emplace_back(&action, std::move(mapping));
	m_actions.emplace(&action);
}

void action_map::add_key_mapping(action& action, key_mapping mapping)
{
	m_key_mappings.emplace_back(&action, std::move(mapping));
	m_actions.emplace(&action);
}

void action_map::add_mouse_button_mapping(action& action, mouse_button_mapping mapping)
{
	m_mouse_button_mappings.emplace_back(&action, std::move(mapping));
	m_actions.emplace(&action);
}

void action_map::add_mouse_motion_mapping(action& action, mouse_motion_mapping mapping)
{
	m_mouse_motion_mappings.emplace_back(&action, std::move(mapping));
	m_actions.emplace(&action);
}

void action_map::add_mouse_scroll_mapping(action& action, mouse_scroll_mapping mapping)
{
	m_mouse_scroll_mappings.emplace_back(&action, std::move(mapping));
	m_actions.emplace(&action);
}

void action_map::remove_mappings(action& action, mapping_type type)
{
	auto predicate = [&](const auto& tuple) -> bool
	{
		return std::get<0>(tuple) == &action;
	};
	
	switch (type)
	{
		case mapping_type::gamepad_axis:
			std::erase_if(m_gamepad_axis_mappings, predicate);
			break;
		
		case mapping_type::gamepad_button:
			std::erase_if(m_gamepad_button_mappings, predicate);
			break;
		
		case mapping_type::key:
			std::erase_if(m_key_mappings, predicate);
			break;
		
		case mapping_type::mouse_button:
			std::erase_if(m_mouse_button_mappings, predicate);
			break;
		
		case mapping_type::mouse_motion:
			std::erase_if(m_mouse_motion_mappings, predicate);
			break;
		
		case mapping_type::mouse_scroll:
			std::erase_if(m_mouse_scroll_mappings, predicate);
			break;
		
		default:
			//std::unreachable();
			break;
	}
	
	for (const auto& entry: m_gamepad_axis_mappings)
	{
		if (std::get<0>(entry) == &action)
		{
			return;
		}
	}
	for (const auto& entry: m_gamepad_button_mappings)
	{
		if (std::get<0>(entry) == &action)
		{
			return;
		}
	}
	for (const auto& entry: m_key_mappings)
	{
		if (std::get<0>(entry) == &action)
		{
			return;
		}
	}
	for (const auto& entry: m_mouse_button_mappings)
	{
		if (std::get<0>(entry) == &action)
		{
			return;
		}
	}
	for (const auto& entry: m_mouse_motion_mappings)
	{
		if (std::get<0>(entry) == &action)
		{
			return;
		}
	}
	for (const auto& entry: m_mouse_scroll_mappings)
	{
		if (std::get<0>(entry) == &action)
		{
			return;
		}
	}
	
	m_actions.erase(&action);
}

void action_map::remove_mappings(action& action)
{
	auto predicate = [&](const auto& tuple) -> bool
	{
		return std::get<0>(tuple) == &action;
	};
	
	std::erase_if(m_gamepad_axis_mappings, predicate);
	std::erase_if(m_gamepad_button_mappings, predicate);
	std::erase_if(m_key_mappings, predicate);
	std::erase_if(m_mouse_button_mappings, predicate);
	std::erase_if(m_mouse_motion_mappings, predicate);
	std::erase_if(m_mouse_scroll_mappings, predicate);
	
	m_actions.erase(&action);
}

void action_map::remove_mappings()
{
	m_gamepad_axis_mappings.clear();
	m_gamepad_button_mappings.clear();
	m_key_mappings.clear();
	m_mouse_button_mappings.clear();
	m_mouse_motion_mappings.clear();
	m_mouse_scroll_mappings.clear();
	
	m_actions.clear();
}

void action_map::handle_gamepad_axis_moved(const gamepad_axis_moved_event& event)
{
	for (const auto& [action, mapping]: m_gamepad_axis_mappings)
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
	for (const auto& [action, mapping]: m_gamepad_button_mappings)
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
	for (const auto& [action, mapping]: m_gamepad_button_mappings)
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
	for (const auto& [action, mapping]: m_key_mappings)
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
	for (const auto& [action, mapping]: m_key_mappings)
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
	for (const auto& [action, mapping]: m_mouse_motion_mappings)
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
	for (const auto& [action, mapping]: m_mouse_scroll_mappings)
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
	for (const auto& [action, mapping]: m_mouse_button_mappings)
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
	for (const auto& [action, mapping]: m_mouse_button_mappings)
	{
		if (mapping.button == event.button &&
			(!mapping.mouse || mapping.mouse == event.mouse))
		{
			action->evaluate(0.0f);
		}
	}
}

void action_map::handle_update([[maybe_unused]] const update_event& event)
{
	for (const auto* action: m_actions)
	{
		action->update();
	}
}

std::vector<gamepad_axis_mapping> action_map::get_gamepad_axis_mappings(const action& action) const
{
	std::vector<gamepad_axis_mapping> mappings;
	
	for (const auto& [mapped_action, mapping]: m_gamepad_axis_mappings)
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
	
	for (const auto& [mapped_action, mapping]: m_gamepad_button_mappings)
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
	
	for (const auto& [mapped_action, mapping]: m_key_mappings)
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
	
	for (const auto& [mapped_action, mapping]: m_mouse_button_mappings)
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
	
	for (const auto& [mapped_action, mapping]: m_mouse_motion_mappings)
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
	
	for (const auto& [mapped_action, mapping]: m_mouse_scroll_mappings)
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
	m_subscriptions.emplace_back(m_event_dispatcher->subscribe<gamepad_axis_moved_event>(std::bind_front(&action_map::handle_gamepad_axis_moved, this)));
	m_subscriptions.emplace_back(m_event_dispatcher->subscribe<gamepad_button_pressed_event>(std::bind_front(&action_map::handle_gamepad_button_pressed, this)));
	m_subscriptions.emplace_back(m_event_dispatcher->subscribe<gamepad_button_released_event>(std::bind_front(&action_map::handle_gamepad_button_released, this)));
	m_subscriptions.emplace_back(m_event_dispatcher->subscribe<key_pressed_event>(std::bind_front(&action_map::handle_key_pressed, this)));
	m_subscriptions.emplace_back(m_event_dispatcher->subscribe<key_released_event>(std::bind_front(&action_map::handle_key_released, this)));
	m_subscriptions.emplace_back(m_event_dispatcher->subscribe<mouse_button_pressed_event>(std::bind_front(&action_map::handle_mouse_button_pressed, this)));
	m_subscriptions.emplace_back(m_event_dispatcher->subscribe<mouse_button_released_event>(std::bind_front(&action_map::handle_mouse_button_released, this)));
	m_subscriptions.emplace_back(m_event_dispatcher->subscribe<mouse_moved_event>(std::bind_front(&action_map::handle_mouse_moved, this)));
	m_subscriptions.emplace_back(m_event_dispatcher->subscribe<mouse_scrolled_event>(std::bind_front(&action_map::handle_mouse_scrolled, this)));
	m_subscriptions.emplace_back(m_event_dispatcher->subscribe<update_event>(std::bind_front(&action_map::handle_update, this)));
}

void action_map::unsubscribe()
{
	m_subscriptions.clear();
}

} // namespace input
