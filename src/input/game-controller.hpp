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

#ifndef ANTKEEPER_GAME_CONTROLLER_HPP
#define ANTKEEPER_GAME_CONTROLLER_HPP

#include "input-device.hpp"

enum class game_controller_button
{
	a,
	b,
	x,
	y,
	back,
	guide,
	start,
	left_stick,
	right_stick,
	left_shoulder,
	right_shoulder,
	dpad_up,
	dpad_down,
	dpad_left,
	dpad_right
};

enum class game_controller_axis
{
	left_x,
	left_y,
	right_x,
	right_y,
	trigger_left,
	trigger_right,
};

/**
 * A virtual game controller which can generate game controller-related input events and pass them to an event dispatcher.
 *
 * @ingroup input
 */
class game_controller: public input_device
{
public:
	/**
	 * Creates a game controller input device.
	 */
	game_controller();

	/// Destroys a game controller input device.
	virtual ~game_controller() = default;
	
	/**
	 * Simulates a game controller button press.
	 *
	 * @param button Index of the pressed button.
	 */
	void press(game_controller_button button);

	/**
	 * Simulates a game controller button release.
	 *
	 * @param button Index of the released button.
	 */
	void release(game_controller_button button);

	/**
	 * Simulates a game controller axis movement.
	 *
	 * @param axis Index of the moved axis.
	 * @param negative Whether the movement was negative or positive.
	 * @param value Normalized degree of movement.
	 */
	void move(game_controller_axis axis, float value);
	
	/**
	 * Simulates a game controller being connected.
	 *
	 * @param reconnected `true` if the controller is being reconnected, or `false` if the controller is being connected for the first time.
	 */
	void connect(bool reconnnected);
	
	/// Simulates a game controller being disconnected.
	void disconnect();
	
	/// Returns `true` if the controller is currently connected.
	bool is_connected() const;
	
private:
	bool connected;
};

inline bool game_controller::is_connected() const
{
	return connected;
}

#endif // ANTKEEPER_GAME_CONTROLLER_HPP

