// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/app/sdl/sdl-window-manager.hpp>
#include <engine/app/sdl/sdl-window.hpp>
#include <engine/debug/log.hpp>
#include <engine/config.hpp>
#include <engine/gl/pipeline.hpp>
#include <stdexcept>

namespace app {

sdl_window_manager::sdl_window_manager()
{
	// Init SDL events and video subsystems
	debug::log_trace("Initializing SDL events and video subsystems...");
	if (SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_VIDEO) != 0)
	{
		debug::log_fatal("Failed to initialize SDL events and video subsystems: {}", SDL_GetError());
		throw std::runtime_error("Failed to initialize SDL events and video subsystems");
	}
	debug::log_trace("Initialized SDL events and video subsystems");
	
	// Render native IME
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
	
	// Disable unused events
	SDL_EventState(SDL_AUDIODEVICEADDED, SDL_IGNORE);
	SDL_EventState(SDL_AUDIODEVICEREMOVED, SDL_IGNORE);
	SDL_EventState(SDL_RENDER_TARGETS_RESET, SDL_IGNORE);
	SDL_EventState(SDL_RENDER_DEVICE_RESET, SDL_IGNORE);
	SDL_EventState(SDL_USEREVENT, SDL_IGNORE);
	
	// Query displays
	const int display_count = SDL_GetNumVideoDisplays();
	if (display_count < 1)
	{
		debug::log_warning("No displays detected: {}", SDL_GetError());
	}
	else
	{
		// Allocate displays
		m_displays.resize(display_count);
		debug::log_info("Display count: {}", display_count);
		
		for (int i = 0; i < display_count; ++i)
		{
			// Update display state
			update_display(i);
			
			// Log display information
			display& display = m_displays[i];
			const auto display_resolution = display.get_bounds().size();
			debug::log_info("Display {} name: \"{}\"; resolution: {}x{}; refresh rate: {}Hz; DPI: {}", i, display.get_name(), display_resolution.x(), display_resolution.y(), display.get_refresh_rate(), display.get_dpi());
		}
	}
	
	// Load OpenGL library
	debug::log_trace("Loading OpenGL library...");
	if (SDL_GL_LoadLibrary(nullptr) != 0)
	{
		debug::log_fatal("Failed to load OpenGL library: {}", SDL_GetError());
		throw std::runtime_error("Failed to load OpenGL library");
	}
	debug::log_trace("Loaded OpenGL library");
	
	// Set OpenGL-related window creation hints
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, config::opengl_version_major);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, config::opengl_version_minor);
	
	#if defined(DEBUG)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG | SDL_GL_CONTEXT_DEBUG_FLAG);
	#else
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	#endif
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, config::opengl_min_red_size);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, config::opengl_min_green_size);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, config::opengl_min_blue_size);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, config::opengl_min_alpha_size);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, config::opengl_min_depth_size);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, config::opengl_min_stencil_size);
	SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 0);
}

sdl_window_manager::~sdl_window_manager()
{
	// Quit SDL video subsystem
	debug::log_trace("Quitting SDL video subsystem...");
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	debug::log_trace("Quit SDL video subsystem");
}

std::shared_ptr<window> sdl_window_manager::create_window
(
	const std::string& title,
	const math::ivec2& windowed_position,
	const math::ivec2& windowed_size,
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
	m_window_map[window->m_internal_window] = window.get();
	
	return window;
}

