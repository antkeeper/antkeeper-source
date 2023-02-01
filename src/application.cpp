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
#include "resources/image.hpp"
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <cstring>
#include <iomanip>
#include <stdexcept>
#include <utility>

application::application():
	closed(false),
	fullscreen(true),
	v_sync(false),
	cursor_visible(true),
	display_dimensions({0, 0}),
	display_dpi(0.0f),
	window_dimensions({0, 0}),
	viewport_dimensions({0, 0}),
	mouse_position({0, 0}),
	sdl_window(nullptr),
	sdl_gl_context(nullptr)
{
	// Log SDL compiled version
	SDL_version sdl_compiled_version;
	SDL_VERSION(&sdl_compiled_version);
	debug::log::debug("Compiled against SDL {}.{}.{}", sdl_compiled_version.major, sdl_compiled_version.minor, sdl_compiled_version.patch);
	
	// Log SDL linked version
	SDL_version sdl_linked_version;
	SDL_GetVersion(&sdl_linked_version);
	debug::log::debug("Linking against SDL {}.{}.{}", sdl_linked_version.major, sdl_linked_version.minor, sdl_linked_version.patch);
	
	// Init SDL events and video subsystems
	debug::log::trace("Initializing SDL events and video subsystems...");
	if (SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_VIDEO) != 0)
	{
		debug::log::fatal("Failed to initialize SDL events and video subsystems: {}", SDL_GetError());
		throw std::runtime_error("Failed to initialize SDL events and video subsystems");
	}
	debug::log::trace("Initialized SDL events and video subsystems");
	
	// Detect display dimensions
	SDL_DisplayMode sdl_desktop_display_mode;
	if (SDL_GetDesktopDisplayMode(0, &sdl_desktop_display_mode) != 0)
	{
		debug::log::fatal("Failed to detect desktop display mode: {}", SDL_GetError());
		throw std::runtime_error("Failed to detect desktop display mode");
	}
	display_dimensions = {sdl_desktop_display_mode.w, sdl_desktop_display_mode.h};
	
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
	
	// Create a hidden fullscreen window
	debug::log::trace("Creating {}x{} window...", display_dimensions[0], display_dimensions[1]);
	sdl_window = SDL_CreateWindow
	(
		config::application_name,
    	SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
	    display_dimensions[0], display_dimensions[1],
		SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN
	);
	if (!sdl_window)
	{
		debug::log::fatal("Failed to create {}x{} window: {}", display_dimensions[0], display_dimensions[1], SDL_GetError());
		throw std::runtime_error("Failed to create SDL window");
	}
	debug::log::trace("Created {}x{} window", display_dimensions[0], display_dimensions[1]);
	
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
	debug::log::info("OpenGL context default framebuffer format: R{}G{}B{}A{}D{}S{}", opengl_context_red_size, opengl_context_green_size, opengl_context_blue_size, opengl_context_alpha_size, opengl_context_depth_size, opengl_context_stencil_size);
	
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
			"OpenGL default framebuffer format (R{}G{}B{}A{}D{}S{}) does not meet minimum requested format (R{}G{}B{}A{}D{}S{})",
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
	
	// Update window size and viewport size
	SDL_GetWindowSize(sdl_window, &window_dimensions[0], &window_dimensions[1]);
	SDL_GL_GetDrawableSize(sdl_window, &viewport_dimensions[0], &viewport_dimensions[1]);
	
	// Set v-sync mode
	set_v_sync(true);

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
	int x = (display_dimensions[0] >> 1) - (width >> 1);
	int y = (display_dimensions[1] >> 1) - (height >> 1);
	
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
			SDL_HideWindow(sdl_window);
			SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			SDL_ShowWindow(sdl_window);
		}
		else
		{
			SDL_SetWindowFullscreen(sdl_window, 0);
			SDL_SetWindowBordered(sdl_window, SDL_TRUE);
			SDL_SetWindowResizable(sdl_window, SDL_TRUE);
		}
	}
}

void application::set_v_sync(bool v_sync)
{
	if (this->v_sync != v_sync)
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
						window_resized();
						break;
					
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						debug::log::debug("Window focus gained");
						window_focus_changed_publisher.publish({true});
						break;
					
					case SDL_WINDOWEVENT_FOCUS_LOST:
						debug::log::debug("Window focus lost");
						window_focus_changed_publisher.publish({false});
						break;
					
					case SDL_WINDOWEVENT_MOVED:
						debug::log::trace("Window moved to ({}, {})", sdl_event.window.data1, sdl_event.window.data2);
						window_moved_publisher.publish({static_cast<int>(sdl_event.window.data1), static_cast<int>(sdl_event.window.data2)});
						break;
					
					[[unlikely]] case SDL_WINDOWEVENT_CLOSE:
						debug::log::info("Window closed");
						window_closed_publisher.publish({});
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
		}
	}
	
	// Process accumulated mouse motion events
	// if (mouse_motion)
	// {
		// mouse.move(mouse_x, mouse_y, mouse_dx, mouse_dy);
	// }
}

void application::window_resized()
{
	// Update window size and viewport size
	SDL_GetWindowSize(sdl_window, &window_dimensions[0], &window_dimensions[1]);
	SDL_GL_GetDrawableSize(sdl_window, &viewport_dimensions[0], &viewport_dimensions[1]);
	
	debug::log::debug("Window resized to {}x{}", window_dimensions[0], window_dimensions[1]);
	
	rasterizer->context_resized(viewport_dimensions[0], viewport_dimensions[1]);
	
	window_resized_publisher.publish
	(
		{
			window_dimensions[0],
			window_dimensions[1],
			viewport_dimensions[0],
			viewport_dimensions[1]
		}
	);
}
