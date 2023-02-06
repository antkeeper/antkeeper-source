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

#include "app/sdl/sdl-window-manager.hpp"
#include "app/sdl/sdl-window.hpp"
#include "debug/log.hpp"
#include "config.hpp"
#include <stdexcept>

namespace app {

sdl_window_manager::sdl_window_manager()
{
	// Init SDL events and video subsystems
	debug::log::trace("Initializing SDL events and video subsystems...");
	if (SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_VIDEO) != 0)
	{
		debug::log::fatal("Failed to initialize SDL events and video subsystems: {}", SDL_GetError());
		throw std::runtime_error("Failed to initialize SDL events and video subsystems");
	}
	debug::log::trace("Initialized SDL events and video subsystems");
	
	// Query displays
	const int display_count = SDL_GetNumVideoDisplays();
	if (display_count < 1)
	{
		debug::log::warning("No displays detected: {}", SDL_GetError());
	}
	else
	{
		debug::log::info("Display count: {}", display_count);
		
		displays.resize(display_count);
		for (int i = 0; i < display_count; ++i)
		{
			// Query display mode
			SDL_DisplayMode display_mode;
			if (SDL_GetDesktopDisplayMode(i, &display_mode) != 0)
			{
				debug::log::error("Failed to get mode of display {}: {}", i, SDL_GetError());
				SDL_ClearError();
				continue;
			}
			
			// Query display name
			const char* display_name = SDL_GetDisplayName(i);
			if (!display_name)
			{
				debug::log::warning("Failed to get name of display {}: {}", i, SDL_GetError());
				SDL_ClearError();
				display_name = "";
			}
			
			// Query display DPI
			float display_dpi;
			if (SDL_GetDisplayDPI(i, &display_dpi, nullptr, nullptr) != 0)
			{
				const float default_dpi = 96.0f;
				debug::log::warning("Failed to get DPI of display {}: {}; Defaulting to {} DPI", i, SDL_GetError(), default_dpi);
				SDL_ClearError();
			}
			
			// Update display properties
			display& display = displays[i];
			display.set_index(i);
			display.set_name(display_name);
			display.set_size({display_mode.w, display_mode.h});
			display.set_refresh_rate(display_mode.refresh_rate);
			display.set_dpi(display_dpi);
			
			// Log display information
			debug::log::info("Display {} name: \"{}\"; resolution: {}x{}; refresh rate: {}Hz; DPI: {}", i, display_name, display_mode.w, display_mode.h, display_mode.refresh_rate, display_dpi);
		}
	}
	
	// Load OpenGL library
	debug::log::trace("Loading OpenGL library...");
	if (SDL_GL_LoadLibrary(nullptr) != 0)
	{
		debug::log::fatal("Failed to load OpenGL library: {}", SDL_GetError());
		throw std::runtime_error("Failed to load OpenGL library");
	}
	debug::log::trace("Loaded OpenGL library");
	
	// Set OpenGL-related window creation hints
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, config::opengl_version_major);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, config::opengl_version_minor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, config::opengl_min_red_size);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, config::opengl_min_green_size);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, config::opengl_min_blue_size);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, config::opengl_min_alpha_size);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, config::opengl_min_depth_size);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, config::opengl_min_stencil_size);
}

sdl_window_manager::~sdl_window_manager()
{
	// Quit SDL video subsystem
	debug::log::trace("Quitting SDL video subsystem...");
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	debug::log::trace("Quit SDL video subsystem");
}

window* sdl_window_manager::create_window
(
	const std::string& title,
	const math::vector<int, 2>& windowed_position,
	const math::vector<int, 2>& windowed_size,
	bool maximized,
	bool fullscreen,
	bool v_sync
)
{
	// Create new window
	app::sdl_window* window = new app::sdl_window
	(
		title,
		windowed_position,
		windowed_size,
		maximized,
		fullscreen,
		v_sync
	);
	
	// Map internal SDL window to window
	window_map[window->internal_window] = window;
	
	return window;
}

