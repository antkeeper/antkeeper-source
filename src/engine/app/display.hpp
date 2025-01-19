// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_APP_DISPLAY_HPP
#define ANTKEEPER_APP_DISPLAY_HPP

#include <engine/app/display-orientation.hpp>
#include <engine/app/display-events.hpp>
#include <engine/geom/primitives/rectangle.hpp>
#include <engine/event/publisher.hpp>
#include <string>

namespace app {

/// Virtual display.
class display
{
public:
	/// Sets the index of the display.
	/// @param index Index of the display.
	inline void set_index(int index) noexcept
	{
		m_index = index;
	}
	
	/// Sets the name of the display.
	/// @param name Name of the display.
	inline void set_name(const std::string& name) noexcept
	{
		m_name = name;
	}
	
	/// Sets the bounds of the display.
	/// @param bounds Bounds of the display, in display units.
	inline void set_bounds(const geom::rectangle<int>& bounds) noexcept
	{
		m_bounds = bounds;
	}
	
	/// Sets the usable bounds of the display, which excludes areas reserved by the OS for things like menus or docks.
	/// @param bounds Usable bounds of the display, in display units.
	inline void set_usable_bounds(const geom::rectangle<int>& bounds) noexcept
	{
		m_usable_bounds = bounds;
	}
	
	/// Sets the refresh rate of the display.
	/// @param rate Refresh rate, in Hz.
	inline void set_refresh_rate(int rate) noexcept
	{
		m_refresh_rate = rate;
	}
	
	/// Sets the DPI of the display.
	/// @param dpi DPI.
	inline void set_dpi(float dpi) noexcept
	{
		m_dpi = dpi;
	}
	
	/// Sets the orientation of the display.
	/// @param orientation Display orientation.
	inline void set_orientation(display_orientation orientation) noexcept
	{
		m_orientation = orientation;
	}

	/// Returns the index of the display.
	[[nodiscard]] inline const int& get_index() const noexcept
	{
		return m_index;
	}
	
	/// Returns the name of the display.
	[[nodiscard]] inline const std::string& get_name() const noexcept
	{
		return m_name;
	}
	
	/// Returns the bounds of the display, in display units.
	[[nodiscard]] inline const geom::rectangle<int>& get_bounds() const noexcept
	{
		return m_bounds;
	}
	
	/// Returns the usable bounds of the display, which excludes areas reserved by the OS for things like menus or docks, in display units.
	[[nodiscard]] inline const geom::rectangle<int>& get_usable_bounds() const noexcept
	{
		return m_usable_bounds;
	}
	
	/// Returns the refresh rate of the display, in Hz.
	[[nodiscard]] inline const int& get_refresh_rate() const noexcept
	{
		return m_refresh_rate;
	}
	
	/// Returns the DPI of the display.
	[[nodiscard]] inline const float& get_dpi() const noexcept
	{
		return m_dpi;
	}
	
	/// Returns the current orientation of the display.
	[[nodiscard]] inline const display_orientation& get_orientation() const noexcept
	{
		return m_orientation;
	}
	
	/// Returns `true` if the display is connected, `false` otherwise.
	[[nodiscard]] inline const bool& is_connected() const noexcept
	{
		return m_connected;
	}
	
	/// Returns the channel through which display connected events are published.
	[[nodiscard]] inline event::channel<display_connected_event>& get_connected_channel() noexcept
	{
		return m_connected_publisher.channel();
	}
	
	/// Returns the channel through which display disconnected events are published.
	[[nodiscard]] inline event::channel<display_disconnected_event>& get_disconnected_channel() noexcept
	{
		return m_disconnected_publisher.channel();
	}
	
	/// Returns the channel through which display orientation changed events are published.
	[[nodiscard]] inline event::channel<display_orientation_changed_event>& get_orientation_changed_channel() noexcept
	{
		return m_orientation_changed_publisher.channel();
	}
	
private:
	friend class window_manager;
	friend class sdl_window_manager;
	
	int m_index{0};
	std::string m_name;
	geom::rectangle<int> m_bounds{0};
	geom::rectangle<int> m_usable_bounds{0};
	int m_refresh_rate{0};
	float m_dpi{0.0f};
	display_orientation m_orientation{0};
	bool m_connected{false};
	
	event::publisher<display_connected_event> m_connected_publisher;
	event::publisher<display_disconnected_event> m_disconnected_publisher;
	event::publisher<display_orientation_changed_event> m_orientation_changed_publisher;
};

} // namespace app

#endif // ANTKEEPER_APP_DISPLAY_HPP
