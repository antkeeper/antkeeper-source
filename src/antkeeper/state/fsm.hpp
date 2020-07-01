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

#ifndef ANTKEEPER_STATE_MACHINE_HPP
#define ANTKEEPER_STATE_MACHINE_HPP

#include <functional>

namespace fsm {

/**
 * A finite state machine state consisting of a pair of `enter()` and `exit()` functions.
 */
struct state
{
	std::function<void()> enter;
	std::function<void()> exit;
};

/**
 * General purpose finite state machine.
 */
class machine
{
public:
	machine();

	void change_state(const state& next_state);

	const state& get_previous_state() const;
	const state& get_current_state() const;

private:
	state previous_state;
	state current_state;
};

inline const state& machine::get_previous_state() const
{
	return previous_state;
}

inline const state& machine::get_current_state() const
{
	return current_state;
}

} // namespace fsm

#endif // ANTKEEPER_STATE_MACHINE_HPP

