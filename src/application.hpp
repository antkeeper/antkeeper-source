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

#ifndef ANTKEEPER_APPLICATION_HPP
#define ANTKEEPER_APPLICATION_HPP

#include <list>
#include <memory>
#include <unordered_map>
#include "gl/rasterizer.hpp"
#include "input/keyboard.hpp"
#include "input/mouse.hpp"
#include "input/gamepad.hpp"
#include "utility/fundamental-types.hpp"

// Forward declarations
typedef struct SDL_Window SDL_Window;
typedef void* SDL_GLContext;
class event_dispatcher;

namespace debug
{
	class logger;
	class performance_sampler;
}

/**
 * 
 */
class application
{
public:
	/**
	 * Creates and initializes an application.
	 */
	application();
	
	/**
	 * Destroys an application.
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
	 * Puts the application window into either fullscreen or windowed mode.
	 *
	 * @param fullscreen `true` if the window should be fullscreen, `false` if it should be windowed.
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
	const int2& get_display_dimensions() const;
	
	/// Returns the DPI of the display.
	float get_display_dpi() const;
	
	/// Returns the dimensions of the window.
	const int2& get_window_dimensions() const;
	
	/// Returns the dimensions of the window's drawable viewport.
	const int2& get_viewport_dimensions() const;
	
	/// Returns `true` if the window is in fullscreen mode, `false` otherwise.
	bool is_fullscreen() const;
	
	/// Returns `true` if the v-sync is enabled, `false` otherwise.
	bool get_v_sync() const;
	
	/// Returns the rasterizer for the window.
	gl::rasterizer* get_rasterizer();
	
	/// Returns the application logger.
	debug::logger* get_logger();
	
	/// Returns the virtual keyboard.
	input::keyboard* get_keyboard();
	
	/// Returns the virtual mouse.
	input::mouse* get_mouse();
	
	/// Returns the list of virtual gamepads.
	const std::list<input::gamepad*>& get_gamepads();
	
	/// Returns the application's event dispatcher.
	event_dispatcher* get_event_dispatcher();
	
	void process_events();
	
	bool was_closed() const;

private:
	void window_resized();
	
	bool closed;
	bool fullscreen;
	bool v_sync;
	bool cursor_visible;
	int2 display_dimensions;
	float display_dpi;
	int2 window_dimensions;
	int2 viewport_dimensions;
	int2 mouse_position;
	debug::logger* logger;

	SDL_Window* sdl_window;
	SDL_GLContext sdl_gl_context;
	
	gl::rasterizer* rasterizer;
	
	// Events
	event_dispatcher* event_dispatcher;

	// Input devices
	input::keyboard* keyboard;
	input::mouse* mouse;
	std::list<input::gamepad*> gamepads;
	std::unordered_map<int, input::gamepad*> gamepad_map;
};

inline debug::logger* application::get_logger()
{
	return logger;
}

inline const int2& application::get_display_dimensions() const
{
	return display_dimensions;
}

inline float application::get_display_dpi() const
{
	return display_dpi;
}

inline const int2& application::get_window_dimensions() const
{
	return window_dimensions;
}

inline const int2& application::get_viewport_dimensions() const
{
	return viewport_dimensions;
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

inline input::keyboard* application::get_keyboard()
{
	return keyboard;
}

inline input::mouse* application::get_mouse()
{
	return mouse;
}

inline const std::list<input::gamepad*>& application::get_gamepads()
{
	return gamepads;
}

inline event_dispatcher* application::get_event_dispatcher()
{
	return event_dispatcher;
}

inline bool application::was_closed() const
{
	return closed;
}

#endif // ANTKEEPER_APPLICATION_HPP
