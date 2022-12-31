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

#include "event-router.hpp"
#include "control.hpp"
#include "mapping.hpp"
#include "mouse.hpp"
#include "event/event-dispatcher.hpp"

namespace input {

event_router::event_router():
	event_dispatcher(nullptr)
{}

event_router::~event_router()
{
	remove_mappings();
	set_event_dispatcher(nullptr);
}

void event_router::add_mapping(const mapping& mapping)
{
	control* control = mapping.control;

	switch (mapping.get_type())
	{
		case mapping_type::key:
		{
			input::key_mapping* key_mapping = new input::key_mapping(static_cast<const input::key_mapping&>(mapping));
			key_mappings.push_back(key_mapping);
			controls[control].push_back(key_mapping);

			break;
		}

		case mapping_type::mouse_motion:
		{
			input::mouse_motion_mapping* mouse_motion_mapping = new input::mouse_motion_mapping(static_cast<const input::mouse_motion_mapping&>(mapping));
			mouse_motion_mappings.push_back(mouse_motion_mapping);
			controls[control].push_back(mouse_motion_mapping);

			break;
		}

		case mapping_type::mouse_wheel:
		{
			input::mouse_wheel_mapping* mouse_wheel_mapping = new input::mouse_wheel_mapping(static_cast<const input::mouse_wheel_mapping&>(mapping));
			mouse_wheel_mappings.push_back(mouse_wheel_mapping);
			controls[control].push_back(mouse_wheel_mapping);

			break;
		}

		case mapping_type::mouse_button:
		{
			input::mouse_button_mapping* mouse_button_mapping = new input::mouse_button_mapping(static_cast<const input::mouse_button_mapping&>(mapping));
			mouse_button_mappings.push_back(mouse_button_mapping);
			controls[control].push_back(mouse_button_mapping);

			break;
		}

		case mapping_type::gamepad_axis:
		{
			input::gamepad_axis_mapping* gamepad_axis_mapping = new input::gamepad_axis_mapping(static_cast<const input::gamepad_axis_mapping&>(mapping));
			gamepad_axis_mappings.push_back(gamepad_axis_mapping);
			controls[control].push_back(gamepad_axis_mapping);

			break;
		}

		case mapping_type::gamepad_button:
		{
			input::gamepad_button_mapping* gamepad_button_mapping = new input::gamepad_button_mapping(static_cast<const input::gamepad_button_mapping&>(mapping));
			gamepad_button_mappings.push_back(gamepad_button_mapping);
			controls[control].push_back(gamepad_button_mapping);

			break;
		}

		default:
			break;
	}
}

void event_router::remove_mappings(control* control)
{
	auto it = controls.find(control);
	if (it != controls.end())
	{
		for (mapping* mapping: it->second)
		{
			switch (mapping->get_type())
			{
				case mapping_type::key:
					key_mappings.remove(static_cast<key_mapping*>(mapping));
					break;

				case mapping_type::mouse_motion:
					mouse_motion_mappings.remove(static_cast<mouse_motion_mapping*>(mapping));
					break;

				case mapping_type::mouse_wheel:
					mouse_wheel_mappings.remove(static_cast<mouse_wheel_mapping*>(mapping));
					break;

				case mapping_type::mouse_button:
					mouse_button_mappings.remove(static_cast<mouse_button_mapping*>(mapping));
					break;

				case mapping_type::gamepad_axis:
					gamepad_axis_mappings.remove(static_cast<gamepad_axis_mapping*>(mapping));
					break;

				case mapping_type::gamepad_button:
					gamepad_button_mappings.remove(static_cast<gamepad_button_mapping*>(mapping));
					break;

				default:
					break;
			}

			delete mapping;
		}

		controls.erase(it);
	}
}

void event_router::remove_mappings(control* control, mapping_type type)
{
	auto it = controls.find(control);
	if (it != controls.end())
	{
		std::list<mapping*> flagged_mappings;
		
		for (mapping* mapping: it->second)
		{
			if (mapping->get_type() != type)
				continue;
			
			// Flag mapping for deletion
			flagged_mappings.push_back(mapping);
			
			switch (mapping->get_type())
			{
				case mapping_type::key:
					key_mappings.remove(static_cast<key_mapping*>(mapping));
					break;

				case mapping_type::mouse_motion:
					mouse_motion_mappings.remove(static_cast<mouse_motion_mapping*>(mapping));
					break;

				case mapping_type::mouse_wheel:
					mouse_wheel_mappings.remove(static_cast<mouse_wheel_mapping*>(mapping));
					break;

				case mapping_type::mouse_button:
					mouse_button_mappings.remove(static_cast<mouse_button_mapping*>(mapping));
					break;

				case mapping_type::gamepad_axis:
					gamepad_axis_mappings.remove(static_cast<gamepad_axis_mapping*>(mapping));
					break;

				case mapping_type::gamepad_button:
					gamepad_button_mappings.remove(static_cast<gamepad_button_mapping*>(mapping));
					break;

				default:
					break;
			}
		}
		
		// Delete flagged mappings
		for (mapping* mapping: flagged_mappings)
		{
			it->second.remove(mapping);
			delete mapping;
		}
	}
}

void event_router::set_event_dispatcher(::event_dispatcher* event_dispatcher)
{
	if (this->event_dispatcher)
	{
		this->event_dispatcher->unsubscribe<key_pressed_event>(this);
		this->event_dispatcher->unsubscribe<key_released_event>(this);
		this->event_dispatcher->unsubscribe<mouse_moved_event>(this);
		this->event_dispatcher->unsubscribe<mouse_wheel_scrolled_event>(this);
		this->event_dispatcher->unsubscribe<mouse_button_pressed_event>(this);
		this->event_dispatcher->unsubscribe<mouse_button_released_event>(this);
		this->event_dispatcher->unsubscribe<gamepad_axis_moved_event>(this);
		this->event_dispatcher->unsubscribe<gamepad_button_pressed_event>(this);
		this->event_dispatcher->unsubscribe<gamepad_button_released_event>(this);
	}

	this->event_dispatcher = event_dispatcher;

	if (event_dispatcher)
	{
		event_dispatcher->subscribe<key_pressed_event>(this);
		event_dispatcher->subscribe<key_released_event>(this);
		event_dispatcher->subscribe<mouse_moved_event>(this);
		event_dispatcher->subscribe<mouse_wheel_scrolled_event>(this);
		event_dispatcher->subscribe<mouse_button_pressed_event>(this);
		event_dispatcher->subscribe<mouse_button_released_event>(this);
		event_dispatcher->subscribe<gamepad_axis_moved_event>(this);
		event_dispatcher->subscribe<gamepad_button_pressed_event>(this);
		event_dispatcher->subscribe<gamepad_button_released_event>(this);
	}
}

void event_router::remove_mappings()
{
	for (auto it = controls.begin(); it != controls.end(); ++it)
	{
		for (mapping* mapping: it->second)
		{
			delete mapping;
		}
	}

	controls.clear();
	key_mappings.clear();
	mouse_motion_mappings.clear();
	mouse_wheel_mappings.clear();
	mouse_button_mappings.clear();
	gamepad_axis_mappings.clear();
	gamepad_button_mappings.clear();
}

const std::list<mapping*>* event_router::get_mappings(control* control) const
{
	auto it = controls.find(control);
	if (it == controls.end())
	{
		return nullptr;
	}

	return &it->second;
}

void event_router::handle_event(const key_pressed_event& event)
{
	for (const key_mapping* mapping: key_mappings)
	{
		if ((!mapping->keyboard || mapping->keyboard == event.keyboard) && mapping->scancode == event.scancode)
		{
			mapping->control->set_current_value(1.0f);
		}
	}
}

void event_router::handle_event(const key_released_event& event)
{
	for (const key_mapping* mapping: key_mappings)
	{
		if ((!mapping->keyboard || mapping->keyboard == event.keyboard) && mapping->scancode == event.scancode)
		{
			mapping->control->set_current_value(0.0f);
		}
	}
}

void event_router::handle_event(const mouse_moved_event& event)
{
	for (const mouse_motion_mapping* mapping: mouse_motion_mappings)
	{
		if (!mapping->mouse || mapping->mouse == event.mouse)
		{
			if (mapping->axis == mouse_motion_axis::negative_x && event.dx < 0)
			{
				mapping->control->set_temporary_value(mapping->control->get_current_value() - event.dx);
			}
			else if (mapping->axis == mouse_motion_axis::positive_x && event.dx > 0)
			{
				mapping->control->set_temporary_value(mapping->control->get_current_value() + event.dx);
			}
			else if (mapping->axis == mouse_motion_axis::negative_y && event.dy < 0)
			{
				mapping->control->set_temporary_value(mapping->control->get_current_value() - event.dy);
			}
			else if (mapping->axis == mouse_motion_axis::positive_y && event.dy > 0)
			{
				mapping->control->set_temporary_value(mapping->control->get_current_value() + event.dy);
			}
		}
	}
}

void event_router::handle_event(const mouse_wheel_scrolled_event& event)
{
	for (const mouse_wheel_mapping* mapping: mouse_wheel_mappings)
	{
		if (!mapping->mouse || mapping->mouse == event.mouse)
		{
			if (mapping->axis == mouse_wheel_axis::negative_x && event.x < 0)
			{
				mapping->control->set_temporary_value(mapping->control->get_current_value() - event.x);
			}
			else if (mapping->axis == mouse_wheel_axis::positive_x && event.x > 0)
			{
				mapping->control->set_temporary_value(mapping->control->get_current_value() + event.x);
			}
			else if (mapping->axis == mouse_wheel_axis::negative_y && event.y < 0)
			{
				mapping->control->set_temporary_value(mapping->control->get_current_value() - event.y);
			}
			else if (mapping->axis == mouse_wheel_axis::positive_y && event.y > 0)
			{
				mapping->control->set_temporary_value(mapping->control->get_current_value() + event.y);
			}
		}
	}
}

void event_router::handle_event(const mouse_button_pressed_event& event)
{
	for (const mouse_button_mapping* mapping: mouse_button_mappings)
	{
		if ((!mapping->mouse || mapping->mouse == event.mouse) && mapping->button == event.button)
		{
			mapping->control->set_current_value(1.0f);
		}
	}
}

void event_router::handle_event(const mouse_button_released_event& event)
{
	for (const mouse_button_mapping* mapping: mouse_button_mappings)
	{
		if ((!mapping->mouse || mapping->mouse == event.mouse) && mapping->button == event.button)
		{
			mapping->control->set_current_value(0.0f);
		}
	}
}

void event_router::handle_event(const gamepad_axis_moved_event& event)
{
	for (const gamepad_axis_mapping* mapping: gamepad_axis_mappings)
	{
		if ((!mapping->controller || mapping->controller == event.controller) && mapping->axis == event.axis)
		{
			if (mapping->negative && event.value >= 0.0f || !mapping->negative && event.value <= 0.0f)
			{
				mapping->control->set_current_value(0.0f);
			}
			else
			{
				mapping->control->set_current_value(std::abs(event.value));
			}
		}
	}
}

void event_router::handle_event(const gamepad_button_pressed_event& event)
{
	for (const gamepad_button_mapping* mapping: gamepad_button_mappings)
	{
		if ((!mapping->controller || mapping->controller == event.controller) && mapping->button == event.button)
		{
			mapping->control->set_current_value(1.0f);
		}
	}
}

void event_router::handle_event(const gamepad_button_released_event& event)
{
	for (const gamepad_button_mapping* mapping: gamepad_button_mappings)
	{
		if ((!mapping->controller || mapping->controller == event.controller) && mapping->button == event.button)
		{
			mapping->control->set_current_value(0.0f);
		}
	}
}

} // namespace input
