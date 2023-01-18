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
#include "debug/logger.hpp"
#include "event/event-dispatcher.hpp"
#include "event/window-events.hpp"
#include "input/scancode.hpp"
#include "input/sdl-game-controller-tables.hpp"
#include "input/sdl-scancode-table.hpp"
#include "resources/image.hpp"
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <stdexcept>
#include <utility>
#include <iostream>
#include <iomanip>

application::application(debug::logger& log):
	closed(false),
	fullscreen(true),
	v_sync(false),
	cursor_visible(true),
	display_dimensions({0, 0}),
	display_dpi(0.0f),
	window_dimensions({0, 0}),
	viewport_dimensions({0, 0}),
	mouse_position({0, 0}),
	logger(&log),
	sdl_window(nullptr),
	sdl_gl_context(nullptr)
{
	// Get SDL compiled version
	SDL_version sdl_compiled_version;
	SDL_VERSION(&sdl_compiled_version);
	std::string sdl_compiled_version_string = std::to_string(sdl_compiled_version.major) + "." + std::to_string(sdl_compiled_version.minor) + "." + std::to_string(sdl_compiled_version.patch);

	// Get SDL linked version
	SDL_version sdl_linked_version;
	SDL_GetVersion(&sdl_linked_version);
	std::string sdl_linked_version_string = std::to_string(sdl_linked_version.major) + "." + std::to_string(sdl_linked_version.minor) + "." + std::to_string(sdl_linked_version.patch);

	// Init SDL
	logger->push_task("Initializing SDL");
	logger->log("Compiled against SDL " + sdl_compiled_version_string);
	logger->log("Linking against SDL " + sdl_linked_version_string);
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		logger->pop_task(EXIT_FAILURE);
		throw std::runtime_error("Failed to initialize SDL");
	}
	else
	{
		logger->pop_task(EXIT_SUCCESS);
	}
	
	// Load default OpenGL library
	logger->push_task("Loading OpenGL library");
	if (SDL_GL_LoadLibrary(nullptr) != 0)
	{
		logger->pop_task(EXIT_FAILURE);
	}
	else
	{
		logger->pop_task(EXIT_SUCCESS);
	}

	// Set window creation hints
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	
	// Get display dimensions
	SDL_DisplayMode sdl_desktop_display_mode;
	if (SDL_GetDesktopDisplayMode(0, &sdl_desktop_display_mode) != 0)
	{
		logger->error("Failed to detect desktop display mode: \"" + std::string(SDL_GetError()) + "\"");
		throw std::runtime_error("Failed to detect desktop display mode");
	}
	else
	{
		display_dimensions = {sdl_desktop_display_mode.w, sdl_desktop_display_mode.h};
	}
	
	// Get display DPI
	if (SDL_GetDisplayDPI(0, &display_dpi, nullptr, nullptr) != 0)
	{
		logger->error("Failed to detect display DPI: \"" + std::string(SDL_GetError()) + "\"");
		throw std::runtime_error("Failed to detect display DPI");
	}
	else
	{
		logger->log("Detected " + std::to_string(sdl_desktop_display_mode.w) + "x" + std::to_string(sdl_desktop_display_mode.h) + " display with " + std::to_string(display_dpi) + " DPI");
	}
	
	// Create a hidden fullscreen window
	logger->push_task("Creating " + std::to_string(display_dimensions[0]) + "x" + std::to_string(display_dimensions[1]) + " window");
	sdl_window = SDL_CreateWindow
	(
		"Antkeeper",
    	SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
	    display_dimensions[0], display_dimensions[1],
		SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN
	);
	
	if (!sdl_window)
	{
		logger->pop_task(EXIT_FAILURE);
		throw std::runtime_error("Failed to create SDL window");
	}
	else
	{
		logger->pop_task(EXIT_SUCCESS);
	}

	// Create OpenGL context
	logger->push_task("Creating OpenGL 3.3 context");
	sdl_gl_context = SDL_GL_CreateContext(sdl_window);
	if (!sdl_gl_context)
	{
		logger->pop_task(EXIT_FAILURE);
		throw std::runtime_error("Failed to create OpenGL context");
	}
	else
	{
		logger->pop_task(EXIT_SUCCESS);
	}
	
	// Make OpenGL context current
	logger->push_task("Making OpenGL context current");
	if (SDL_GL_MakeCurrent(sdl_window, sdl_gl_context) != 0)
	{
		logger->pop_task(EXIT_FAILURE);
		throw std::runtime_error("Failed to make OpenGL context current");
	}
	else
	{
		logger->pop_task(EXIT_SUCCESS);
	}

	// Load OpenGL functions via GLAD
	logger->push_task("Loading OpenGL functions");
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		logger->pop_task(EXIT_FAILURE);
		throw std::runtime_error("Failed to load OpenGL functions");
	}
	else
	{
		logger->pop_task(EXIT_SUCCESS);
	}
	
	// Update window size and viewport size
	SDL_GetWindowSize(sdl_window, &window_dimensions[0], &window_dimensions[1]);
	SDL_GL_GetDrawableSize(sdl_window, &viewport_dimensions[0], &viewport_dimensions[1]);
	
	// Set v-sync mode
	set_v_sync(true);

	// Init SDL joystick and gamepad subsystems
	logger->push_task("Initializing SDL Joystick and Game Controller subsystems");
	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0)
	{
		logger->pop_task(EXIT_FAILURE);
	}
	else
	{
		logger->pop_task(EXIT_SUCCESS);
	}
	
	// Setup rasterizer
	rasterizer = new gl::rasterizer();
	
	// Setup events
	event_dispatcher = new ::event_dispatcher();
	
	// Setup input
	keyboard = new input::keyboard();
	keyboard->set_event_dispatcher(event_dispatcher);
	mouse = new input::mouse();
	mouse->set_event_dispatcher(event_dispatcher);
	
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
			SDL_ShowCursor(SDL_ENABLE);
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
			logger->push_task("Enabling adaptive v-sync");
			if (SDL_GL_SetSwapInterval(-1) != 0)
			{
				logger->pop_task(EXIT_FAILURE);
				
				logger->push_task("Enabling synchronized v-sync");
				if (SDL_GL_SetSwapInterval(1) != 0)
				{
					logger->pop_task(EXIT_FAILURE);
				}
				else
				{
					this->v_sync = v_sync;
					logger->pop_task(EXIT_SUCCESS);
				}
			}
			else
			{
				this->v_sync = v_sync;
				logger->pop_task(EXIT_SUCCESS);
			}
		}
		else
		{
			logger->push_task("Disabling v-sync");
			if (SDL_GL_SetSwapInterval(0) != 0)
			{
				logger->pop_task(EXIT_FAILURE);
			}
			else
			{
				this->v_sync = v_sync;
				logger->pop_task(EXIT_SUCCESS);
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
	logger->push_task("Adding SDL game controller mappings");
	int mapping_count = SDL_GameControllerAddMappingsFromRW(SDL_RWFromConstMem(mappings, static_cast<int>(size)), 0);
	if (mapping_count == -1)
	{
		logger->pop_task(EXIT_FAILURE);
	}
	else
	{
		logger->log("Added " + std::to_string(mapping_count) + " SDL game controller mappings");
		logger->pop_task(EXIT_SUCCESS);
	}
}

void application::process_events()
{
	// Mouse motion event accumulators
	bool mouse_motion = false;
	int mouse_x;
	int mouse_y;
	int mouse_dx = 0;
	int mouse_dy = 0;
	
	SDL_Event sdl_event;
	while (SDL_PollEvent(&sdl_event))
	{
		switch (sdl_event.type)
		{
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				if (sdl_event.key.repeat == 0)
				{
					input::scancode scancode = input::scancode::unknown;
					if (sdl_event.key.keysym.scancode <= SDL_SCANCODE_APP2)
					{
						scancode = input::sdl_scancode_table[sdl_event.key.keysym.scancode];
					}

					if (sdl_event.type == SDL_KEYDOWN)
						keyboard->press(scancode);
					else
						keyboard->release(scancode);
				}
				break;
			}

			case SDL_MOUSEMOTION:
			{
				// More than one mouse motion event is often generated per frame, and may be a source of lag.
				// Mouse events are accumulated here to prevent excess function calls and allocations
				mouse_motion = true;
				mouse_x = sdl_event.motion.x;
				mouse_y = sdl_event.motion.y;
				mouse_dx += sdl_event.motion.xrel;
				mouse_dy += sdl_event.motion.yrel;
				break;
			}

			case SDL_MOUSEBUTTONDOWN:
			{
				mouse->press(sdl_event.button.button, sdl_event.button.x, sdl_event.button.y);
				break;
			}
			
			case SDL_MOUSEBUTTONUP:
			{
				mouse->release(sdl_event.button.button, sdl_event.button.x, sdl_event.button.y);
				break;
			}
			
			case SDL_MOUSEWHEEL:
			{
				int direction = (sdl_event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) ? -1 : 1;
				mouse->scroll(sdl_event.wheel.x * direction, sdl_event.wheel.y * direction);
				break;
			}

			case SDL_CONTROLLERBUTTONDOWN:
			{
				if (sdl_event.cbutton.button != SDL_CONTROLLER_BUTTON_INVALID)
				{
					if (auto it = gamepad_map.find(sdl_event.cdevice.which); it != gamepad_map.end())
					{
						input::gamepad_button button = input::sdl_button_table[sdl_event.cbutton.button];
						it->second->press(button);
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
						input::gamepad_button button = input::sdl_button_table[sdl_event.cbutton.button];
						it->second->release(button);
					}
				}
				break;
			}

			case SDL_CONTROLLERAXISMOTION:
			{
				if (sdl_event.caxis.axis != SDL_CONTROLLER_AXIS_INVALID)
				{
					if (auto it = gamepad_map.find(sdl_event.cdevice.which); it != gamepad_map.end())
					{
						input::gamepad_axis axis = input::sdl_axis_table[sdl_event.caxis.axis];
						float value = sdl_event.caxis.value;
						static const float min = static_cast<float>(std::numeric_limits<std::int16_t>::min());
						static const float max = static_cast<float>(std::numeric_limits<std::int16_t>::max());
						value /= (value < 0.0f) ? -min : max;
						it->second->move(axis, value);
					}
				}
				break;
			}

			case SDL_CONTROLLERDEVICEADDED:
			{
				if (SDL_IsGameController(sdl_event.cdevice.which))
				{
					SDL_GameController* sdl_controller = SDL_GameControllerOpen(sdl_event.cdevice.which);

					std::string controller_name = SDL_GameControllerNameForIndex(sdl_event.cdevice.which);
					if (sdl_controller)
					{
						if (auto it = gamepad_map.find(sdl_event.cdevice.which); it != gamepad_map.end())
						{
							logger->log("Reconnected gamepad  \"" + controller_name + "\"");
							
							it->second->connect(true);
							gamepad_connection_signal.emit(*it->second, true);
						}
						else
						{
							// Get gamepad GUID
							SDL_Joystick* sdl_joystick = SDL_GameControllerGetJoystick(sdl_controller);
							SDL_JoystickGUID sdl_guid = SDL_JoystickGetGUID(sdl_joystick);
							char guid_buffer[33];
							std::memset(guid_buffer, '\0', 33);
							SDL_JoystickGetGUIDString(sdl_guid, &guid_buffer[0], 33);
							std::string guid(guid_buffer);
							
							logger->log("Connected gamepad \"" + controller_name + "\" with GUID: " + guid + "");
							
							// Create new gamepad
							input::gamepad* gamepad = new input::gamepad();
							gamepad->set_event_dispatcher(event_dispatcher);
							gamepad->set_guid(guid);
							
							// Add gamepad to list and map
							gamepads.push_back(gamepad);
							gamepad_map[sdl_event.cdevice.which] = gamepad;
							
							// Send controller connected event
							gamepad->connect(false);
							gamepad_connection_signal.emit(*it->second, false);
						}
					}
					else
					{
						logger->error("Failed to connected gamepad \"" + controller_name + "\"");
					}
				}

				break;
			}

			case SDL_CONTROLLERDEVICEREMOVED:
			{
				SDL_GameController* sdl_controller = SDL_GameControllerFromInstanceID(sdl_event.cdevice.which);

				if (sdl_controller)
				{
					SDL_GameControllerClose(sdl_controller);
					logger->log("Disconnected gamepad");
					
					if (auto it = gamepad_map.find(sdl_event.cdevice.which); it != gamepad_map.end())
					{
						it->second->disconnect();
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
						window_focus_signal.emit(true);
						break;
					
					case SDL_WINDOWEVENT_FOCUS_LOST:
						window_focus_signal.emit(false);
						break;
					
					case SDL_WINDOWEVENT_MOVED:
						window_motion_signal.emit(sdl_event.window.data1, sdl_event.window.data2);
						break;
					
					case SDL_WINDOWEVENT_CLOSE:
						window_close_signal.emit();
						break;
					
					default:
						break;
				}
				break;
			}

			case SDL_QUIT:
			{
				close();
				break;
			}
		}
	}
	
	// Process accumulated mouse motion events
	if (mouse_motion)
	{
		mouse->move(mouse_x, mouse_y, mouse_dx, mouse_dy);
	}
}

void application::window_resized()
{
	// Update window size and viewport size
	SDL_GetWindowSize(sdl_window, &window_dimensions[0], &window_dimensions[1]);
	SDL_GL_GetDrawableSize(sdl_window, &viewport_dimensions[0], &viewport_dimensions[1]);
	
	rasterizer->context_resized(viewport_dimensions[0], viewport_dimensions[1]);
	
	window_resized_event event;
	event.w = window_dimensions[0];
	event.h = window_dimensions[1];
	
	event_dispatcher->queue(event);
	
	window_size_signal.emit(window_dimensions[0], window_dimensions[1]);
	viewport_size_signal.emit(viewport_dimensions[0], viewport_dimensions[1]);
}
