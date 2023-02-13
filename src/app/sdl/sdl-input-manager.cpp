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

#include "app/sdl/sdl-input-manager.hpp"
#include "input/application-events.hpp"
#include "debug/log.hpp"
#include "math/map.hpp"
#include <SDL2/SDL.h>
#include <stdexcept>

namespace app {

sdl_input_manager::sdl_input_manager()
{
	// Init SDL joystick and controller subsystems
	debug::log::trace("Initializing SDL joystick and controller subsystems...");
	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0)
	{
		debug::log::error("Failed to initialize SDL joystick and controller subsytems: {}", SDL_GetError());
		throw std::runtime_error("Failed to initialize SDL joystick and controller subsytems");
	}
	else
	{
		debug::log::trace("Initialized SDL joystick and controller subsystems");
	}
	
	// Register keyboard and mouse
	register_keyboard(keyboard);
	register_mouse(mouse);
	
	// Generate keyboard and mouse device connected events
	keyboard.connect();
	mouse.connect();
}

sdl_input_manager::~sdl_input_manager()
{
	// Quit SDL joystick and controller subsystems
	debug::log::trace("Quitting SDL joystick and controller subsystems...");
	SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
	debug::log::trace("Quit SDL joystick and controller subsystems...");
}

void sdl_input_manager::update()
{
	// Active modifier keys
	std::uint16_t sdl_key_mod = KMOD_NONE;
	std::uint16_t modifier_keys = input::modifier_key::none;
	
	// Gather SDL events from event queue
	SDL_PumpEvents();
	
	// Handle OS events
	for (;;)
	{
		// Get next display or window event
		SDL_Event event;
		int status = SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LOCALECHANGED);
		
		if (!status)
		{
			break;
		}
		else if (status < 0)
		{
			debug::log::error("Failed to peep SDL events: {}", SDL_GetError());
			throw std::runtime_error("Failed to peep SDL events");
		}
		
		switch (event.type)
		{
			case SDL_QUIT:
				debug::log::debug("Application quit requested");
				this->event_queue.enqueue<input::application_quit_event>({});
				break;
			
			default:
				break;
		}
	}
	
	// Handle keyboard, mouse, and gamepad events
	for (;;)
	{
		// Get next display or window event
		SDL_Event event;
		int status = SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_KEYDOWN, SDL_LASTEVENT);
		
		if (!status)
		{
			break;
		}
		else if (status < 0)
		{
			debug::log::error("Failed to peep SDL events: {}", SDL_GetError());
			throw std::runtime_error("Failed to peep SDL events");
		}
		
		switch (event.type)
		{
			[[likely]] case SDL_MOUSEMOTION:
			{
				mouse.move({event.motion.x, event.motion.y}, {event.motion.xrel, event.motion.yrel});
				break;
			}
			
			[[likely]] case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				// Get scancode of key
				const input::scancode scancode = static_cast<input::scancode>(event.key.keysym.scancode);
				
				// Rebuild modifier keys bit mask
				if (sdl_key_mod != event.key.keysym.mod)
				{
					sdl_key_mod = event.key.keysym.mod;
					
					modifier_keys = input::modifier_key::none;
					if (sdl_key_mod & KMOD_LSHIFT)
						modifier_keys |= input::modifier_key::left_shift;
					if (sdl_key_mod & KMOD_RSHIFT)
						modifier_keys |= input::modifier_key::right_shift;
					if (sdl_key_mod & KMOD_LCTRL)
						modifier_keys |= input::modifier_key::left_ctrl;
					if (sdl_key_mod & KMOD_RCTRL)
						modifier_keys |= input::modifier_key::right_ctrl;
					if (sdl_key_mod & KMOD_LALT)
						modifier_keys |= input::modifier_key::left_alt;
					if (sdl_key_mod & KMOD_RALT)
						modifier_keys |= input::modifier_key::right_alt;
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
				
				if (event.type == SDL_KEYDOWN)
				{
					keyboard.press(scancode, modifier_keys, (event.key.repeat > 0));
				}
				else
				{
					keyboard.release(scancode, modifier_keys);
				}
				
				break;
			}
			
			case SDL_MOUSEWHEEL:
			{
				const float flip = (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) ? -1.0f : 1.0f;
				mouse.scroll({event.wheel.preciseX * flip, event.wheel.preciseY * flip});
				break;
			}
			
			case SDL_MOUSEBUTTONDOWN:
			{
				mouse.press(static_cast<input::mouse_button>(event.button.button));
				break;
			}
			
			case SDL_MOUSEBUTTONUP:
			{
				mouse.release(static_cast<input::mouse_button>(event.button.button));
				break;
			}
			
			[[likely]] case SDL_CONTROLLERAXISMOTION:
			{
				if (event.caxis.axis != SDL_CONTROLLER_AXIS_INVALID)
				{
					if (auto it = gamepad_map.find(event.cdevice.which); it != gamepad_map.end())
					{
						// Map axis position onto `[-1, 1]`.
						const float position = math::map
						(
							static_cast<float>(event.caxis.value),
							static_cast<float>(std::numeric_limits<decltype(event.caxis.value)>::min()),
							static_cast<float>(std::numeric_limits<decltype(event.caxis.value)>::max()),
							-1.0f,
							1.0f
						);
						
						// Generate gamepad axis moved event
						it->second->move(static_cast<input::gamepad_axis>(event.caxis.axis), position);
					}
				}
				break;
			}
			
			case SDL_CONTROLLERBUTTONDOWN:
			{
				if (event.cbutton.button != SDL_CONTROLLER_BUTTON_INVALID)
				{
					if (auto it = gamepad_map.find(event.cdevice.which); it != gamepad_map.end())
					{
						it->second->press(static_cast<input::gamepad_button>(event.cbutton.button));
					}
				}
				break;
			}
			
			case SDL_CONTROLLERBUTTONUP:
			{
				if (event.cbutton.button != SDL_CONTROLLER_BUTTON_INVALID)
				{
					if (auto it = gamepad_map.find(event.cdevice.which); it != gamepad_map.end())
					{
						it->second->release(static_cast<input::gamepad_button>(event.cbutton.button));
					}
				}
				break;
			}
			
			[[unlikely]] case SDL_CONTROLLERDEVICEADDED:
			{
				if (SDL_IsGameController(event.cdevice.which))
				{
					SDL_GameController* sdl_controller = SDL_GameControllerOpen(event.cdevice.which);
					if (sdl_controller)
					{
						// Get gamepad name
						const char* controller_name = SDL_GameControllerNameForIndex(event.cdevice.which);
						if (!controller_name)
						{
							controller_name = "";
						}
						
						if (auto it = gamepad_map.find(event.cdevice.which); it != gamepad_map.end())
						{
							// Gamepad reconnected
							debug::log::info("Reconnected gamepad {}", event.cdevice.which);
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
							
							debug::log::info("Connected gamepad {}; name: \"{}\"; UUID: {}", event.cdevice.which, controller_name, gamepad_uuid.string());
							
							// Create new gamepad
							input::gamepad* gamepad = new input::gamepad();
							gamepad->set_uuid(gamepad_uuid);
							
							// Add gamepad to gamepad map
							gamepad_map[event.cdevice.which] = gamepad;
							
							// Register gamepad
							register_device(*gamepad);
							
							// Generate gamepad connected event
							gamepad->connect();
						}
					}
					else
					{
						debug::log::error("Failed to connect gamepad {}: {}", event.cdevice.which, SDL_GetError());
						SDL_ClearError();
					}
				}

				break;
			}
			
			[[unlikely]] case SDL_CONTROLLERDEVICEREMOVED:
			{
				SDL_GameController* sdl_controller = SDL_GameControllerFromInstanceID(event.cdevice.which);
				
				if (sdl_controller)
				{
					SDL_GameControllerClose(sdl_controller);
					if (auto it = gamepad_map.find(event.cdevice.which); it != gamepad_map.end())
					{
						it->second->disconnect();
					}
					
					debug::log::info("Disconnected gamepad {}", event.cdevice.which);
				}
				
				break;
			}
			
			default:
				break;
		}
	}
	
	// Flush event queue
	this->event_queue.flush();
}

void sdl_input_manager::show_cursor()
{
	if (SDL_ShowCursor(SDL_ENABLE) < 0)
	{
		debug::log::error("Failed to show cursor: \"{}\"", SDL_GetError());
		SDL_ClearError();
	}
}

void sdl_input_manager::hide_cursor()
{
	if (SDL_ShowCursor(SDL_DISABLE) < 0)
	{
		debug::log::error("Failed to hide cursor: \"{}\"", SDL_GetError());
		SDL_ClearError();
	}
}

} // namespace app
