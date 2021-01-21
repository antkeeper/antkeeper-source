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

#ifndef ANTKEEPER_INPUT_MOUSE_HPP
#define ANTKEEPER_INPUT_MOUSE_HPP

#include "device.hpp"
#include <tuple>

namespace input {

/**
 * Enumerates the mouse motion axes.
 */
enum class mouse_motion_axis
{
	/// Indicates the positive X-axis.
	positive_x,

	/// Indicates the negative X-axis.
	negative_x,

	/// Indicates the positive Y-axis.
	positive_y,

	/// Indicates the negative Y-axis.
	negative_y
};

/**
 * Enumerates the mouse wheel axes.
 */
enum class mouse_wheel_axis
{
	/// Indicates the positive X-axis.
	positive_x,

	/// Indicates the negative X-axis.
	negative_x,

	/// Indicates the positive Y-axis.
	positive_y,

	/// Indicates the negative Y-axis.
	negative_y
};

/**
 * A virtual mouse which can generate mouse-related input events and pass them to an event dispatcher.
 */
class mouse: public device
{
public:
	/**
	 * Creates a mouse input device.
	 */
	mouse();

	/// Destroys a mouse input device.
	virtual ~mouse() = default;
	
	/**
	 * Simulates a mouse button press.
	 *
	 * @param button Index of the pressed button.
	 * @param x X-coordinate of the mouse position.
	 * @param y Y-coordinate of the mouse position.
	 */
	void press(int button, int x, int y);

	/**
	 * Simulates a mouse button release.	
	 *
	 * @param button Index of the released button.
	 * @param x X-coordinate of the mouse position.
	 * @param y Y-coordinate of the mouse position.
	 */
	void release(int button, int x, int y);

	/**
	 * Simulates mouse movement.
	 *
	 * @param x X-coordinate of the mouse position.
	 * @param y Y-coordinate of the mouse position.
	 * @param dx Relative movement on the X-axis.
	 * @param dy Relative movement on the Y-axis.
	 */
	void move(int x, int y, int dx, int dy);

	/**
	 * Simulates mouse wheel scrolling.
	 */
	void scroll(int x, int y);
	
	/// Returns the current mouse position.
	const std::tuple<int, int>& get_current_position() const;

	/// Returns the previous mouse position.
	const std::tuple<int, int>& get_previous_position() const;
	
private:
	std::tuple<int, int> current_position;
	std::tuple<int, int> previous_position;
};

inline const std::tuple<int, int>& mouse::get_current_position() const
{
	return current_position;
}

inline const std::tuple<int, int>& mouse::get_previous_position() const
{
	return previous_position;
}

} // namespace input

#endif // ANTKEEPER_INPUT_MOUSE_HPP

