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

#include <engine/app/sdl/sdl-input-manager.hpp>
#include <engine/input/application-events.hpp>
#include <engine/debug/log.hpp>
#include <engine/math/map.hpp>
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
	register_keyboard(m_keyboard);
	register_mouse(m_mouse);
	
	// Generate keyboard and mouse device connected events
	m_keyboard.connect();
	m_mouse.connect();
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
		const int status = SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LOCALECHANGED);
		
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
				this->m_event_dispatcher.dispatch<input::application_quit_event>({});
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
		const int status = SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_KEYDOWN, SDL_LASTEVENT);
		
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
				m_mouse.move({event.motion.x, event.motion.y}, {event.motion.xrel, event.motion.yrel});
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
					m_keyboard.press(scancode, modifier_keys, (event.key.repeat > 0));
				}
				else
				{
					m_keyboard.release(scancode, modifier_keys);
				}
				
				break;
			}
			
			case SDL_MOUSEWHEEL:
			{
				const float flip = (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) ? -1.0f : 1.0f;
				m_mouse.scroll({event.wheel.preciseX * flip, event.wheel.preciseY * flip});
				break;
			}
			
			case SDL_MOUSEBUTTONDOWN:
			{
				m_mouse.press(static_cast<input::mouse_button>(event.button.button));
				break;
			}
			
			case SDL_MOUSEBUTTONUP:
			{
				m_mouse.release(static_cast<input::mouse_button>(event.button.button));
				break;
			}
			
			[[likely]] case SDL_CONTROLLERAXISMOTION:
			{
				if (event.caxis.axis != SDL_CONTROLLER_AXIS_INVALID)
				{
					if (auto it = m_gamepad_map.find(event.cdevice.which); it != m_gamepad_map.end())
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
					if (auto it = m_gamepad_map.find(event.cdevice.which); it != m_gamepad_map.end())
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
					if (auto it = m_gamepad_map.find(event.cdevice.which); it != m_gamepad_map.end())
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
						
						if (auto it = m_gamepad_map.find(event.cdevice.which); it != m_gamepad_map.end())
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
							
							// Allocate gamepad
							auto& gamepad = m_gamepad_map[event.cdevice.which];
							gamepad = std::make_unique<input::gamepad>();						
							gamepad->set_uuid(gamepad_uuid);
							
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
					if (auto it = m_gamepad_map.find(event.cdevice.which); it != m_gamepad_map.end())
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
}

void sdl_input_manager::set_cursor_visible(bool visible)
{
	if (SDL_ShowCursor((visible) ? SDL_ENABLE : SDL_DISABLE) < 0)
	{
		debug::log::error("Failed to set cursor visibility: \"{}\"", SDL_GetError());
		SDL_ClearError();
	}
}

void sdl_input_manager::set_relative_mouse_mode(bool enabled)
{
	if (SDL_SetRelativeMouseMode((enabled) ? SDL_TRUE : SDL_FALSE) < 0)
	{
		debug::log::error("Failed to set relative mouse mode: \"{}\"", SDL_GetError());
		SDL_ClearError();
	}
}

} // namespace app