void sdl_window_manager::update()
{
	// Gather SDL events from event queue
	SDL_PumpEvents();
	
	for (;;)
	{
		// Get next window or display event
		SDL_Event event;
		int status = SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_DISPLAYEVENT, SDL_SYSWMEVENT);
		
		if (!status)
		{
			break;
		}
		else if (status < 0)
		{
			debug::log::error("Failed to peep SDL events: {}", SDL_GetError());
			throw std::runtime_error("Failed to peep SDL events");
		}
		
		// Handle event
		if (event.type == SDL_WINDOWEVENT)
		{
			switch (event.window.event)
			{
				case SDL_WINDOWEVENT_SIZE_CHANGED:
				{
					// Get window
					SDL_Window* internal_window = SDL_GetWindowFromID(event.window.windowID);
					app::sdl_window* window = get_window(internal_window);
					
					// Update window state
					window->size = {event.window.data1, event.window.data2};
					const auto window_flags = SDL_GetWindowFlags(internal_window);					
					if (!(window_flags & SDL_WINDOW_MAXIMIZED) && !(window_flags & SDL_WINDOW_FULLSCREEN))
					{
						window->windowed_size = window->size;
					}
					SDL_GL_GetDrawableSize(internal_window, &window->viewport_size.x(), &window->viewport_size.y());
					window->rasterizer->context_resized(window->viewport_size.x(), window->viewport_size.y());
					
					// Publish window resized event
					window->resized_publisher.publish({window, window->size});
					break;
				}
				
				case SDL_WINDOWEVENT_MOVED:
				{
					// Get window
					SDL_Window* internal_window = SDL_GetWindowFromID(event.window.windowID);
					app::sdl_window* window = get_window(internal_window);
					
					// Update window state
					window->position = {event.window.data1, event.window.data2};
					const auto window_flags = SDL_GetWindowFlags(internal_window);
					if (!(window_flags & SDL_WINDOW_MAXIMIZED) && !(window_flags & SDL_WINDOW_FULLSCREEN))
					{
						window->windowed_position = window->position;
					}
					
					// Publish window moved event
					window->moved_publisher.publish({window, window->position});
					break;
				}
				
				case SDL_WINDOWEVENT_FOCUS_GAINED:
				{
					// Get window
					SDL_Window* internal_window = SDL_GetWindowFromID(event.window.windowID);
					app::sdl_window* window = get_window(internal_window);
					
					// Publish window focus gained event
					window->focus_changed_publisher.publish({window, true});
					break;
				}
				
				case SDL_WINDOWEVENT_FOCUS_LOST:
				{
					// Get window
					SDL_Window* internal_window = SDL_GetWindowFromID(event.window.windowID);
					app::sdl_window* window = get_window(internal_window);
					
					// Publish window focus lost event
					window->focus_changed_publisher.publish({window, false});
					break;
				}
				
				case SDL_WINDOWEVENT_MAXIMIZED:
				{
					// Get window
					SDL_Window* internal_window = SDL_GetWindowFromID(event.window.windowID);
					app::sdl_window* window = get_window(internal_window);
					
					// Update window state
					window->maximized = true;
					
					// Publish window maximized event
					window->maximized_publisher.publish({window});
					break;
				}
				
				case SDL_WINDOWEVENT_RESTORED:
				{
					// Get window
					SDL_Window* internal_window = SDL_GetWindowFromID(event.window.windowID);
					app::sdl_window* window = get_window(internal_window);
					
					// Update window state
					window->maximized = false;
					
					// Publish window restored event
					window->restored_publisher.publish({window});
					break;
				}
				
				case SDL_WINDOWEVENT_MINIMIZED:
				{
					// Get window
					SDL_Window* internal_window = SDL_GetWindowFromID(event.window.windowID);
					app::sdl_window* window = get_window(internal_window);
					
					// Publish window minimized event
					window->minimized_publisher.publish({window});
					break;
				}
				
				[[unlikely]] case SDL_WINDOWEVENT_CLOSE:
				{
					// Get window
					SDL_Window* internal_window = SDL_GetWindowFromID(event.window.windowID);
					app::sdl_window* window = get_window(internal_window);
					
					// Publish window closed event
					window->closed_publisher.publish({window});
					break;
				}
				
				default:
					break;
			}
		}
		else if (event.type == SDL_DISPLAYEVENT)
		{
			
		}
	}
}

sdl_window* sdl_window_manager::get_window(SDL_Window* internal_window)
{
	sdl_window* window = nullptr;
	if (auto i = window_map.find(internal_window); i != window_map.end())
	{
		window = i->second;
	}
	else
	{
		throw std::runtime_error("SDL window unrecognized by SDL window manager");
	}
	return window;
}

std::size_t sdl_window_manager::get_display_count() const
{
	return displays.size();
}

const display& sdl_window_manager::get_display(std::size_t index) const
{
	return displays[index];
}

} // namespace app
