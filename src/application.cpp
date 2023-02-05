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

#include "application.hpp"
#include "config.hpp"
#include "debug/log.hpp"
#include "input/scancode.hpp"
#include "math/map.hpp"
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <stdexcept>
#include <utility>

application::application
(
	const std::string& window_title,
	int window_x,
	int window_y,
	int window_w,
	int window_h,
	bool maximized,
	bool fullscreen,
	bool v_sync
):
	closed(false),
	maximized(false),
	fullscreen(true),
	v_sync(false),
	cursor_visible(true),
	display_size({0, 0}),
	display_dpi(0.0f),
	windowed_position({-1, -1}),
	windowed_size({-1, -1}),
	viewport_size({-1, -1}),
	mouse_position({0, 0}),
	sdl_window(nullptr),
	sdl_gl_context(nullptr)
{
	// Log SDL info
	// SDL_version sdl_compiled_version;
	// SDL_version sdl_linked_version;
	// SDL_VERSION(&sdl_compiled_version);
	// SDL_GetVersion(&sdl_linked_version);
	// debug::log::info
	// (
		// "SDL compiled version: {}.{}.{}; linked version: {}.{}.{}",
		// sdl_compiled_version.major,
		// sdl_compiled_version.minor,
		// sdl_compiled_version.patch,
		// sdl_linked_version.major,
		// sdl_linked_version.minor,
		// sdl_linked_version.patch
	// );
	
	// Init SDL events and video subsystems
	debug::log::trace("Initializing SDL events and video subsystems...");
	if (SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_VIDEO) != 0)
	{
		debug::log::fatal("Failed to initialize SDL events and video subsystems: {}", SDL_GetError());
		throw std::runtime_error("Failed to initialize SDL events and video subsystems");
	}
	debug::log::trace("Initialized SDL events and video subsystems");
	
	// Query displays
	debug::log::trace("Querying displays...");
	
	const int sdl_display_count = SDL_GetNumVideoDisplays();
	if (sdl_display_count < 1)
	{
		debug::log::fatal("No displays detected: {}", SDL_GetError());
		throw std::runtime_error("No displays detected");
	}
	
	debug::log::info("Display count: {}", sdl_display_count);
	
	for (int i = 0; i < sdl_display_count; ++i)
	{
		// Query display mode
		SDL_DisplayMode sdl_display_mode;
		if (SDL_GetDesktopDisplayMode(i, &sdl_display_mode) != 0)
		{
			debug::log::error("Failed to get mode of display {}: {}", i, SDL_GetError());
			SDL_ClearError();
			continue;
		}
		
		// Query display name
		const char* sdl_display_name = SDL_GetDisplayName(i);
		if (!sdl_display_name)
		{
			debug::log::warning("Failed to get name of display {}: {}", i, SDL_GetError());
			SDL_ClearError();
			sdl_display_name = "";
		}
		
		// Query display DPI
		float sdl_display_dpi;
		if (SDL_GetDisplayDPI(i, &sdl_display_dpi, nullptr, nullptr) != 0)
		{
			const float default_dpi = 96.0f;
			debug::log::warning("Failed to get DPI of display {}: {}; Defaulting to {} DPI", i, SDL_GetError(), default_dpi);
			SDL_ClearError();
		}
		
		// Log display information
		debug::log::info("Display {} name: \"{}\"; resolution: {}x{}; refresh rate: {}Hz; DPI: {}", i, sdl_display_name, sdl_display_mode.w, sdl_display_mode.h, sdl_display_mode.refresh_rate, sdl_display_dpi);
	}
	
	debug::log::trace("Queried displays");
	
	// Detect display dimensions
	SDL_DisplayMode sdl_desktop_display_mode;
	if (SDL_GetDesktopDisplayMode(0, &sdl_desktop_display_mode) != 0)
	{
		debug::log::fatal("Failed to detect desktop display mode: {}", SDL_GetError());
		throw std::runtime_error("Failed to detect desktop display mode");
	}
	display_size = {sdl_desktop_display_mode.w, sdl_desktop_display_mode.h};
	
	// Detect display DPI
	if (SDL_GetDisplayDPI(0, &display_dpi, nullptr, nullptr) != 0)
	{
		debug::log::fatal("Failed to detect display DPI: {}", SDL_GetError());
		throw std::runtime_error("Failed to detect display DPI");
	}
	
	// Log display properties
	debug::log::info("Detected {}x{}@{}Hz display with {} DPI", sdl_desktop_display_mode.w, sdl_desktop_display_mode.h, sdl_desktop_display_mode.refresh_rate, display_dpi);
	
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
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, config::opengl_version_major);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, config::opengl_version_minor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, config::opengl_min_red_size);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, config::opengl_min_green_size);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, config::opengl_min_blue_size);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, config::opengl_min_alpha_size);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, config::opengl_min_depth_size);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, config::opengl_min_stencil_size);
	
	Uint32 sdl_window_flags = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
	if (fullscreen)
	{
		sdl_window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}
	if (maximized)
	{
		sdl_window_flags |= SDL_WINDOW_MAXIMIZED;
	}
	if (window_x == -1 && window_y == -1)
	{
		window_x = SDL_WINDOWPOS_CENTERED;
		window_y = SDL_WINDOWPOS_CENTERED;
	}
	if (window_w <= 0 || window_h <= 0)
	{
		window_w = sdl_desktop_display_mode.w / 2;
		window_h = sdl_desktop_display_mode.h / 2;
	}
	
	// Create a hidden fullscreen window
	debug::log::trace("Creating window...");
	sdl_window = SDL_CreateWindow
	(
		window_title.c_str(),
    	window_x,
    	window_y,
	    window_w,
	    window_h,
		sdl_window_flags
	);
	if (!sdl_window)
	{
		debug::log::fatal("Failed to create {}x{} window: {}", display_size[0], display_size[1], SDL_GetError());
		throw std::runtime_error("Failed to create SDL window");
	}
	debug::log::trace("Created window");
	
	
	if (window_x != SDL_WINDOWPOS_CENTERED && window_y != SDL_WINDOWPOS_CENTERED)
	{
		this->windowed_position = {window_x, window_y};
	}
	this->windowed_size = {window_w, window_h};
	this->maximized = maximized;
	this->fullscreen = fullscreen;
	
	// Set hard window minimum size
	SDL_SetWindowMinimumSize(sdl_window, 160, 120);
	
	// Create OpenGL context
	debug::log::trace("Creating OpenGL {}.{} context...", config::opengl_version_major, config::opengl_version_minor);
	sdl_gl_context = SDL_GL_CreateContext(sdl_window);
	if (!sdl_gl_context)
	{
		debug::log::fatal("Failed to create OpenGL context: {}", SDL_GetError());
		throw std::runtime_error("Failed to create OpenGL context");
	}
	
	// Log version of created OpenGL context
	int opengl_context_version_major = -1;
	int opengl_context_version_minor = -1;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &opengl_context_version_major);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &opengl_context_version_minor);
	debug::log::info("Created OpenGL {}.{} context", opengl_context_version_major, opengl_context_version_minor);
	
	// Compare OpenGL context version with requested version
	if (opengl_context_version_major != config::opengl_version_major ||
		opengl_context_version_minor != config::opengl_version_minor)
	{
		debug::log::warning("Requested OpenGL {}.{} context but created OpenGL {}.{} context", config::opengl_version_major, config::opengl_version_minor, opengl_context_version_major, opengl_context_version_minor);
	}
	
	// Log format of OpenGL context default framebuffer
	int opengl_context_red_size = -1;
	int opengl_context_green_size = -1;
	int opengl_context_blue_size = -1;
	int opengl_context_alpha_size = -1;
	int opengl_context_depth_size = -1;
	int opengl_context_stencil_size = -1;
	SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &opengl_context_red_size);
	SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &opengl_context_green_size);
	SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &opengl_context_blue_size);
	SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &opengl_context_alpha_size);
	SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &opengl_context_depth_size);
	SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &opengl_context_stencil_size);
	debug::log::info("OpenGL context format: R{}G{}B{}A{}D{}S{}", opengl_context_red_size, opengl_context_green_size, opengl_context_blue_size, opengl_context_alpha_size, opengl_context_depth_size, opengl_context_stencil_size);
	
	// Compare OpenGL context default framebuffer format with request format
	if (opengl_context_red_size < config::opengl_min_red_size ||
		opengl_context_green_size < config::opengl_min_green_size ||
		opengl_context_blue_size < config::opengl_min_blue_size ||
		opengl_context_alpha_size < config::opengl_min_alpha_size ||
		opengl_context_depth_size < config::opengl_min_depth_size ||
		opengl_context_stencil_size < config::opengl_min_stencil_size)
	{
		debug::log::warning
		(
			"OpenGL context format (R{}G{}B{}A{}D{}S{}) does not meet minimum requested format (R{}G{}B{}A{}D{}S{})",
			opengl_context_red_size, opengl_context_green_size, opengl_context_blue_size, opengl_context_alpha_size, opengl_context_depth_size, opengl_context_stencil_size,
			config::opengl_min_red_size, config::opengl_min_green_size, config::opengl_min_blue_size, config::opengl_min_alpha_size, config::opengl_min_depth_size, config::opengl_min_stencil_size
		);
	}
	
	// Load OpenGL functions via GLAD
	debug::log::trace("Loading OpenGL functions...");
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		debug::log::fatal("Failed to load OpenGL functions", SDL_GetError());
		throw std::runtime_error("Failed to load OpenGL functions");
	}
	debug::log::trace("Loaded OpenGL functions");
	
	// Log OpenGL context information
	debug::log::info
	(
		"OpenGL vendor: {}; renderer: {}; version: {}; shading language version: {}",
		reinterpret_cast<const char*>(glGetString(GL_VENDOR)),
		reinterpret_cast<const char*>(glGetString(GL_RENDERER)),
		reinterpret_cast<const char*>(glGetString(GL_VERSION)),
		reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION))
	);
	
	// Clear window color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	swap_buffers();
	
	// Set v-sync mode
	set_v_sync(v_sync);
	
	// Update viewport size
	SDL_GL_GetDrawableSize(sdl_window, &viewport_size[0], &viewport_size[1]);
	
	// Init SDL joystick and controller subsystems
	debug::log::trace("Initializing SDL joystick and controller subsystems...");
	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0)
	{
		debug::log::error("Failed to initialize SDL joystick and controller subsytems: {}", SDL_GetError());
	}
	else
	{
		debug::log::trace("Initialized SDL joystick and controller subsystems");
	}
	
	// Setup rasterizer
	rasterizer = new gl::rasterizer();
	rasterizer->context_resized(viewport_size[0], viewport_size[1]);
	
	// Register keyboard and mouse with input device manager
	device_manager.register_device(keyboard);
	device_manager.register_device(mouse);
	
	// Generate keyboard and mouse device connected events
	keyboard.connect();
	mouse.connect();
	
	// Connect gamepads
	process_events();
}

