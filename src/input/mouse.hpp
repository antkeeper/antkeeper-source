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

#ifndef ANTKEEPER_INPUT_MOUSE_HPP
#define ANTKEEPER_INPUT_MOUSE_HPP

#include "input/device.hpp"
#include "input/input-events.hpp"
#include "input/mouse-button.hpp"
#include "event/publisher.hpp"
#include "math/vector.hpp"
#include <cstdint>

namespace input {

/**
 * A virtual mouse which generates mouse-related input events.
 */
class mouse: public device
{
public:
	/**
	 * Constructs a mouse input device.
	 */
	mouse() = default;
	
	/// Destructs a mouse input device.
	virtual ~mouse() = default;
	
	/**
	 * Simulates a mouse button press.
	 *
	 * @param button Button to press.
	 */
	void press(mouse_button button);
	
	/**
	 * Simulates a mouse button release.	
	 *
	 * @param button Button to release.
	 */
	void release(mouse_button button);
	
	/**
	 * Simulates mouse movement.
	 *
	 * @param position Mouse position, in pixels, relative to the window.
	 * @param difference Relative movement of the mouse, in pixels.
	 */
	void move(const math::vector<std::int32_t, 2>& position, const math::vector<std::int32_t, 2>& difference);
	
	/**
	 * Simulates mouse scrolling.
	 *
	 * @param velocity Scroll velocity.
	 */
	void scroll(const math::vector<float, 2>& velocity);
	
	/// Returns the current mouse position, in pixels, relative to the window.
	[[nodiscard]] inline const math::vector<std::int32_t, 2>& get_position() const noexcept
	{
		return position;
	}
	
	/// Returns the channel through which mouse button pressed events are published.
	[[nodiscard]] inline ::event::channel<mouse_button_pressed_event>& get_button_pressed_channel() noexcept
	{
		return button_pressed_publisher.channel();
	}
	
	/// Returns the channel through which mouse button released events are published.
	[[nodiscard]] inline ::event::channel<mouse_button_released_event>& get_button_released_channel() noexcept
	{
		return button_released_publisher.channel();
	}
	
	/// Returns the channel through which mouse moved events are published.
	[[nodiscard]] inline ::event::channel<mouse_moved_event>& get_moved_channel() noexcept
	{
		return moved_publisher.channel();
	}
	
	/// Returns the channel through which mouse scrolled events are published.
	[[nodiscard]] inline ::event::channel<mouse_scrolled_event>& get_scrolled_channel() noexcept
	{
		return scrolled_publisher.channel();
	}
	
	/// Returns device_type::mouse.
	[[nodiscard]] inline virtual constexpr device_type get_device_type() const noexcept
	{
		return device_type::mouse;
	}
	
private:
	math::vector<std::int32_t, 2> position;
	
	::event::publisher<mouse_button_pressed_event> button_pressed_publisher;
	::event::publisher<mouse_button_released_event> button_released_publisher;
	::event::publisher<mouse_moved_event> moved_publisher;
	::event::publisher<mouse_scrolled_event> scrolled_publisher;
};

} // namespace input

#endif // ANTKEEPER_INPUT_MOUSE_HPP
