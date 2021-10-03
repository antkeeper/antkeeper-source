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

#ifndef ANTKEEPER_INPUT_DEVICE_HPP
#define ANTKEEPER_INPUT_DEVICE_HPP

#include "event/event-dispatcher.hpp"
#include <string>

namespace input {

/**
 * Base class for virtual devices which generate input events.
 */
class device
{
public:
	device();
	virtual ~device() = default;

	void set_event_dispatcher(event_dispatcher* event_dispatcher);
	const event_dispatcher* get_event_dispatcher() const;
	event_dispatcher* get_event_dispatcher();
	
	/**
	 * Sets the globally unique identifier (GUID) of this input device.
	 *
	 * @param guid GUID string.
	 */
	void set_guid(const std::string& guid);
	
	/// Returns the globally unique identifier (GUID) of this input device.
	const std::string& get_guid() const;

protected:
	event_dispatcher* event_dispatcher;
	
private:
	std::string guid;
};

inline const event_dispatcher* device::get_event_dispatcher() const
{
	return event_dispatcher;
}

inline event_dispatcher* device::get_event_dispatcher()
{
	return event_dispatcher;
}

inline const std::string& device::get_guid() const
{
	return guid;
}

} // namespace input

#endif // ANTKEEPER_INPUT_DEVICE_HPP

