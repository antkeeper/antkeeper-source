/*
 * Copyright (C) 2017-2019  Christopher J. Howard
 *
 * This file is part of Antkeeper Source Code.
 *
 * Antkeeper Source Code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper Source Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper Source Code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SCHEDULED_FUNCTION_EVENT_HPP
#define SCHEDULED_FUNCTION_EVENT_HPP

#include <emergent/emergent.hpp>
using namespace Emergent;

/**
 * Event which asks a caller to execute a function when the event is handled.
 *
 * @ingroup input
 */
class ScheduledFunctionEvent: public Event<ScheduledFunctionEvent>
{
public:
	virtual EventBase* clone() const;

	void* caller;
	std::function<void()> function;
};

#endif // SCHEDULED_FUNCTION_EVENT_HPP