application::~application()
{
	// Destroy the OpenGL context
	SDL_GL_DeleteContext(sdl_gl_context);
	
	// Destroy the SDL window
	SDL_DestroyWindow(sdl_window);

	// Shutdown SDL
	SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
	SDL_Quit();
}

void application::close()
{
	closed = true;
}

void application::set_title(const std::string& title)
{
	SDL_SetWindowTitle(sdl_window, title.c_str());
}

void application::set_cursor_visible(bool visible)
{
	SDL_ShowCursor((visible) ? SDL_ENABLE : SDL_DISABLE);
	cursor_visible = visible;
}

void application::set_relative_mouse_mode(bool enabled)
{
	if (enabled)
	{
		SDL_GetMouseState(&mouse_position[0], &mouse_position[1]);
		SDL_ShowCursor(SDL_DISABLE);
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
	else
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
		SDL_WarpMouseInWindow(sdl_window, mouse_position[0], mouse_position[1]);
		if (cursor_visible)
		{
			SDL_ShowCursor(SDL_ENABLE);
		}
	}
}

void application::resize_window(int width, int height)
{
	int x = (display_size[0] >> 1) - (width >> 1);
	int y = (display_size[1] >> 1) - (height >> 1);
	
	// Resize and center window
	SDL_SetWindowPosition(sdl_window, x, y);
	SDL_SetWindowSize(sdl_window, width, height);
}

