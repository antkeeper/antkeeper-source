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

#include <engine/app/sdl/sdl-window-manager.hpp>
#include <engine/app/sdl/sdl-window.hpp>
#include <engine/debug/log.hpp>
#include <engine/config.hpp>
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
		// Allocate displays
		displays.resize(display_count);
		debug::log::info("Display count: {}", display_count);
		
		for (int i = 0; i < display_count; ++i)
		{
			// Update display state
			update_display(i);
			
			// Log display information
			display& display = displays[i];
			const auto display_resolution = display.get_bounds().size();
			debug::log::info("Display {} name: \"{}\"; resolution: {}x{}; refresh rate: {}Hz; DPI: {}", i, display.get_name(), display_resolution.x(), display_resolution.y(), display.get_refresh_rate(), display.get_dpi());
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

std::shared_ptr<window> sdl_window_manager::create_window
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
	std::shared_ptr<app::sdl_window> window = std::make_shared<app::sdl_window>
	(
		title,
		windowed_position,
		windowed_size,
		maximized,
		fullscreen,
		v_sync
	);
	
	// Map internal SDL window to window
	window_map[window->internal_window] = window.get();
	
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
					if (!(window_flags & SDL_WINDOW_MAXIMIZED) && !(window_flags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)))
					{
						window->windowed_size = window->size;
					}
					SDL_GL_GetDrawableSize(internal_window, &window->viewport_size.x(), &window->viewport_size.y());
					window->rasterizer->context_resized(window->viewport_size.x(), window->viewport_size.y());
					
					// Log window resized event
					debug::log::debug("Window {} resized to {}x{}", event.window.windowID, event.window.data1, event.window.data2);
					
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
					if (!(window_flags & SDL_WINDOW_MAXIMIZED) && !(window_flags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)))
					{
						window->windowed_position = window->position;
					}
					
					// Log window moved event
					debug::log::debug("Window {} moved to ({}, {})", event.window.windowID, event.window.data1, event.window.data2);
					
					// Publish window moved event
					window->moved_publisher.publish({window, window->position});
					break;
				}
				
				case SDL_WINDOWEVENT_FOCUS_GAINED:
				{
					// Get window
					SDL_Window* internal_window = SDL_GetWindowFromID(event.window.windowID);
					app::sdl_window* window = get_window(internal_window);
					
					// Log window focus gained event
					debug::log::debug("Window {} gained focus", event.window.windowID);
					
					// Publish window focus gained event
					window->focus_changed_publisher.publish({window, true});
					break;
				}
				
				case SDL_WINDOWEVENT_FOCUS_LOST:
				{
					// Get window
					SDL_Window* internal_window = SDL_GetWindowFromID(event.window.windowID);
					app::sdl_window* window = get_window(internal_window);
					
					// Log window focus lost event
					debug::log::debug("Window {} lost focus", event.window.windowID);
					
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
					
					// Log window focus gained event
					debug::log::debug("Window {} maximized", event.window.windowID);
					
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
					
					// Log window restored event
					debug::log::debug("Window {} restored", event.window.windowID);
					
					// Publish window restored event
					window->restored_publisher.publish({window});
					break;
				}
				
				case SDL_WINDOWEVENT_MINIMIZED:
				{
					// Get window
					SDL_Window* internal_window = SDL_GetWindowFromID(event.window.windowID);
					app::sdl_window* window = get_window(internal_window);
					
					// Log window focus gained event
					debug::log::debug("Window {} minimized", event.window.windowID);
					
					// Publish window minimized event
					window->minimized_publisher.publish({window});
					break;
				}
				
				[[unlikely]] case SDL_WINDOWEVENT_CLOSE:
				{
					// Get window
					SDL_Window* internal_window = SDL_GetWindowFromID(event.window.windowID);
					app::sdl_window* window = get_window(internal_window);
					
					// Log window closed event
					debug::log::debug("Window {} closed", event.window.windowID);
					
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
			switch (event.display.event)
			{
				case SDL_DISPLAYEVENT_CONNECTED:
					if (event.display.display < displays.size())
					{
						// Get previously connected display
						display& display = displays[event.display.display];
						
						// Update display state
						display.connected = true;
						
						// Log display (re)connected event
						debug::log::info("Reconnected display {}", event.display.display);
						
						// Publish display (re)connected event
						display.connected_publisher.publish({&display});
					}
					else if (event.display.display == displays.size())
					{
						// Allocate new display
						displays.resize(displays.size() + 1);
						display& display = displays[event.display.display];
						
						// Update display state
						update_display(event.display.display);
						
						// Log display info
						const auto display_resolution = display.get_bounds().size();
						debug::log::info("Connected display {}; name: \"{}\"; resolution: {}x{}; refresh rate: {}Hz; DPI: {}", event.display.display, display.get_name(), display_resolution.x(), display_resolution.y(), display.get_refresh_rate(), display.get_dpi());
						
						// Publish display connected event
						display.connected_publisher.publish({&display});
					}
					else
					{
						debug::log::error("Index of connected display ({}) out of range", event.display.display);
					}
					break;
				
				case SDL_DISPLAYEVENT_DISCONNECTED:
					if (event.display.display < displays.size())
					{
						// Get display
						display& display = displays[event.display.display];
						
						// Update display state
						display.connected = false;
						
						// Log display disconnected event
						debug::log::info("Disconnected display {}", event.display.display);
						
						// Publish display disconnected event
						display.disconnected_publisher.publish({&display});
					}
					else
					{
						debug::log::error("Index of disconnected display ({}) out of range", event.display.display);
					}
					break;
				
				case SDL_DISPLAYEVENT_ORIENTATION:
					if (event.display.display < displays.size())
					{
						// Get display
						display& display = displays[event.display.display];
						
						// Update display state
						switch (event.display.data1)
						{
							case SDL_ORIENTATION_LANDSCAPE:
								display.set_orientation(display_orientation::landscape);
								break;
							case SDL_ORIENTATION_LANDSCAPE_FLIPPED:
								display.set_orientation(display_orientation::landscape_flipped);
								break;
							case SDL_ORIENTATION_PORTRAIT:
								display.set_orientation(display_orientation::portrait);
								break;
							case SDL_ORIENTATION_PORTRAIT_FLIPPED:
								display.set_orientation(display_orientation::portrait_flipped);
								break;
							default:
								display.set_orientation(display_orientation::unknown);
								break;
						}
						
						// Log display orientation changed event
						debug::log::info("Display {} orientation changed", event.display.display);
						
						// Publish display orientation changed event
						display.orientation_changed_publisher.publish({&display, display.get_orientation()});
					}
					else
					{
						debug::log::error("Index of orientation-changed display ({}) out of range", event.display.display);
					}
					break;
				
				default:
					break;
			}
		}
	}
}

