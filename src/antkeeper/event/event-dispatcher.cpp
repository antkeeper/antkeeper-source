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

#include "event-dispatcher.hpp"

event_dispatcher::event_dispatcher()
{}

event_dispatcher::~event_dispatcher()
{
	clear();
}

void event_dispatcher::update(double time)
{
	// Process pending subscriptions
	for (auto it = to_subscribe.begin(); it != to_subscribe.end(); ++it)
	{
		handler_map[std::get<0>(*it)].push_back(std::get<1>(*it));
	}
	to_subscribe.clear();

	// Process pending unsubscriptions
	for (auto it = to_unsubscribe.begin(); it != to_unsubscribe.end(); ++it)
	{
		handler_map[std::get<0>(*it)].remove(std::get<1>(*it));
	}
	to_unsubscribe.clear();

	// Dispatch queued events
	flush();

	// For each scheduled event
	for (auto event = scheduled_events.begin(); event != scheduled_events.end();)
	{
		// If the event is due
		if (time >= event->first)
		{
			// Dispatch event
			dispatch(*(event->second));

			// Delete event
			delete event->second;
			event = scheduled_events.erase(event);
		}
		else
		{
			break;
		}
	}
}

void event_dispatcher::flush()
{
	// For each event in the queue
	for (auto event = queued_events.begin(); event != queued_events.end(); ++event)
	{
		// Dispatch event
		dispatch(**event);

		// Delete event
		delete (*event);
	}

	// Clear event queue
	queued_events.clear();
}

void event_dispatcher::clear()
{
	// For each event in the queue
	for (auto event = queued_events.begin(); event != queued_events.end(); ++event)
	{
		// Delete event
		delete (*event);
	}

	// Clear event queue
	queued_events.clear();

	// For each scheduled event
	for (auto event = scheduled_events.begin(); event != scheduled_events.end(); ++event)
	{
		// Delete event
		delete event->second;
	}

	// Clear scheduled events
	scheduled_events.clear();
}