void application::set_fullscreen(bool fullscreen)
{
	if (this->fullscreen != fullscreen)
	{
		this->fullscreen = fullscreen;
		
		if (fullscreen)
		{
			SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		}
		else
		{
			SDL_SetWindowFullscreen(sdl_window, 0);
		}
	}
}

void application::set_v_sync(bool v_sync)
{
	if (v_sync)
	{
		debug::log::trace("Enabling adaptive v-sync...");
		if (SDL_GL_SetSwapInterval(-1) != 0)
		{
			debug::log::error("Failed to enable adaptive v-sync: {}", SDL_GetError());
			debug::log::trace("Enabling synchronized v-sync...");
			if (SDL_GL_SetSwapInterval(1) != 0)
			{
				debug::log::error("Failed to enable synchronized v-sync: {}", SDL_GetError());
			}
			else
			{
				this->v_sync = v_sync;
				debug::log::debug("Enabled synchronized v-sync");
			}
		}
		else
		{
			this->v_sync = v_sync;
			debug::log::debug("Enabled adaptive v-sync");
		}
	}
	else
	{
		debug::log::trace("Disabling v-sync...");
		if (SDL_GL_SetSwapInterval(0) != 0)
		{
			debug::log::error("Failed to disable v-sync: {}", SDL_GetError());
		}
		else
		{
			this->v_sync = v_sync;
			debug::log::debug("Disabled v-sync");
		}
	}
}

