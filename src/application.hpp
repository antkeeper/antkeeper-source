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

#include <array>
#include <functional>
#include <list>
#include <memory>
#include <unordered_map>

// Forward declarations
typedef struct SDL_Window SDL_Window;
typedef void* SDL_GLContext;
class event_dispatcher;
class frame_scheduler;
class game_controller;
class keyboard;
class mouse;
class rasterizer;
class image;

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
	typedef std::array<std::function<void()>, 2> state_type;
	typedef std::function<int(application*)> bootloader_type;
	typedef std::function<void(double, double)> update_callback_type;
	typedef std::function<void(double)> render_callback_type;
	
	/**
	 * Creates and initializes an application.
	 */
	application();
	
	/**
	 * Destroys an application.
	 */
	~application();

	/**
	 * Executes the application, causing it to run the bootloader then enter the execution loop until closed.
	 *
	 * @param bootloader Function which will be executed immediately before the execution loop is entered. The bootloader will be passed a pointer to this application and should return an exit status code.
	 *
	 * @return Exit status code.
	 */
	int execute(bootloader_type bootloader);
	
	/**
	 * Requests the application's execution loop to cleanly terminate, and specifies its exit status code.
	 *
	 * @param status Status to be returned by application::execute() upon execution loop termination.
	 */
	void close(int status);
	
	/**
	 * Changes the applications state, resulting in the execution of the current state's exit function (if any), followed by the new state's enter function (if any).
	 *
	 * @param state A pair of enter and exit functions, respectively, which define the state.
	 */
	void change_state(const state_type& state);
	
	/**
	 * Captures a screenshot of the most recently rendered frame.
	 *
	 * @return Image containing the captured frame.
	 */
	std::shared_ptr<image> capture_frame() const;
	
	/**
	 * Saves a PNG screenshot of the most recently rendered frame.
	 *
	 * @param path File path to the where the screenshot should be saved.
	 */
	void save_frame(const std::string& path) const;
	
	/**
	 * Sets the update callback, which is executed at regular intervals until the application is closed. The update callback expects two parameters, the first being the total time in seconds since the application was executed (t), and the second being the time in seconds since the last update (dt). dt will always be a fixed value, and is determined by the user-specified update rate.
	 *
	 * @see application::set_update_rate()
	 */
	void set_update_callback(const update_callback_type& callback);
	
	/**
	 * Sets the render callback, which is executed as many times as possible between update callbacks. The render callback expects one parameter, alpha, which is always between 0 and 1 and can be used to interpolate between update states.
	 */
	void set_render_callback(const render_callback_type& callback);
	
	/**
	 * Sets the frequency with which the update callback should be called.
	 *
	 * @param frequency Number of times per second the update callback should be called.
	 */
	void set_update_rate(double frequency);
	
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
	 * @param vsync `true` if v-sync should be enabled, `false` otherwise.
	 */
	void set_vsync(bool vsync);
	
	void set_window_opacity(float opacity);
	
	void swap_buffers();
	
	/// Returns the dimensions of the current display.
	const std::array<int, 2>& get_display_dimensions() const;
	
	/// Returns the dimensions of the window.
	const std::array<int, 2>& get_window_dimensions() const;
	
	/// Returns the dimensions of the window's drawable viewport.
	const std::array<int, 2>& get_viewport_dimensions() const;
	
	/// Returns `true` if the window is in fullscreen mode, `false` otherwise.
	bool is_fullscreen() const;
	
	/// Returns the rasterizer for the window.
	::rasterizer* get_rasterizer();
	
	/// Returns the application logger.
	debug::logger* get_logger();
	
	/// Returns the virtual keyboard.
	::keyboard* get_keyboard();
	
	/// Returns the virtual mouse.
	::mouse* get_mouse();
	
	/// Returns the list of virtual game controllers.
	const std::list<game_controller*>& get_game_controllers();
	
	/// Returns the application's event dispatcher.
	event_dispatcher* get_event_dispatcher();

private:
	void update(double t, double dt);
	void render(double alpha);
	
	void translate_sdl_events();
	void window_resized();
	
	bool closed;
	int exit_status;
	state_type state;
	update_callback_type update_callback;
	render_callback_type render_callback;
	bool fullscreen;
	bool vsync;
	bool cursor_visible;
	std::array<int, 2> display_dimensions;
	std::array<int, 2> window_dimensions;
	std::array<int, 2> viewport_dimensions;
	std::array<int, 2> mouse_position;
	double update_rate;
	debug::logger* logger;

	SDL_Window* sdl_window;
	SDL_GLContext sdl_gl_context;
	
	rasterizer* rasterizer;
	
	// Frame timing
	frame_scheduler* frame_scheduler;
	debug::performance_sampler* performance_sampler;
	
	// Events
	event_dispatcher* event_dispatcher;

	// Input devices
	keyboard* keyboard;
	mouse* mouse;
	std::list<game_controller*> game_controllers;
	std::unordered_map<int, game_controller*> game_controller_map;
};

inline debug::logger* application::get_logger()
{
	return logger;
}

inline const std::array<int, 2>& application::get_display_dimensions() const
{
	return display_dimensions;
}

inline const std::array<int, 2>& application::get_window_dimensions() const
{
	return window_dimensions;
}

inline const std::array<int, 2>& application::get_viewport_dimensions() const
{
	return viewport_dimensions;
}

inline bool application::is_fullscreen() const
{
	return fullscreen;
}

inline rasterizer* application::get_rasterizer()
{
	return rasterizer;
}

inline keyboard* application::get_keyboard()
{
	return keyboard;
}

inline mouse* application::get_mouse()
{
	return mouse;
}

inline const std::list<game_controller*>& application::get_game_controllers()
{
	return game_controllers;
}

inline event_dispatcher* application::get_event_dispatcher()
{
	return event_dispatcher;
}

#endif // ANTKEEPER_APPLICATION_HPP