void sdl_window_manager::update()
{
	// Gather SDL events from event queue
	SDL_PumpEvents();
	
	for (;;)
	{
		// Get next window or displayp event
		SDL_Event event;
		int status = SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_DISPLAYEVENT, SDL_SYSWMEVENT);
		
		if (!status)
		{
			break;
		}
		else if (status < 0)
		{
			debug::log_error("Failed to peep SDL events: {}", SDL_GetError());
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
					window->m_size = {event.window.data1, event.window.data2};
					const auto window_flags = SDL_GetWindowFlags(internal_window);					
					if (!(window_flags & SDL_WINDOW_MAXIMIZED) && !(window_flags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)))
					{
						window->m_windowed_size = window->m_size;
					}
					SDL_GL_GetDrawableSize(internal_window, &window->m_viewport_size.x(), &window->m_viewport_size.y());
					
					// Change reported dimensions of graphics pipeline default framebuffer
					window->m_graphics_pipeline->defaut_framebuffer_resized
					(
						static_cast<std::uint32_t>(window->m_viewport_size.x()),
						static_cast<std::uint32_t>(window->m_viewport_size.y())
					);
					
					// Log window resized event
					debug::log_debug("Window {} resized to {}x{}", event.window.windowID, event.window.data1, event.window.data2);
					
					// Publish window resized event
					window->m_resized_publisher.publish({window, window->m_size});
					break;
				}
				
				case SDL_WINDOWEVENT_MOVED:
				{
					// Get window
					SDL_Window* internal_window = SDL_GetWindowFromID(event.window.windowID);
					app::sdl_window* window = get_window(internal_window);
					
					// Update window state
					window->m_position = {event.window.data1, event.window.data2};
					const auto window_flags = SDL_GetWindowFlags(internal_window);
					if (!(window_flags & SDL_WINDOW_MAXIMIZED) && !(window_flags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)))
					{
						window->m_windowed_position = window->m_position;
					}
					
					// Log window moved event
					debug::log_debug("Window {} moved to ({}, {})", event.window.windowID, event.window.data1, event.window.data2);
					
					// Publish window moved event
					window->m_moved_publisher.publish({window, window->m_position});
					break;
				}
				
				case SDL_WINDOWEVENT_FOCUS_GAINED:
				{
					// Get window
					SDL_Window* internal_window = SDL_GetWindowFromID(event.window.windowID);
					app::sdl_window* window = get_window(internal_window);
					
					// Log window focus gained event
					debug::log_debug("Window {} gained focus", event.window.windowID);
					
					// Publish window focus gained event
					window->m_focus_changed_publisher.publish({window, true});
					break;
				}
				
				case SDL_WINDOWEVENT_FOCUS_LOST:
				{
					// Get window
					SDL_Window* internal_window = SDL_GetWindowFromID(event.window.windowID);
					app::sdl_window* window = get_window(internal_window);
					
					// Log window focus lost event
					debug::log_debug("Window {} lost focus", event.window.windowID);
					
					// Publish window focus lost event
					window->m_focus_changed_publisher.publish({window, false});
					break;
				}
				
				case SDL_WINDOWEVENT_MAXIMIZED:
				{
					// Get window
					SDL_Window* internal_window = SDL_GetWindowFromID(event.window.windowID);
					app::sdl_window* window = get_window(internal_window);
					
					// Update window state
					window->m_maximized = true;
					
					// Log window focus gained event
					debug::log_debug("Window {} maximized", event.window.windowID);
					
					// Publish window maximized event
					window->m_maximized_publisher.publish({window});
					break;
				}
				
				case SDL_WINDOWEVENT_RESTORED:
				{
					// Get window
					SDL_Window* internal_window = SDL_GetWindowFromID(event.window.windowID);
					app::sdl_window* window = get_window(internal_window);
					
					// Update window state
					window->m_maximized = false;
					
					// Log window restored event
					debug::log_debug("Window {} restored", event.window.windowID);
					
					// Publish window restored event
					window->m_restored_publisher.publish({window});
					break;
				}
				
				case SDL_WINDOWEVENT_MINIMIZED:
				{
					// Get window
					SDL_Window* internal_window = SDL_GetWindowFromID(event.window.windowID);
					app::sdl_window* window = get_window(internal_window);
					
					// Log window focus gained event
					debug::log_debug("Window {} minimized", event.window.windowID);
					
					// Publish window minimized event
					window->m_minimized_publisher.publish({window});
					break;
				}
				
				[[unlikely]] case SDL_WINDOWEVENT_CLOSE:
				{
					// Get window
					SDL_Window* internal_window = SDL_GetWindowFromID(event.window.windowID);
					app::sdl_window* window = get_window(internal_window);
					
					// Log window closed event
					debug::log_debug("Window {} closed", event.window.windowID);
					
					// Publish window closed event
					window->m_closed_publisher.publish({window});
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
					if (event.display.display < m_displays.size())
					{
						// Get previously connected display
						display& display = m_displays[event.display.display];
						
						// Update display state
						display.m_connected = true;
						
						// Log display (re)connected event
						debug::log_info("Reconnected display {}", event.display.display);
						
						// Publish display (re)connected event
						display.m_connected_publisher.publish({&display});
					}
					else if (event.display.display == m_displays.size())
					{
						// Allocate new display
						m_displays.resize(m_displays.size() + 1);
						display& display = m_displays[event.display.display];
						
						// Update display state
						update_display(event.display.display);
						
						// Log display info
						const auto display_resolution = display.get_bounds().size();
						debug::log_info("Connected display {}; name: \"{}\"; resolution: {}x{}; refresh rate: {}Hz; DPI: {}", event.display.display, display.get_name(), display_resolution.x(), display_resolution.y(), display.get_refresh_rate(), display.get_dpi());
						
						// Publish display connected event
						display.m_connected_publisher.publish({&display});
					}
					else
					{
						debug::log_error("Index of connected display ({}) out of range", event.display.display);
					}
					break;
				
				case SDL_DISPLAYEVENT_DISCONNECTED:
					if (event.display.display < m_displays.size())
					{
						// Get display
						display& display = m_displays[event.display.display];
						
						// Update display state
						display.m_connected = false;
						
						// Log display disconnected event
						debug::log_info("Disconnected display {}", event.display.display);
						
						// Publish display disconnected event
						display.m_disconnected_publisher.publish({&display});
					}
					else
					{
						debug::log_error("Index of disconnected display ({}) out of range", event.display.display);
					}
					break;
				
				case SDL_DISPLAYEVENT_ORIENTATION:
					if (event.display.display < m_displays.size())
					{
						// Get display
						display& display = m_displays[event.display.display];
						
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
						debug::log_info("Display {} orientation changed", event.display.display);
						
						// Publish display orientation changed event
						display.m_orientation_changed_publisher.publish({&display, display.get_orientation()});
					}
					else
					{
						debug::log_error("Index of orientation-changed display ({}) out of range", event.display.display);
					}
					break;
				
				default:
					break;
			}
		}
	}
	
	for (;;)
	{
		// Get next drop event
		SDL_Event event;
		int status = SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_DROPFILE, SDL_DROPCOMPLETE);
		
		if (!status)
		{
			break;
		}
		else if (status < 0)
		{
			debug::log_error("Failed to peep SDL events: {}", SDL_GetError());
			throw std::runtime_error("Failed to peep SDL events");
		}
		
		switch (event.type)
		{
			case SDL_DROPFILE:
			{
				// Get window
				auto window = get_window(SDL_GetWindowFromID(event.drop.windowID));
				
				// Publish drop file event
				window->m_drop_file_publisher.publish({window, std::filesystem::path(event.drop.file)});
				
				// Free file path
				SDL_free(event.drop.file);
				
				break;
			}
			
			case SDL_DROPTEXT:
			{
				// Get window
				auto window = get_window(SDL_GetWindowFromID(event.drop.windowID));
				
				// Publish drop text event
				window->m_drop_text_publisher.publish({window, std::string(event.drop.file)});
				
				// Free text
				SDL_free(event.drop.file);
				
				break;
			}
			
			case SDL_DROPBEGIN:
			{
				// Get window
				auto window = get_window(SDL_GetWindowFromID(event.drop.windowID));
				
				// Publish drop begin event
				window->m_drop_begin_publisher.publish({window});
				
				break;
			}
			
			case SDL_DROPCOMPLETE:
			{
				// Get window
				auto window = get_window(SDL_GetWindowFromID(event.drop.windowID));
				
				// Publish drop end event
				window->m_drop_end_publisher.publish({window});
				
				break;
			}
			
			default:
				break;
		}
	}
}

