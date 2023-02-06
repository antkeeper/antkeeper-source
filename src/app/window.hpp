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

#ifndef ANTKEEPER_APP_WINDOW_HPP
#define ANTKEEPER_APP_WINDOW_HPP

#include "math/vector.hpp"
#include "event/publisher.hpp"
#include "app/window-events.hpp"
#include "gl/rasterizer.hpp"
#include <string>

namespace app {

class window_manager;

/**
 * 
 */
class window
{
public:
	/**
	 * Constructs a window.
	 */
	window() = default;
	
	/**
	 * Destructs a window.
	 */
	virtual ~window() = default;
	
	/**
	 * Changes the title of the window.
	 *
	 * @param title Window title.
	 */
	virtual void set_title(const std::string& title) = 0;
	
	/**
	 * Changes the position of the window.
	 *
	 * @param position Position of the window, in display units.
	 */
	virtual void set_position(const math::vector<int, 2>& position) = 0;
	
	/**
	 * Changes the size of the window.
	 *
	 * @param size Size of the window, in display units.
	 */
	virtual void set_size(const math::vector<int, 2>& size) = 0;
	
	/**
	 * Sets the minimum size of the window.
	 *
	 * @param size Minimum size of the window, in display units.
	 */
	virtual void set_minimum_size(const math::vector<int, 2>& size) = 0;
	
	/**
	 * Sets the maximum size of the window.
	 *
	 * @param size Maximum size of the window, in display units.
	 */
	virtual void set_maximum_size(const math::vector<int, 2>& size) = 0;
	
	/**
	 * Maximizes or unmaximizes the window.
	 *
	 * @param maximized `true` if the window should be maximized, `false` otherwise.
	 */
	virtual void set_maximized(bool maximized) = 0;
	
	/**
	 * Enables or disables fullscreen mode.
	 *
	 * @param fullscreen `true` if the window should be in fullscreen mode, `false` otherwise.
	 */
	virtual void set_fullscreen(bool fullscreen) = 0;
	
	/**
	 * Enables or disables v-sync.
	 *
	 * @param v_sync `true` if the v-sync should be enabled, `false` otherwise.
	 */
	virtual void set_v_sync(bool v_sync) = 0;
	
	/**
	 * Makes the window's graphics context current.
	 */
	virtual void make_current() = 0;
	
	/**
	 * Swaps the front and back buffers of the window's graphics context.
	 */
	virtual void swap_buffers() = 0;
	
	/// Returns the title of the window.
	[[nodiscard]] inline const std::string& get_title() const noexcept
	{
		return title;
	}
	
	/// Returns the windowed (non-maximized, non-fullscreen) position of the window, in display units.
	[[nodiscard]] inline const math::vector<int, 2>& get_windowed_position() const noexcept
	{
		return windowed_position;
	}
	
	/// Returns the current position of the window, in display units.
	[[nodiscard]] inline const math::vector<int, 2>& get_position() const noexcept
	{
		return position;
	}
	
	/// Returns the windowed (non-maximized, non-fullscreen) size of the window, in display units.
	[[nodiscard]] inline const math::vector<int, 2>& get_windowed_size() const noexcept
	{
		return windowed_size;
	}
	
	/// Returns the current size of the window, in display units.
	[[nodiscard]] inline const math::vector<int, 2>& get_size() const noexcept
	{
		return size;
	}
	
	/// Returns the minimum size of the window, in display units.
	[[nodiscard]] inline const math::vector<int, 2>& get_minimum_size() const noexcept
	{
		return minimum_size;
	}
	
	/// Returns the maximum size of the window, in display units.
	[[nodiscard]] inline const math::vector<int, 2>& get_maximum_size() const noexcept
	{
		return minimum_size;
	}
	
	/// Returns the current size of the window's drawable viewport, in pixels.
	[[nodiscard]] inline const math::vector<int, 2>& get_viewport_size() const noexcept
	{
		return viewport_size;
	}
	
	/// Returns `true` if the window is maximized, `false` otherwise.
	[[nodiscard]] inline bool is_maximized() const noexcept
	{
		return maximized;
	}
	
	/// Returns `true` if the window is in fullscreen mode, `false` otherwise.
	[[nodiscard]] inline bool is_fullscreen() const noexcept
	{
		return fullscreen;
	}
	
	/// Returns `true` if the v-sync is enabled, `false` otherwise.
	[[nodiscard]] inline bool get_v_sync() const noexcept
	{
		return v_sync;
	}
	
	/// Returns the rasterizer associated with this window.
	[[nodiscard]] virtual gl::rasterizer* get_rasterizer() noexcept = 0;
	
	/// Returns the channel through which window closed events are published.
	[[nodiscard]] inline event::channel<window_closed_event>& get_closed_channel() noexcept
	{
		return closed_publisher.channel();
	}
	
	/// Returns the channel through which window focus changed events are published.
	[[nodiscard]] inline event::channel<window_focus_changed_event>& get_focus_changed_channel() noexcept
	{
		return focus_changed_publisher.channel();
	}
	
	/// Returns the channel through which window maximized events are published.
	[[nodiscard]] inline event::channel<window_maximized_event>& get_maximized_channel() noexcept
	{
		return maximized_publisher.channel();
	}
	
	/// Returns the channel through which window minimized events are published.
	[[nodiscard]] inline event::channel<window_minimized_event>& get_minimized_channel() noexcept
	{
		return minimized_publisher.channel();
	}
	
	/// Returns the channel through which window moved events are published.
	[[nodiscard]] inline event::channel<window_moved_event>& get_moved_channel() noexcept
	{
		return moved_publisher.channel();
	}
	
	/// Returns the channel through which window resized events are published.
	[[nodiscard]] inline event::channel<window_resized_event>& get_resized_channel() noexcept
	{
		return resized_publisher.channel();
	}
	
	/// Returns the channel through which window restored events are published.
	[[nodiscard]] inline event::channel<window_restored_event>& get_restored_channel() noexcept
	{
		return restored_publisher.channel();
	}
	
protected:
	friend class window_manager;
	
	std::string title;
	math::vector<int, 2> windowed_position;
	math::vector<int, 2> position;
	math::vector<int, 2> windowed_size;
	math::vector<int, 2> size;
	math::vector<int, 2> minimum_size;
	math::vector<int, 2> maximum_size;
	math::vector<int, 2> viewport_size;
	bool maximized;
	bool fullscreen;
	bool v_sync;
	
	event::publisher<window_closed_event> closed_publisher;
	event::publisher<window_focus_changed_event> focus_changed_publisher;
	event::publisher<window_maximized_event> maximized_publisher;
	event::publisher<window_minimized_event> minimized_publisher;
	event::publisher<window_moved_event> moved_publisher;
	event::publisher<window_resized_event> resized_publisher;
	event::publisher<window_restored_event> restored_publisher;
};

} // namespace app

#endif // ANTKEEPER_APP_WINDOW_HPP
