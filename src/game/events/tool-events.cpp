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

#include "game/events/tool-events.hpp"

event_base* tool_pressed_event::clone() const
{
	tool_pressed_event* event = new tool_pressed_event();
	event->entity = entity;
	event->position = position;
	return event;
}

event_base* tool_released_event::clone() const
{
	tool_released_event* event = new tool_released_event();
	event->entity = entity;
	event->position = position;
	return event;
}
