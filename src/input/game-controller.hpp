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

#ifndef ANTKEEPER_INPUT_GAME_CONTROLLER_HPP
#define ANTKEEPER_INPUT_GAME_CONTROLLER_HPP

#include "device.hpp"

namespace input {

/// Game controller buttons.
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

/// Game controller axes.
enum class game_controller_axis
{
	/// Left stick x-axis.
	left_x,
	
	/// Left stick y-axis.
	left_y,
	
	/// Right stick x-axis.
	right_x,
	
	/// Right stick y-axis.
	right_y,
	
	/// Left trigger.
	left_trigger,
	
	/// Right trigger.
	right_trigger,
};

/// Game controller axis activation response curves.
enum class game_controller_response_curve
{
	/// Linear response curve.
	linear,
	
	/// Squared response curve.
	square,
	
	/// Cubed response curve.
	cube
};

/**
 * A virtual game controller which can generate game controller-related input events and pass them to an event dispatcher.
 *
 * @ingroup input
 */
class game_controller: public device
{
public:
	/**
	 * Creates a game controller input device.
	 */
	game_controller();
	
	/// Destroys a game controller input device.
	virtual ~game_controller() = default;
	
	/**
	 * Sets the activation threshold for a game controller axis.
	 *
	 * @param axis Game controller axis.
	 * @param min Axis minimum activation threshold.
	 * @param max Axis maximum activation threshold.
	 */
	void set_activation_threshold(game_controller_axis axis, float min, float max);
	
	/**
	 * Sets the activation response curve of an axis.
	 *
	 * @param axis Game controller axis.
	 * @param curve Activation response curve.
	 */
	void set_response_curve(game_controller_axis axis, game_controller_response_curve curve);
	
	/**
	 * Sets the type of deadzone shape for the axes on the left stick.
	 *
	 * @param cross If `true`, the x and y axes are independently activated, if `false`, activation of the x and y axes are dependent on their combined magnitude.
	 */
	void set_left_deadzone_cross(bool cross);
	
	/**
	 * Sets the type of deadzone shape for the axes on the right stick.
	 *
	 * @param cross If `true`, the x and y axes are independently activated, if `false`, activation of the x and y axes are dependent on their combined magnitude.
	 */
	void set_right_deadzone_cross(bool cross);
	
	/**
	 * Sets the roundness of the deadzone for the axes on the left stick.
	 *
	 * @param roundness Roundness of the deadzone shape for non-cross deadzones. A value of `0.0` results in a square deadzone, while a value of `1.0` results in a circular deadzone. Values between `0.0` and `1.0` result in a rounded rectangle deadzone.
	 */
	void set_left_deadzone_roundness(float roundness);
	
	/**
	 * Sets the roundness of the deadzone for the axes on the right stick.
	 *
	 * @param roundness Roundness of the deadzone shape for non-cross deadzones. A value of `0.0` results in a square deadzone, while a value of `1.0` results in a circular deadzone. Values between `0.0` and `1.0` result in a rounded rectangle deadzone.
	 */
	void set_right_deadzone_roundness(float roundness);
	
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
	void handle_axial_motion(game_controller_axis axis);
	void handle_biaxial_motion(game_controller_axis axis_x, game_controller_axis axis_y);
	float curve_response(game_controller_axis axis, float response) const;
	
	bool connected;
	float axis_values[6];
	float axis_activation_min[6];
	float axis_activation_max[6];
	game_controller_response_curve axis_response_curves[6];
	
	bool left_deadzone_cross;
	bool right_deadzone_cross;
	float left_deadzone_roundness;
	float right_deadzone_roundness;
};

inline bool game_controller::is_connected() const
{
	return connected;
}

} // namespace input

#endif // ANTKEEPER_INPUT_GAME_CONTROLLER_HPP

