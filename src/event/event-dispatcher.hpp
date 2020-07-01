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

#ifndef ANTKEEPER_EVENT_DISPATCHER_HPP
#define ANTKEEPER_EVENT_DISPATCHER_HPP

#include "event.hpp"
#include "event-handler.hpp"
#include <cstdlib>
#include <list>
#include <map>

/**
 * Queues events and dispatches them to event handlers.
 */
class event_dispatcher
{
public:
	/// Creates an event dispatcher
	event_dispatcher();

	/// Destroys an event dispatcher
	~event_dispatcher();

	/**
	 * Processes all pending subscriptions and unsubscriptions, dispatches queued events, then dispatches due scheduled events.
	 *
	 * @param time The current time.
	 */
	void update(double time);

	/**
	 * Subscribes an event handler to event dispatches.
	 *
	 * @param handler Handler to subscribe.
	 */
	template <typename T>
	void subscribe(event_handler<T>* handler);

	/**
	 * Unsubscribes an event handler from event dispatches.
	 *
	 * @param handler Handler to unsubscribe.
	 */
	template <typename T>
	void unsubscribe(event_handler<T>* handler);

	/**
	 * Adds an event to the queue.
	 *
	 * @param event Event to queue.
	 */
	void queue(const event_base& event);

	/**
	 * Schedules an event to be dispatched at a specific time.
	 *
	 * @param event Event to schedule.
	 * @param time Time that the event should be dispatched.
	 */
	void schedule(const event_base& event, double time);

	/**
	 * Dispatches a single event.
	 *
	 * @param event Event to dispatch.
	 */
	void dispatch(const event_base& event);

	/**
	 * Dispatches all events in the queue.
	 */
	void flush();

	/// Removes all queued and scheduled events from the queue without notifying handlers.
	void clear();

private:
	std::list<std::tuple<std::size_t, event_handler_base*>> to_subscribe;
	std::list<std::tuple<std::size_t, event_handler_base*>> to_unsubscribe;
	std::map<std::size_t, std::list<event_handler_base*>> handler_map;
	std::list<event_base*> queued_events;
	std::multimap<double, event_base*> scheduled_events;
};

template <typename T>
void event_dispatcher::subscribe(event_handler<T>* handler)
{
	to_subscribe.push_back(std::make_tuple(handler->get_handled_event_type_id(), handler));
}

template <typename T>
void event_dispatcher::unsubscribe(event_handler<T>* handler)
{
	to_unsubscribe.push_back(std::make_tuple(handler->get_handled_event_type_id(), handler));
}

inline void event_dispatcher::queue(const event_base& event)
{
	queued_events.push_back(event.clone());
}

inline void event_dispatcher::schedule(const event_base& event, double time)
{
	scheduled_events.insert(std::pair<double, event_base*>(time, event.clone()));
}

inline void event_dispatcher::dispatch(const event_base& event)
{
	// Get list of handlers for this type of event
	const std::list<event_handler_base*>& handlers = handler_map[event.get_event_type_id()];

	// For each handler
	for (auto handler = handlers.begin(); handler != handlers.end(); ++handler)
	{
		// Pass event to the handler
		(*handler)->route_event(event);
	}
}

#endif // ANTKEEPER_EVENT_DISPATCHER_HPP