void application::set_window_opacity(float opacity)
{
	SDL_SetWindowOpacity(sdl_window, opacity);
}

void application::swap_buffers()
{
	SDL_GL_SwapWindow(sdl_window);
}

void application::show_window()
{
	SDL_ShowWindow(sdl_window);
	//SDL_GL_MakeCurrent(sdl_window, sdl_gl_context);
}

void application::hide_window()
{
	SDL_HideWindow(sdl_window);
}

void application::add_game_controller_mappings(const void* mappings, std::size_t size)
{
	debug::log::trace("Adding SDL game controller mappings...");
	int mapping_count = SDL_GameControllerAddMappingsFromRW(SDL_RWFromConstMem(mappings, static_cast<int>(size)), 0);
	if (mapping_count == -1)
	{
		debug::log::error("Failed to add SDL game controller mappings: {}", SDL_GetError());
	}
	else
	{
		debug::log::debug("Added {} SDL game controller mappings", mapping_count);
	}
}

void application::process_events()
{
	// Active modifier keys
	std::uint16_t sdl_key_mod = KMOD_NONE;
	std::uint16_t modifier_keys = input::modifier_key::none;
	
	// Mouse motion event accumulators
	// bool mouse_motion = false;
	// std::int32_t mouse_x;
	// std::int32_t mouse_y;
	// std::int32_t mouse_dx = 0;
	// std::int32_t mouse_dy = 0;
	
	SDL_Event sdl_event;
	while (SDL_PollEvent(&sdl_event))
	{
		switch (sdl_event.type)
		{
			[[likely]] case SDL_MOUSEMOTION:
			{
				// More than one mouse motion event is often generated per frame, and may be a source of lag.
				// Mouse events can be accumulated here to prevent excessive function calls and allocations
				// mouse_motion = true;
				// mouse_x = sdl_event.motion.x;
				// mouse_y = sdl_event.motion.y;
				// mouse_dx += sdl_event.motion.xrel;
				// mouse_dy += sdl_event.motion.yrel;
				
				mouse.move({sdl_event.motion.x, sdl_event.motion.y}, {sdl_event.motion.xrel, sdl_event.motion.yrel});
				break;
			}
			
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				// Get scancode of key
				const input::scancode scancode = static_cast<input::scancode>(sdl_event.key.keysym.scancode);
				
				// Rebuild modifier keys bit mask
				if (sdl_event.key.keysym.mod != sdl_key_mod)
				{
					sdl_key_mod = sdl_event.key.keysym.mod;
					
					modifier_keys = input::modifier_key::none;
					if (sdl_key_mod & KMOD_LSHIFT)
						modifier_keys |= input::modifier_key::left_shift;
					if (sdl_key_mod & KMOD_RSHIFT)
						modifier_keys |= input::modifier_key::right_shift;
					if (sdl_key_mod & KMOD_LCTRL)
						modifier_keys |= input::modifier_key::left_ctrl;
					if (sdl_key_mod & KMOD_RCTRL)
						modifier_keys |= input::modifier_key::right_ctrl;
					if (sdl_key_mod & KMOD_LGUI)
						modifier_keys |= input::modifier_key::left_gui;
					if (sdl_key_mod & KMOD_RGUI)
						modifier_keys |= input::modifier_key::right_gui;
					if (sdl_key_mod & KMOD_NUM)
						modifier_keys |= input::modifier_key::num_lock;
					if (sdl_key_mod & KMOD_CAPS)
						modifier_keys |= input::modifier_key::caps_lock;
					if (sdl_key_mod & KMOD_SCROLL)
						modifier_keys |= input::modifier_key::scroll_lock;
					if (sdl_key_mod & KMOD_MODE)
						modifier_keys |= input::modifier_key::alt_gr;
				}
				
				// Determine if event was generated from a key repeat
				const bool repeat = sdl_event.key.repeat > 0;
				
				if (sdl_event.type == SDL_KEYDOWN)
				{
					keyboard.press(scancode, repeat, modifier_keys);
				}
				else
				{
					keyboard.release(scancode, repeat, modifier_keys);
				}
				
				break;
			}
			
			case SDL_MOUSEWHEEL:
			{
				const float flip = (sdl_event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) ? -1.0f : 1.0f;
				mouse.scroll({sdl_event.wheel.preciseX * flip, sdl_event.wheel.preciseY * flip});
				break;
			}
			
			case SDL_MOUSEBUTTONDOWN:
			{
				mouse.press(static_cast<input::mouse_button>(sdl_event.button.button));
				break;
			}
			
			case SDL_MOUSEBUTTONUP:
			{
				mouse.release(static_cast<input::mouse_button>(sdl_event.button.button));
				break;
			}
			
			[[likely]] case SDL_CONTROLLERAXISMOTION:
			{
				if (sdl_event.caxis.axis != SDL_CONTROLLER_AXIS_INVALID)
				{
					if (auto it = gamepad_map.find(sdl_event.cdevice.which); it != gamepad_map.end())
					{
						// Map axis position onto `[-1, 1]`.
						const float position = math::map
						(
							static_cast<float>(sdl_event.caxis.value),
							static_cast<float>(std::numeric_limits<decltype(sdl_event.caxis.value)>::min()),
							static_cast<float>(std::numeric_limits<decltype(sdl_event.caxis.value)>::max()),
							-1.0f,
							1.0f
						);
						
						// Generate gamepad axis moved event
						it->second->move(static_cast<input::gamepad_axis>(sdl_event.caxis.axis), position);
					}
				}
				break;
			}
			
			case SDL_CONTROLLERBUTTONDOWN:
			{
				if (sdl_event.cbutton.button != SDL_CONTROLLER_BUTTON_INVALID)
				{
					if (auto it = gamepad_map.find(sdl_event.cdevice.which); it != gamepad_map.end())
					{
						it->second->press(static_cast<input::gamepad_button>(sdl_event.cbutton.button));
					}
				}
				break;
			}
			
			case SDL_CONTROLLERBUTTONUP:
			{
				if (sdl_event.cbutton.button != SDL_CONTROLLER_BUTTON_INVALID)
				{
					if (auto it = gamepad_map.find(sdl_event.cdevice.which); it != gamepad_map.end())
					{
						it->second->release(static_cast<input::gamepad_button>(sdl_event.cbutton.button));
					}
				}
				break;
			}
			
			case SDL_WINDOWEVENT:
			{
				switch (sdl_event.window.event)
				{
					case SDL_WINDOWEVENT_SIZE_CHANGED:
					{
						// Query SDL window parameters
						SDL_Window* sdl_window = SDL_GetWindowFromID(sdl_event.window.windowID);
						const auto sdl_window_flags = SDL_GetWindowFlags(sdl_window);
						int sdl_window_drawable_w = 0;
						int sdl_window_drawable_h = 0;
						SDL_GL_GetDrawableSize(sdl_window, &sdl_window_drawable_w, &sdl_window_drawable_h);
						
						// Build window resized event
						input::event::window_resized event;
						event.window = nullptr;
						event.size.x() = static_cast<std::int32_t>(sdl_event.window.data1);
						event.size.y() = static_cast<std::int32_t>(sdl_event.window.data2);
						event.maximized = sdl_window_flags & SDL_WINDOW_MAXIMIZED;
						event.fullscreen = sdl_window_flags & SDL_WINDOW_FULLSCREEN;
						event.viewport_size.x() = static_cast<std::int32_t>(sdl_window_drawable_w);
						event.viewport_size.y() = static_cast<std::int32_t>(sdl_window_drawable_h);
						
						// Update windowed size
						if (!event.maximized && !event.fullscreen)
						{
							windowed_size = event.size;
						}
						
						// Update GL context size
						rasterizer->context_resized(event.viewport_size.x(), event.viewport_size.y());
						
						// Publish window resized event
						window_resized_publisher.publish(event);
						break;
					}
					
					case SDL_WINDOWEVENT_MOVED:
					{
						// Query SDL window parameters
						SDL_Window* sdl_window = SDL_GetWindowFromID(sdl_event.window.windowID);
						const auto sdl_window_flags = SDL_GetWindowFlags(sdl_window);
						
						// Build window moved event
						input::event::window_moved event;
						event.window = nullptr;
						event.position.x() = static_cast<std::int32_t>(sdl_event.window.data1);
						event.position.y() = static_cast<std::int32_t>(sdl_event.window.data2);
						event.maximized = sdl_window_flags & SDL_WINDOW_MAXIMIZED;
						event.fullscreen = sdl_window_flags & SDL_WINDOW_FULLSCREEN;
						
						// Update windowed position
						if (!event.maximized && !event.fullscreen)
						{
							windowed_position = event.position;
						}
						
						// Publish window moved event
						window_moved_publisher.publish(event);
						break;
					}
					
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						// Build and publish window focused gained event
						window_focus_changed_publisher.publish({nullptr, true});
						break;
					
					case SDL_WINDOWEVENT_FOCUS_LOST:
						// Build and publish window focused lost event
						window_focus_changed_publisher.publish({nullptr, false});
						break;
					
					case SDL_WINDOWEVENT_MAXIMIZED:
						// Update window maximized 
						maximized = true;
						
						// Build and publish window maximized event
						window_maximized_publisher.publish({nullptr});
						break;
					
					case SDL_WINDOWEVENT_RESTORED:
						// Update window maximized 
						maximized = false;
						
						// Build and publish window restored event
						window_restored_publisher.publish({nullptr});
						break;
					
					case SDL_WINDOWEVENT_MINIMIZED:
						// Build and publish window minimized event
						window_minimized_publisher.publish({nullptr});
						break;
					
					[[unlikely]] case SDL_WINDOWEVENT_CLOSE:
						// Build and publish window closed event
						window_closed_publisher.publish({nullptr});
						break;
					
					default:
						break;
				}
				break;
			}
			
			[[unlikely]] case SDL_CONTROLLERDEVICEADDED:
			{
				if (SDL_IsGameController(sdl_event.cdevice.which))
				{
					SDL_GameController* sdl_controller = SDL_GameControllerOpen(sdl_event.cdevice.which);
					const char* controller_name = SDL_GameControllerNameForIndex(sdl_event.cdevice.which);
					
					if (sdl_controller)
					{
						if (auto it = gamepad_map.find(sdl_event.cdevice.which); it != gamepad_map.end())
						{
							// Gamepad reconnected
							debug::log::info("Reconnected gamepad \"{}\"", controller_name);
							it->second->connect();
						}
						else
						{
							// Get gamepad GUID
							SDL_Joystick* sdl_joystick = SDL_GameControllerGetJoystick(sdl_controller);
							SDL_JoystickGUID sdl_guid = SDL_JoystickGetGUID(sdl_joystick);
							
							// Copy into UUID struct
							::uuid gamepad_uuid;
							std::memcpy(gamepad_uuid.data.data(), sdl_guid.data, gamepad_uuid.data.size());
							
							debug::log::info("Connected gamepad \"{}\" with UUID {}", controller_name, gamepad_uuid.string());
							
							// Create new gamepad
							input::gamepad* gamepad = new input::gamepad();
							gamepad->set_uuid(gamepad_uuid);
							
							// Add gamepad to gamepad map
							gamepad_map[sdl_event.cdevice.which] = gamepad;
							
							// Register gamepad with device manager
							device_manager.register_device(*gamepad);
							
							// Generate gamepad connected event
							gamepad->connect();
						}
					}
					else
					{
						debug::log::error("Failed to connected gamepad \"{}\": {}", controller_name, SDL_GetError());
					}
				}

				break;
			}
			
			[[unlikely]] case SDL_CONTROLLERDEVICEREMOVED:
			{
				SDL_GameController* sdl_controller = SDL_GameControllerFromInstanceID(sdl_event.cdevice.which);
				
				if (sdl_controller)
				{
					const char* controller_name = SDL_GameControllerNameForIndex(sdl_event.cdevice.which);
					
					SDL_GameControllerClose(sdl_controller);
					if (auto it = gamepad_map.find(sdl_event.cdevice.which); it != gamepad_map.end())
					{
						it->second->disconnect();
					}
					
					debug::log::info("Disconnected gamepad \"{}\"", controller_name);
				}

				break;
			}
			
			[[unlikely]] case SDL_QUIT:
			{
				debug::log::info("Quit requested");
				close();
				break;
			}
			
			default:
				break;
		}
	}
	
	// Process accumulated mouse motion events
	// if (mouse_motion)
	// {
		// mouse.move(mouse_x, mouse_y, mouse_dx, mouse_dy);
	// }
}
