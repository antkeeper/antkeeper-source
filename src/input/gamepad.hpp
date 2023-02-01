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

#ifndef ANTKEEPER_INPUT_GAMEPAD_HPP
#define ANTKEEPER_INPUT_GAMEPAD_HPP

#include "input/device.hpp"
#include "input/event.hpp"
#include "input/gamepad-axis.hpp"
#include "input/gamepad-button.hpp"
#include "event/publisher.hpp"

namespace input {

/// Gamepad axis activation response curves.
enum class gamepad_response_curve
{
	/// Linear response curve.
	linear,
	
	/// Squared response curve.
	square,
	
	/// Cubed response curve.
	cube
};

/**
 * A virtual gamepad which generates gamepad-related input events.
 */
class gamepad: public device
{
public:
	/**
	 * Constructs a gamepad input device.
	 */
	gamepad();
	
	/// Destructs a gamepad input device.
	virtual ~gamepad() = default;
	
	/**
	 * Sets the activation threshold for a gamepad axis.
	 *
	 * @param axis Gamepad axis.
	 * @param min Axis minimum activation threshold.
	 * @param max Axis maximum activation threshold.
	 */
	void set_activation_threshold(gamepad_axis axis, float min, float max);
	
	/**
	 * Sets the activation response curve of an axis.
	 *
	 * @param axis Gamepad axis.
	 * @param curve Activation response curve.
	 */
	void set_response_curve(gamepad_axis axis, gamepad_response_curve curve);
	
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
	 * Simulates a gamepad button press.
	 *
	 * @param button Button to press.
	 */
	void press(gamepad_button button);
	
	/**
	 * Simulates a gamepad button release.
	 *
	 * @param button Button to release.
	 */
	void release(gamepad_button button);
	
	/**
	 * Simulates a gamepad axis movement.
	 *
	 * @param axis Gamepad axis.
	 * @param position Position on the axis, on `[-1, 1]`.
	 */
	void move(gamepad_axis axis, float position);
	
	/// Returns the channel through which gamepad button pressed events are published.
	[[nodiscard]] inline ::event::channel<event::gamepad_button_pressed>& get_button_pressed_channel() noexcept
	{
		return button_pressed_publisher.channel();
	}
	
	/// Returns the channel through which gamepad button released events are published.
	[[nodiscard]] inline ::event::channel<event::gamepad_button_released>& get_button_released_channel() noexcept
	{
		return button_released_publisher.channel();
	}
	
	/// Returns the channel through which gamepad axis moved events are published.
	[[nodiscard]] inline ::event::channel<event::gamepad_axis_moved>& get_axis_moved_channel() noexcept
	{
		return axis_moved_publisher.channel();
	}
	
	/// Returns device_type::gamepad.
	[[nodiscard]] inline virtual constexpr device_type get_device_type() const noexcept
	{
		return device_type::gamepad;
	}
	
private:
	void handle_axial_motion(gamepad_axis axis);
	void handle_biaxial_motion(gamepad_axis axis_x, gamepad_axis axis_y);
	float curve_response(gamepad_axis axis, float response) const;
	
	float axis_positions[6];
	float axis_activation_min[6];
	float axis_activation_max[6];
	gamepad_response_curve axis_response_curves[6];
	
	bool left_deadzone_cross;
	bool right_deadzone_cross;
	float left_deadzone_roundness;
	float right_deadzone_roundness;
	
	::event::publisher<event::gamepad_button_pressed> button_pressed_publisher;
	::event::publisher<event::gamepad_button_released> button_released_publisher;
	::event::publisher<event::gamepad_axis_moved> axis_moved_publisher;
};

} // namespace input

#endif // ANTKEEPER_INPUT_GAMEPAD_HPP
