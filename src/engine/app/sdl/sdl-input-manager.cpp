// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/app/sdl/sdl-input-manager.hpp>
#include <engine/input/application-events.hpp>
#include <engine/input/clipboard-events.hpp>
#include <engine/input/input-update-event.hpp>
#include <engine/debug/log.hpp>
#include <engine/math/functions.hpp>
#include <SDL2/SDL.h>
#include <stdexcept>

namespace app {

sdl_input_manager::sdl_input_manager()
{
	// Init SDL joystick and controller subsystems
	debug::log_debug("Initializing SDL joystick and controller subsystems...");
	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0)
	{
		auto error_message = std::format("Failed to initialize SDL joystick and controller subsytems: {}", SDL_GetError());
		debug::log_error("{}", error_message);
		debug::log_debug("Initializing SDL joystick and controller subsystems... FAILED");
		throw std::runtime_error(std::move(error_message));
	}
	else
	{
		debug::log_debug("Initializing SDL joystick and controller subsystems... OK");
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
	debug::log_debug("Quitting SDL joystick and controller subsystems...");
	SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
	debug::log_debug("Quitting SDL joystick and controller subsystems... OK");
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
			auto error_message = std::format("Failed to peep SDL events: {}", SDL_GetError());
			debug::log_error("{}", error_message);
			throw std::runtime_error(std::move(error_message));
		}
		
		switch (event.type)
		{
			case SDL_QUIT:
				debug::log_debug("Received application quit request");
				m_event_dispatcher.dispatch<input::application_quit_event>({});
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
		const int status = SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_KEYDOWN, SDL_CLIPBOARDUPDATE);
		
		if (!status)
		{
			break;
		}
		else if (status < 0)
		{
			auto error_message = std::format("Failed to peep SDL events: {}", SDL_GetError());
			debug::log_error("{}", error_message);
			throw std::runtime_error(std::move(error_message));
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
			
			case SDL_TEXTINPUT:
			{
				m_keyboard.input_text(event.text.text);
				break;
			}
			
			case SDL_TEXTEDITING:
			{
				m_keyboard.edit_text(event.edit.text, static_cast<std::size_t>(event.edit.start), static_cast<std::size_t>(event.edit.length));
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
						const float position = math::map_range
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
							debug::log_info("Reconnected gamepad {}", event.cdevice.which);
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
							
							debug::log_info("Connected gamepad {}; name: \"{}\"; UUID: {}", event.cdevice.which, controller_name, gamepad_uuid.string());
							
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
						debug::log_error("Failed to connect gamepad {}: {}", event.cdevice.which, SDL_GetError());
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
					
					debug::log_info("Disconnected gamepad {}", event.cdevice.which);
				}
				
				break;
			}
			
			[[unlikely]] case SDL_CLIPBOARDUPDATE:
			{
				m_event_dispatcher.dispatch<input::clipboard_updated_event>({});
				break;
			}
			
			default:
				break;
		}
	}
	
	// Dispatch input update event
	m_event_dispatcher.dispatch<input::update_event>({});
}

void sdl_input_manager::set_cursor_visible(bool visible)
{
	if (SDL_ShowCursor((visible) ? SDL_ENABLE : SDL_DISABLE) < 0)
	{
		debug::log_error("Failed to set cursor visibility: \"{}\"", SDL_GetError());
		SDL_ClearError();
	}
}

void sdl_input_manager::set_relative_mouse_mode(bool enabled)
{
	if (SDL_SetRelativeMouseMode((enabled) ? SDL_TRUE : SDL_FALSE) < 0)
	{
		debug::log_error("Failed to set relative mouse mode: \"{}\"", SDL_GetError());
		SDL_ClearError();
	}
}

void sdl_input_manager::set_clipboard_text(const std::string& text)
{
	if (SDL_SetClipboardText(text.c_str()) != 0)
	{
		debug::log_error("Failed to set clipboard text: \"{}\"", SDL_GetError());
		SDL_ClearError();
	}
}

std::string sdl_input_manager::get_clipboard_text() const
{
	// Get SDL-allocated text from clipboard
	auto sdl_clipboard_text = SDL_GetClipboardText();
	
	// Copy text into string
	std::string clipboard_text = sdl_clipboard_text;
	
	// Free SDL-allocated text
	SDL_free(sdl_clipboard_text);
	
	return clipboard_text;
}

void sdl_input_manager::start_text_input(const geom::rectangle<int>& rect)
{
	const SDL_Rect sdl_rect
	{
		rect.min.x(),
		rect.min.y(),
		rect.max.x() - rect.min.x(),
		rect.max.y() - rect.min.y()
	};
	SDL_SetTextInputRect(&sdl_rect);
	SDL_StartTextInput();
	debug::log_debug("Started text input");
}

void sdl_input_manager::stop_text_input()
{
	SDL_StopTextInput();
	debug::log_debug("Stopped text input");
}

} // namespace app
