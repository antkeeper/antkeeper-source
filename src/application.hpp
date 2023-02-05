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

#ifndef ANTKEEPER_APPLICATION_HPP
#define ANTKEEPER_APPLICATION_HPP

#include "event/publisher.hpp"
#include "gl/rasterizer.hpp"
#include "input/device-manager.hpp"
#include "input/event.hpp"
#include "input/gamepad.hpp"
#include "input/keyboard.hpp"
#include "input/mouse.hpp"
#include "utility/fundamental-types.hpp"
#include <list>
#include <memory>
#include <unordered_map>

// Forward declarations
typedef struct SDL_Window SDL_Window;
typedef void* SDL_GLContext;

/**
 * 
 */
class application
{
public:
	/**
	 * Constructs and initializes an application.
	 */
	application
	(
		const std::string& window_title,
		int window_x,
		int window_y,
		int window_w,
		int window_h,
		bool maximized,
		bool fullscreen,
		bool v_sync
	);
	
	/**
	 * Destructs an application.
	 */
	~application();
	
	/**
	 * Requests the application to close.
	 */
	void close();
	
	/**
	 * Sets the application window's title.
	 *
	 * @param title Window title.
	 */
	void set_title(const std::string& title);
	
	/**
	 * Sets the cursor visibility.
	 *
	 * @param visible `true` if the cursor should be visible, `false` otherwise.
	 */
	void set_cursor_visible(bool visible);
	
	/**
	 * Enables or disables relative mouse mode, in which only relative mouse movement events are generated.
	 *
	 * @param enabled `true` if relative mouse mode should be enabled, `false` otherwise.
	 */
	void set_relative_mouse_mode(bool enabled);
	
	/**
	 * Resizes the application window.
	 *
	 * @param width Width of the window, in pixels.
	 * @param height Height of the window, in pixels.
	 */
	void resize_window(int width, int height);
	
	/**
	 * Puts the application window into either fullscreen or window mode.
	 *
	 * @param fullscreen `true` if the window should be fullscreen, `false` if it should be window.
	 */
	void set_fullscreen(bool fullscreen);
	
	/**
	 * Enables or disables v-sync mode.
	 *
	 * @param v_sync `true` if v-sync should be enabled, `false` otherwise.
	 */
	void set_v_sync(bool v_sync);
	
	void set_window_opacity(float opacity);
	
	void swap_buffers();
	
	void show_window();
	void hide_window();
	
	void add_game_controller_mappings(const void* mappings, std::size_t size);
	
	/// Returns the dimensions of the current display.
	[[nodiscard]] const int2& get_display_size() const;
	
	/// Returns the DPI of the display.
	[[nodiscard]] float get_display_dpi() const;
	
	/// Returns the position of the window when not maximized or fullscreen.
	[[nodiscard]] const int2& get_windowed_position() const;
	
	/// Returns the dimensions of the window when not maximized or fullscreen.
	[[nodiscard]] const int2& get_windowed_size() const;
	
	/// Returns the dimensions of the window's drawable viewport.
	[[nodiscard]] const int2& get_viewport_size() const;
	
	/// Returns `true` if the window is maximized, `false` otherwise.
	[[nodiscard]] bool is_maximized() const;
	
	/// Returns `true` if the window is in fullscreen mode, `false` otherwise.
	[[nodiscard]] bool is_fullscreen() const;
	
	/// Returns `true` if the v-sync is enabled, `false` otherwise.
	[[nodiscard]] bool get_v_sync() const;
	
	/// Returns the rasterizer for the window.
	[[nodiscard]] gl::rasterizer* get_rasterizer();
	
	void process_events();
	
	[[nodiscard]] bool was_closed() const;
	
	/**
	 * Returns the input device manager.
	 */
	/// @{
	[[nodiscard]] inline const input::device_manager& get_device_manager() const noexcept
	{
		return device_manager;
	}
	[[nodiscard]] inline input::device_manager& get_device_manager() noexcept
	{
		return device_manager;
	}
	/// @}
	
	/// Returns the channel through which window closed events are published.
	[[nodiscard]] inline event::channel<input::event::window_closed>& get_window_closed_channel() noexcept
	{
		return window_closed_publisher.channel();
	}
	
	/// Returns the channel through which window focus changed events are published.
	[[nodiscard]] inline event::channel<input::event::window_focus_changed>& get_window_focus_changed_channel() noexcept
	{
		return window_focus_changed_publisher.channel();
	}
	
	/// Returns the channel through which window moved events are published.
	[[nodiscard]] inline event::channel<input::event::window_moved>& get_window_moved_channel() noexcept
	{
		return window_moved_publisher.channel();
	}
	
	/// Returns the channel through which window resized events are published.
	[[nodiscard]] inline event::channel<input::event::window_resized>& get_window_resized_channel() noexcept
	{
		return window_resized_publisher.channel();
	}
	
	/// Returns the channel through which window maximized events are published.
	[[nodiscard]] inline event::channel<input::event::window_maximized>& get_window_maximized_channel() noexcept
	{
		return window_maximized_publisher.channel();
	}
	
	/// Returns the channel through which window restored events are published.
	[[nodiscard]] inline event::channel<input::event::window_restored>& get_window_restored_channel() noexcept
	{
		return window_restored_publisher.channel();
	}
	
	/// Returns the channel through which window minimized events are published.
	[[nodiscard]] inline event::channel<input::event::window_minimized>& get_window_minimized_channel() noexcept
	{
		return window_minimized_publisher.channel();
	}
	
private:
	void window_moved();
	void window_resized();
	
	bool closed;
	bool maximized;
	bool fullscreen;
	bool v_sync;
	bool cursor_visible;
	int2 display_size;
	float display_dpi;
	int2 windowed_position;
	int2 windowed_size;
	int2 viewport_size;
	int2 mouse_position;
	
	SDL_Window* sdl_window;
	SDL_GLContext sdl_gl_context;
	
	gl::rasterizer* rasterizer;
	
	// Input devices
	input::device_manager device_manager;
	input::keyboard keyboard;
	input::mouse mouse;
	std::unordered_map<int, input::gamepad*> gamepad_map;
	
	event::publisher<input::event::window_closed> window_closed_publisher;
	event::publisher<input::event::window_focus_changed> window_focus_changed_publisher;
	event::publisher<input::event::window_moved> window_moved_publisher;
	event::publisher<input::event::window_resized> window_resized_publisher;
	event::publisher<input::event::window_maximized> window_maximized_publisher;
	event::publisher<input::event::window_restored> window_restored_publisher;
	event::publisher<input::event::window_minimized> window_minimized_publisher;
};

inline const int2& application::get_display_size() const
{
	return display_size;
}

inline float application::get_display_dpi() const
{
	return display_dpi;
}

inline const int2& application::get_windowed_position() const
{
	return windowed_position;
}

inline const int2& application::get_windowed_size() const
{
	return windowed_size;
}

inline const int2& application::get_viewport_size() const
{
	return viewport_size;
}

inline bool application::is_maximized() const
{
	return maximized;
}

inline bool application::is_fullscreen() const
{
	return fullscreen;
}

inline bool application::get_v_sync() const
{
	return v_sync;
}

inline gl::rasterizer* application::get_rasterizer()
{
	return rasterizer;
}

inline bool application::was_closed() const
{
	return closed;
}

#endif // ANTKEEPER_APPLICATION_HPP