sdl_window* sdl_window_manager::get_window(SDL_Window* internal_window)
{
	if (auto i = window_map.find(internal_window); i != window_map.end())
	{
		return i->second;
	}
	else
	{
		throw std::runtime_error("SDL window unrecognized by SDL window manager");
	}
	
	return nullptr;
}

std::size_t sdl_window_manager::get_display_count() const
{
	return displays.size();
}

const display& sdl_window_manager::get_display(std::size_t index) const
{
	return displays[index];
}

void sdl_window_manager::update_display(int sdl_display_index)
{
	// Query display mode
	SDL_DisplayMode sdl_display_mode;
	if (SDL_GetDesktopDisplayMode(sdl_display_index, &sdl_display_mode) != 0)
	{
		debug::log::error("Failed to get mode of display {}: {}", sdl_display_index, SDL_GetError());
		SDL_ClearError();
		sdl_display_mode = {0, 0, 0, 0, nullptr};
	}
	
	// Query display name
	const char* sdl_display_name = SDL_GetDisplayName(sdl_display_index);
	if (!sdl_display_name)
	{
		debug::log::warning("Failed to get name of display {}: {}", sdl_display_index, SDL_GetError());
		SDL_ClearError();
		sdl_display_name = nullptr;
	}
	
	// Query display bounds
	SDL_Rect sdl_display_bounds;
	if (SDL_GetDisplayBounds(sdl_display_index, &sdl_display_bounds) != 0)
	{
		debug::log::warning("Failed to get bounds of display {}: {}", sdl_display_index, SDL_GetError());
		SDL_ClearError();
		sdl_display_bounds = {0, 0, sdl_display_mode.w, sdl_display_mode.h};
	}
	
	// Query display usable bounds
	SDL_Rect sdl_display_usable_bounds;
	if (SDL_GetDisplayUsableBounds(sdl_display_index, &sdl_display_usable_bounds) != 0)
	{
		debug::log::warning("Failed to get usable bounds of display {}: {}", sdl_display_index, SDL_GetError());
		SDL_ClearError();
		sdl_display_usable_bounds = sdl_display_bounds;
	}
	
	// Query display DPI
	float sdl_display_dpi;
	if (SDL_GetDisplayDPI(sdl_display_index, &sdl_display_dpi, nullptr, nullptr) != 0)
	{
		debug::log::warning("Failed to get DPI of display {}: {}", sdl_display_index, SDL_GetError());
		SDL_ClearError();
		sdl_display_dpi = 0.0f;
	}
	
	// Query display orientation
	SDL_DisplayOrientation sdl_display_orientation = SDL_GetDisplayOrientation(sdl_display_index);
	
	// Update display properties
	display& display = displays[sdl_display_index];
	display.set_index(static_cast<std::size_t>(sdl_display_index));
	display.set_name(sdl_display_name ? sdl_display_name : std::string());
	display.set_bounds({{sdl_display_bounds.x, sdl_display_bounds.y}, {sdl_display_bounds.x + sdl_display_bounds.w, sdl_display_bounds.y + sdl_display_bounds.h}});
	display.set_usable_bounds({{sdl_display_usable_bounds.x, sdl_display_usable_bounds.y}, {sdl_display_usable_bounds.x + sdl_display_usable_bounds.w, sdl_display_usable_bounds.y + sdl_display_usable_bounds.h}});
	display.set_refresh_rate(sdl_display_mode.refresh_rate);
	display.set_dpi(sdl_display_dpi);
	switch (sdl_display_orientation)
	{
		case SDL_ORIENTATION_LANDSCAPE:
			display.set_orientation(display_orientation::landscape);
			break;
		case SDL_ORIENTATION_LANDSCAPE_FLIPPED:
			display.set_orientation(display_orientation::landscape_flipped);
			break;
		case SDL_ORIENTATION_PORTRAIT:
			display.set_orientation(display_orientation::portrait);
			break;
		case SDL_ORIENTATION_PORTRAIT_FLIPPED:
			display.set_orientation(display_orientation::portrait_flipped);
			break;
		default:
			display.set_orientation(display_orientation::unknown);
			break;
	}
	display.connected = true;
}

} // namespace app
