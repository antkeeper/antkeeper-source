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

#ifndef ANTKEEPER_APP_DISPLAY_HPP
#define ANTKEEPER_APP_DISPLAY_HPP

#include <engine/app/display-orientation.hpp>
#include <engine/app/display-events.hpp>
#include <engine/geom/primitive/rectangle.hpp>
#include <engine/event/publisher.hpp>
#include <string>

namespace app {

/**
 * Virtual display.
 */
class display
{
public:
	/**
	 * Sets the index of the display.
	 *
	 * @param index Index of the display.
	 */
	inline void set_index(int index) noexcept
	{
		this->index = index;
	}
	
	/**
	 * Sets the name of the display.
	 *
	 * @param name Name of the display.
	 */
	inline void set_name(const std::string& name) noexcept
	{
		this->name = name;
	}
	
	/**
	 * Sets the bounds of the display.
	 *
	 * @param bounds Bounds of the display, in display units.
	 */
	inline void set_bounds(const geom::primitive::rectangle<int>& bounds) noexcept
	{
		this->bounds = bounds;
	}
	
	/**
	 * Sets the usable bounds of the display, which excludes areas reserved by the OS for things like menus or docks.
	 */
	inline void set_usable_bounds(const geom::primitive::rectangle<int>& bounds) noexcept
	{
		this->usable_bounds = bounds;
	}
	
	/**
	 * Sets the refresh rate of the display.
	 *
	 * @param rate Refresh rate, in Hz.
	 */
	inline void set_refresh_rate(int rate) noexcept
	{
		this->refresh_rate = rate;
	}
	
	/**
	 * Sets the DPI of the display.
	 *
	 * @param dpi DPI.
	 */
	inline void set_dpi(float dpi) noexcept
	{
		this->dpi = dpi;
	}
	
	/**
	 * Sets the orientation of the display.
	 *
	 * @param orientation Display orientation.
	 */
	inline void set_orientation(display_orientation orientation) noexcept
	{
		this->orientation = orientation;
	}

	/// Returns the index of the display.
	[[nodiscard]] inline const int& get_index() const noexcept
	{
		return index;
	}
	
	/// Returns the name of the display.
	[[nodiscard]] inline const std::string& get_name() const noexcept
	{
		return name;
	}
	
	/// Returns the bounds of the display, in display units.
	[[nodiscard]] inline const geom::primitive::rectangle<int>& get_bounds() const noexcept
	{
		return bounds;
	}
	
	/// Returns the usable bounds of the display, which excludes areas reserved by the OS for things like menus or docks, in display units.
	[[nodiscard]] inline const geom::primitive::rectangle<int>& get_usable_bounds() const noexcept
	{
		return usable_bounds;
	}
	
	/// Returns the refresh rate of the display, in Hz.
	[[nodiscard]] inline const int& get_refresh_rate() const noexcept
	{
		return refresh_rate;
	}
	
	/// Returns the DPI of the display.
	[[nodiscard]] inline const float& get_dpi() const noexcept
	{
		return dpi;
	}
	
	/// Returns the current orientation of the display.
	[[nodiscard]] inline const display_orientation& get_orientation() const noexcept
	{
		return orientation;
	}
	
	/// Returns `true` if the display is connected, `false` otherwise.
	[[nodiscard]] inline const bool& is_connected() const noexcept
	{
		return connected;
	}
	
	/// Returns the channel through which display connected events are published.
	[[nodiscard]] inline event::channel<display_connected_event>& get_connected_channel() noexcept
	{
		return connected_publisher.channel();
	}
	
	/// Returns the channel through which display disconnected events are published.
	[[nodiscard]] inline event::channel<display_disconnected_event>& get_disconnected_channel() noexcept
	{
		return disconnected_publisher.channel();
	}
	
	/// Returns the channel through which display orientation changed events are published.
	[[nodiscard]] inline event::channel<display_orientation_changed_event>& get_orientation_changed_channel() noexcept
	{
		return orientation_changed_publisher.channel();
	}
	
private:
	friend class window_manager;
	friend class sdl_window_manager;
	
	int index{0};
	std::string name;
	geom::primitive::rectangle<int> bounds{0};
	geom::primitive::rectangle<int> usable_bounds{0};
	int refresh_rate{0};
	float dpi{0.0f};
	display_orientation orientation{0};
	bool connected{false};
	
	event::publisher<display_connected_event> connected_publisher;
	event::publisher<display_disconnected_event> disconnected_publisher;
	event::publisher<display_orientation_changed_event> orientation_changed_publisher;
};

} // namespace app

#endif // ANTKEEPER_APP_DISPLAY_HPP