sdl_window* sdl_window_manager::get_window(SDL_Window* internal_window)
{
	auto it = m_window_map.find(internal_window);
	if (it == m_window_map.end())
	{
		throw std::runtime_error("SDL window unrecognized by SDL window manager");
	}
	
	return it->second;
}

std::size_t sdl_window_manager::get_display_count() const
{
	return m_displays.size();
}

const display& sdl_window_manager::get_display(std::size_t index) const
{
	return m_displays[index];
}

void sdl_window_manager::update_display(int sdl_display_index)
{
	// Query display mode
	SDL_DisplayMode sdl_display_mode;
	if (SDL_GetDesktopDisplayMode(sdl_display_index, &sdl_display_mode) != 0)
	{
		debug::log_error("Failed to get mode of display {}: {}", sdl_display_index, SDL_GetError());
		SDL_ClearError();
		sdl_display_mode = {0, 0, 0, 0, nullptr};
	}
	
	// Query display name
	const char* sdl_display_name = SDL_GetDisplayName(sdl_display_index);
	if (!sdl_display_name)
	{
		debug::log_warning("Failed to get name of display {}: {}", sdl_display_index, SDL_GetError());
		SDL_ClearError();
		sdl_display_name = nullptr;
	}
	
	// Query display bounds
	SDL_Rect sdl_display_bounds;
	if (SDL_GetDisplayBounds(sdl_display_index, &sdl_display_bounds) != 0)
	{
		debug::log_warning("Failed to get bounds of display {}: {}", sdl_display_index, SDL_GetError());
		SDL_ClearError();
		sdl_display_bounds = {0, 0, sdl_display_mode.w, sdl_display_mode.h};
	}
	
	// Query display usable bounds
	SDL_Rect sdl_display_usable_bounds;
	if (SDL_GetDisplayUsableBounds(sdl_display_index, &sdl_display_usable_bounds) != 0)
	{
		debug::log_warning("Failed to get usable bounds of display {}: {}", sdl_display_index, SDL_GetError());
		SDL_ClearError();
		sdl_display_usable_bounds = sdl_display_bounds;
	}
	
	// Query display DPI
	float sdl_display_dpi;
	if (SDL_GetDisplayDPI(sdl_display_index, &sdl_display_dpi, nullptr, nullptr) != 0)
	{
		debug::log_warning("Failed to get DPI of display {}: {}", sdl_display_index, SDL_GetError());
		SDL_ClearError();
		sdl_display_dpi = 0.0f;
	}
	
	// Query display orientation
	SDL_DisplayOrientation sdl_display_orientation = SDL_GetDisplayOrientation(sdl_display_index);
	
	// Update display properties
	display& display = m_displays[sdl_display_index];
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
		case SDL_ORIENTATION_UNKNOWN:
			[[fallthrough]];
		default:
			display.set_orientation(display_orientation::unknown);
			break;
	}
	display.m_connected = true;
}

} // namespace app
