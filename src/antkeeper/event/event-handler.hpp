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

#ifndef ANTKEEPER_EVENT_HANDLER_HPP
#define ANTKEEPER_EVENT_HANDLER_HPP

#include "event.hpp"
#include <type_traits>

class event_dispatcher;

/**
 * Abstract base class for event handlers.
 */
class event_handler_base
{
private:
	friend class event_dispatcher;

	/**
	 * Receives an event, casts it to its derived event type, then handles it.
	 *
	 * @param event Received event.
	 */
	virtual void route_event(const event_base& event) = 0;
};

/**
 * Templates abstract base class for event handlers.
 *
 * @tparam Event type.
 */
template <typename T>
class event_handler: public event_handler_base
{
public:
	static_assert(std::is_base_of<event_base, T>::value, "T must be a descendant of event_base.");

	/// Returns the unique event type identifier for the event type handled by this event handler.
	const std::size_t get_handled_event_type_id() const;

	/**
	 * Handles an event of type T.
	 *
	 * @param event Event to handle.
	 */
	virtual void handle_event(const T& event) = 0;

private:
	/// @copydoc event_handler_base::route_event()
	virtual void route_event(const event_base& event) final;
};

template <typename T>
inline const std::size_t event_handler<T>::get_handled_event_type_id() const
{
	return T::event_type_id;
}

template <typename T>
void event_handler<T>::route_event(const event_base& event)
{
	handle_event(static_cast<const T&>(event));
}

#endif // ANTKEEPER_EVENT_HANDLER_HPP

