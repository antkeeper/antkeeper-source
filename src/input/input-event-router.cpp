/*
 * Copyright (C) 2020  Christopher J. Howard
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

#include "input-event-router.hpp"
#include "control.hpp"
#include "input-mapping.hpp"
#include "mouse.hpp"
#include "event/event-dispatcher.hpp"

input_event_router::input_event_router():
	event_dispatcher(nullptr)
{}

input_event_router::~input_event_router()
{
	remove_mappings();
	set_event_dispatcher(nullptr);
}

void input_event_router::add_mapping(const input_mapping& mapping)
{
	control* control = mapping.control;

	switch (mapping.get_type())
	{
		case input_mapping_type::key:
		{
			::key_mapping* key_mapping = new ::key_mapping(static_cast<const ::key_mapping&>(mapping));
			key_mappings.push_back(key_mapping);
			controls[control].push_back(key_mapping);

			break;
		}

		case input_mapping_type::mouse_motion:
		{
			::mouse_motion_mapping* mouse_motion_mapping = new ::mouse_motion_mapping(static_cast<const ::mouse_motion_mapping&>(mapping));
			mouse_motion_mappings.push_back(mouse_motion_mapping);
			controls[control].push_back(mouse_motion_mapping);

			break;
		}

		case input_mapping_type::mouse_wheel:
		{
			::mouse_wheel_mapping* mouse_wheel_mapping = new ::mouse_wheel_mapping(static_cast<const ::mouse_wheel_mapping&>(mapping));
			mouse_wheel_mappings.push_back(mouse_wheel_mapping);
			controls[control].push_back(mouse_wheel_mapping);

			break;
		}

		case input_mapping_type::mouse_button:
		{
			::mouse_button_mapping* mouse_button_mapping = new ::mouse_button_mapping(static_cast<const ::mouse_button_mapping&>(mapping));
			mouse_button_mappings.push_back(mouse_button_mapping);
			controls[control].push_back(mouse_button_mapping);

			break;
		}

		case input_mapping_type::game_controller_axis:
		{
			::game_controller_axis_mapping* game_controller_axis_mapping = new ::game_controller_axis_mapping(static_cast<const ::game_controller_axis_mapping&>(mapping));
			game_controller_axis_mappings.push_back(game_controller_axis_mapping);
			controls[control].push_back(game_controller_axis_mapping);

			break;
		}

		case input_mapping_type::game_controller_button:
		{
			::game_controller_button_mapping* game_controller_button_mapping = new ::game_controller_button_mapping(static_cast<const ::game_controller_button_mapping&>(mapping));
			game_controller_button_mappings.push_back(game_controller_button_mapping);
			controls[control].push_back(game_controller_button_mapping);

			break;
		}

		default:
			break;
	}
}

void input_event_router::remove_mappings(control* control)
{
	auto it = controls.find(control);
	if (it != controls.end())
	{
		for (input_mapping* mapping: it->second)
		{
			switch (mapping->get_type())
			{
				case input_mapping_type::key:
					key_mappings.remove(static_cast<key_mapping*>(mapping));
					break;

				case input_mapping_type::mouse_motion:
					mouse_motion_mappings.remove(static_cast<mouse_motion_mapping*>(mapping));
					break;

				case input_mapping_type::mouse_wheel:
					mouse_wheel_mappings.remove(static_cast<mouse_wheel_mapping*>(mapping));
					break;

				case input_mapping_type::mouse_button:
					mouse_button_mappings.remove(static_cast<mouse_button_mapping*>(mapping));
					break;

				case input_mapping_type::game_controller_axis:
					game_controller_axis_mappings.remove(static_cast<game_controller_axis_mapping*>(mapping));
					break;

				case input_mapping_type::game_controller_button:
					game_controller_button_mappings.remove(static_cast<game_controller_button_mapping*>(mapping));
					break;

				default:
					break;
			}

			delete mapping;
		}

		controls.erase(it);
	}
}

void input_event_router::set_event_dispatcher(::event_dispatcher* event_dispatcher)
{
	if (this->event_dispatcher)
	{
		this->event_dispatcher->unsubscribe<key_pressed_event>(this);
		this->event_dispatcher->unsubscribe<key_released_event>(this);
		this->event_dispatcher->unsubscribe<mouse_moved_event>(this);
		this->event_dispatcher->unsubscribe<mouse_wheel_scrolled_event>(this);
		this->event_dispatcher->unsubscribe<mouse_button_pressed_event>(this);
		this->event_dispatcher->unsubscribe<mouse_button_released_event>(this);
		this->event_dispatcher->unsubscribe<game_controller_axis_moved_event>(this);
		this->event_dispatcher->unsubscribe<game_controller_button_pressed_event>(this);
		this->event_dispatcher->unsubscribe<game_controller_button_released_event>(this);
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
		event_dispatcher->subscribe<game_controller_axis_moved_event>(this);
		event_dispatcher->subscribe<game_controller_button_pressed_event>(this);
		event_dispatcher->subscribe<game_controller_button_released_event>(this);
	}
}

void input_event_router::remove_mappings()
{
	for (auto it = controls.begin(); it != controls.end(); ++it)
	{
		for (input_mapping* mapping: it->second)
		{
			delete mapping;
		}
	}

	controls.clear();
	key_mappings.clear();
	mouse_motion_mappings.clear();
	mouse_wheel_mappings.clear();
	mouse_button_mappings.clear();
	game_controller_axis_mappings.clear();
	game_controller_button_mappings.clear();
}

const std::list<input_mapping*>* input_event_router::get_mappings(control* control) const
{
	auto it = controls.find(control);
	if (it == controls.end())
	{
		return nullptr;
	}

	return &it->second;
}

void input_event_router::handle_event(const key_pressed_event& event)
{
	for (const key_mapping* mapping: key_mappings)
	{
		if ((!mapping->keyboard || mapping->keyboard == event.keyboard) && mapping->scancode == event.scancode)
		{
			mapping->control->set_current_value(1.0f);
		}
	}
}

void input_event_router::handle_event(const key_released_event& event)
{
	for (const key_mapping* mapping: key_mappings)
	{
		if ((!mapping->keyboard || mapping->keyboard == event.keyboard) && mapping->scancode == event.scancode)
		{
			mapping->control->set_current_value(0.0f);
		}
	}
}

void input_event_router::handle_event(const mouse_moved_event& event)
{
	for (const mouse_motion_mapping* mapping: mouse_motion_mappings)
	{
		if (!mapping->mouse || mapping->mouse == event.mouse)
		{
			if (mapping->axis == mouse_motion_axis::negative_x && event.dx < 0)
			{
				mapping->control->set_temporary_value(-event.dx);
			}
			else if (mapping->axis == mouse_motion_axis::positive_x && event.dx > 0)
			{
				mapping->control->set_temporary_value(event.dx);
			}
			else if (mapping->axis == mouse_motion_axis::negative_y && event.dy < 0)
			{
				mapping->control->set_temporary_value(-event.dy);
			}
			else if (mapping->axis == mouse_motion_axis::positive_y && event.dy > 0)
			{
				mapping->control->set_temporary_value(event.dy);
			}
		}
	}
}

void input_event_router::handle_event(const mouse_wheel_scrolled_event& event)
{
	for (const mouse_wheel_mapping* mapping: mouse_wheel_mappings)
	{
		if (!mapping->mouse || mapping->mouse == event.mouse)
		{
			if (mapping->axis == mouse_wheel_axis::negative_x && event.x < 0)
			{
				mapping->control->set_temporary_value(-event.x);
			}
			else if (mapping->axis == mouse_wheel_axis::positive_x && event.x > 0)
			{
				mapping->control->set_temporary_value(event.x);
			}
			else if (mapping->axis == mouse_wheel_axis::negative_y && event.y < 0)
			{
				mapping->control->set_temporary_value(-event.y);
			}
			else if (mapping->axis == mouse_wheel_axis::positive_y && event.y > 0)
			{
				mapping->control->set_temporary_value(event.y);
			}
		}
	}
}

void input_event_router::handle_event(const mouse_button_pressed_event& event)
{
	for (const mouse_button_mapping* mapping: mouse_button_mappings)
	{
		if ((!mapping->mouse || mapping->mouse == event.mouse) && mapping->button == event.button)
		{
			mapping->control->set_current_value(1.0f);
		}
	}
}

void input_event_router::handle_event(const mouse_button_released_event& event)
{
	for (const mouse_button_mapping* mapping: mouse_button_mappings)
	{
		if ((!mapping->mouse || mapping->mouse == event.mouse) && mapping->button == event.button)
		{
			mapping->control->set_current_value(0.0f);
		}
	}
}

void input_event_router::handle_event(const game_controller_axis_moved_event& event)
{
	for (const game_controller_axis_mapping* mapping: game_controller_axis_mappings)
	{
		if ((!mapping->game_controller || mapping->game_controller == event.game_controller) && mapping->axis == event.axis)
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

void input_event_router::handle_event(const game_controller_button_pressed_event& event)
{
	for (const game_controller_button_mapping* mapping: game_controller_button_mappings)
	{
		if ((!mapping->game_controller || mapping->game_controller == event.game_controller) && mapping->button == event.button)
		{
			mapping->control->set_current_value(1.0f);
		}
	}
}

void input_event_router::handle_event(const game_controller_button_released_event& event)
{
	for (const game_controller_button_mapping* mapping: game_controller_button_mappings)
	{
		if ((!mapping->game_controller || mapping->game_controller == event.game_controller) && mapping->button == event.button)
		{
			mapping->control->set_current_value(0.0f);
		}
	}
}

