/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_EVENT_HPP
#define ANTKEEPER_EVENT_HPP

#include <atomic>
#include <cstdlib>

/**
 * Abstract base class for events.
 */
class event_base
{
public:
	/// Destroys an event base.
	virtual ~event_base() = default;

	/// Returns the unique event type identifier for this event type.
	virtual const std::size_t get_event_type_id() const = 0;

	/**
	 * Allocates a copy of this event.
	 *
	 * @return Newly allocated copy of this event.
	 */
	virtual event_base* clone() const = 0;

protected:
	/// Returns then increments the next available event type ID.
	static std::size_t next_event_type_id();
};

inline std::size_t event_base::next_event_type_id()
{
	static std::atomic<std::size_t> next_event_type_id{0};
	return next_event_type_id++;
}

/**
 * Templated abstract base class for events.
 *
 * @tparam T The derived class.
 */
template <typename T>
class event: public event_base
{
public:
	/// The unique event type identifier for this event type.
	static const std::atomic<std::size_t> event_type_id;

	/// Destroys an event
	virtual ~event() = default;
	
	/// @copydoc event_base::get_event_type_id() const
	virtual const std::size_t get_event_type_id() const final;
	
	/// @copydoc event_base::clone() const
	virtual event_base* clone() const = 0;
};

template <typename T>
const std::atomic<std::size_t> event<T>::event_type_id{event_base::next_event_type_id()};

template <typename T>
inline const std::size_t event<T>::get_event_type_id() const
{
	return event_type_id;
}

#endif // ANTKEEPER_EVENT_HPP

