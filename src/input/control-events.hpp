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

#ifndef ANTKEEPER_INPUT_CONTROL_EVENTS_HPP
#define ANTKEEPER_INPUT_CONTROL_EVENTS_HPP

namespace input {

class control;

/**
 * Event generated when a control has been activated.
 */
struct control_activated_event
{
	/// Control that was activated.
	control* control;
};

/**
 * Event generated while a control is active.
 */
struct control_active_event
{
	/// Active control.
	control* control;
	
	/// Control input value.
	float input_value;
};

/**
 * Event generated when a control has been deactivated.
 */
struct control_deactivated_event
{
	/// Control that was deactivated.
	control* control;
};

} // namespace input

#endif // ANTKEEPER_INPUT_CONTROL_EVENTS_HPP
