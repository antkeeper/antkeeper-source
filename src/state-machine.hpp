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

#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP

#include <array>
#include <functional>

/**
 * Extremely lightweight finite-state machine.
 */
class StateMachine
{
public:
	/**
	 * A state is a fixed-size array of function pointers with the first and second elements referring to the state's enter and exit functions, respectively.
	 */
	typedef std::array<std::function<void()>, 2> State;

	/**
	 * Creates a state machine, setting the initial state to nullptr.
	 */
	StateMachine();

	/**
	 * Changes the current state.
	 */
	void changeState(const State* nextState);

	/**
	 * Returns the previous state.
	 */
	const State* getPreviousState() const;

	/**
	 * Returns the current state.
	 */
	const State* getCurrentState() const;

private:
	const State* previousState;
	const State* currentState;
};

inline const StateMachine::State* StateMachine::getPreviousState() const
{
	return previousState;
}

inline const StateMachine::State* StateMachine::getCurrentState() const
{
	return currentState;
}

#endif // STATE_MACHINE_HPP

