/*
 * Copyright (C) 2017  Christopher J. Howard
 *
 * This file is part of Antkeeper Source Code.
 *
 * Antkeeper Source Code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper Source Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper Source Code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "input.hpp"
#include <iostream>

InputDevice::InputDevice(const std::string& name):
	name(name),
	disconnected(true)
{}

void InputDevice::setDisconnected(bool disconnected)
{
	this->disconnected = disconnected;
}

Keyboard::Keyboard(const std::string& name):
	InputDevice(name)
{}

Keyboard::~Keyboard()
{}

void Keyboard::addKeyObserver(KeyObserver* observer)
{
	keyObservers.push_back(observer);
}

void Keyboard::removeKeyObserver(KeyObserver* observer)
{
	keyObservers.remove(observer);
}

void Keyboard::removeKeyObservers()
{
	keyObservers.clear();
}

void Keyboard::press(int scancode)
{
	for (auto observer: keyObservers)
	{
		observer->keyPressed(scancode);
	}
}

void Keyboard::release(int scancode)
{
	for (auto observer: keyObservers)
	{
		observer->keyReleased(scancode);
	}
}

Mouse::Mouse(const std::string& name):
	InputDevice(name),
	notifyingMotionObservers(false),
	notifyingButtonObservers(false),
	notifyingWheelObservers(false)
{}

Mouse::~Mouse()
{}

void Mouse::addMouseMotionObserver(MouseMotionObserver* observer)
{
	if (notifyingMotionObservers)
	{
		additionFlaggedMotionObservers.push_back(observer);
	}
	else
	{
		motionObservers.push_back(observer);
	}
} 

void Mouse::addMouseButtonObserver(MouseButtonObserver* observer)
{
	if (notifyingButtonObservers)
	{
		additionFlaggedButtonObservers.push_back(observer);
	}
	else
	{
		buttonObservers.push_back(observer);
	}
}

void Mouse::addMouseWheelObserver(MouseWheelObserver* observer)
{
	if (notifyingWheelObservers)
	{
		additionFlaggedWheelObservers.push_back(observer);
	}
	else
	{
		wheelObservers.push_back(observer);
	}
}

void Mouse::removeMouseMotionObserver(MouseMotionObserver* observer)
{
	if (notifyingMotionObservers)
	{
		removalFlaggedMotionObservers.push_back(observer);
	}
	else
	{
		motionObservers.remove(observer);
	}
}

void Mouse::removeMouseButtonObserver(MouseButtonObserver* observer)
{
	if (notifyingButtonObservers)
	{
		removalFlaggedButtonObservers.push_back(observer);
	}
	else
	{
		buttonObservers.remove(observer);
	}
}

void Mouse::removeMouseWheelObserver(MouseWheelObserver* observer)
{
	if (notifyingWheelObservers)
	{
		removalFlaggedWheelObservers.push_back(observer);
	}
	else
	{
		wheelObservers.remove(observer);
	}
}

void Mouse::removeMouseMotionObservers()
{
	motionObservers.clear();
}

void Mouse::removeMouseButtonObservers()
{
	buttonObservers.clear();
}

void Mouse::removeMouseWheelObservers()
{
	wheelObservers.clear();
}

void Mouse::press(int button, int x, int y)
{
	// Notify observers
	notifyingButtonObservers = true;
	for (auto observer: buttonObservers)
	{
		observer->mouseButtonPressed(button, x, y);
	}
	notifyingButtonObservers = false;
	
	// Process flags
	processFlaggedButtonObservers();
}

void Mouse::release(int button, int x, int y)
{
	// Notify observers
	notifyingButtonObservers = true;
	for (auto observer: buttonObservers)
	{
		observer->mouseButtonReleased(button, x, y);
	}
	notifyingButtonObservers = false;
	
	// Process flags
	processFlaggedButtonObservers();
}

void Mouse::move(int x, int y)
{
	previousPosition = currentPosition;
	currentPosition = glm::ivec2(x, y);
	
	// Notify observers
	notifyingMotionObservers = true;
	for (auto observer: motionObservers)
	{
		observer->mouseMoved(x, y);
	}
	notifyingMotionObservers = false;
	
	// Process flags
	processFlaggedMotionObservers();
}

void Mouse::scroll(int x, int y)
{
	// Notify observers
	notifyingWheelObservers = true;
	for (auto observer: wheelObservers)
	{
		observer->mouseWheelScrolled(x, y);
	}
	notifyingWheelObservers = false;
	
	// Process flags
	processFlaggedWheelObservers();
}

void Mouse::processFlaggedMotionObservers()
{
	// Remove observers which are flagged for removal
	for (auto observer: removalFlaggedMotionObservers)
	{
		motionObservers.remove(observer);
	}
	removalFlaggedMotionObservers.clear();
	
	// Add observers which are flagged for addition
	for (auto observer: additionFlaggedMotionObservers)
	{
		motionObservers.push_back(observer);
	}
	additionFlaggedMotionObservers.clear();
}

void Mouse::processFlaggedButtonObservers()
{
	// Remove observers which are flagged for removal
	for (auto observer: removalFlaggedButtonObservers)
	{
		buttonObservers.remove(observer);
	}
	removalFlaggedButtonObservers.clear();
	
	// Add observers which are flagged for addition
	for (auto observer: additionFlaggedButtonObservers)
	{
		buttonObservers.push_back(observer);
	}
	additionFlaggedButtonObservers.clear();
}

void Mouse::processFlaggedWheelObservers()
{
	// Remove observers which are flagged for removal
	for (auto observer: removalFlaggedWheelObservers)
	{
		wheelObservers.remove(observer);
	}
	removalFlaggedWheelObservers.clear();
	
	// Add observers which are flagged for addition
	for (auto observer: additionFlaggedWheelObservers)
	{
		wheelObservers.push_back(observer);
	}
	additionFlaggedWheelObservers.clear();
}

Gamepad::Gamepad(const std::string& name):
	InputDevice(name)
{}

Gamepad::~Gamepad()
{}

void Gamepad::addGamepadButtonObserver(GamepadButtonObserver* observer)
{
	buttonObservers.push_back(observer);
}

void Gamepad::removeGamepadButtonObserver(GamepadButtonObserver* observer)
{
	buttonObservers.remove(observer);
}

void Gamepad::removeGamepadButtonObservers()
{
	buttonObservers.clear();
}

void Gamepad::addGamepadAxisObserver(GamepadAxisObserver* observer)
{
	axisObservers.push_back(observer);
}

void Gamepad::removeGamepadAxisObserver(GamepadAxisObserver* observer)
{
	axisObservers.remove(observer);
}

void Gamepad::removeGamepadAxisObservers()
{
	axisObservers.clear();
}

void Gamepad::press(int button)
{
	for (auto observer: buttonObservers)
	{
		observer->gamepadButtonPressed(button);
	}
}

void Gamepad::release(int button)
{
	for (auto observer: buttonObservers)
	{
		observer->gamepadButtonReleased(button);
	}
}

void Gamepad::move(int axis, bool negative, float value)
{
	for (auto observer: axisObservers)
	{
		observer->gamepadAxisMoved(axis, negative, value);
	}
}

InputEvent::InputEvent():
	type(InputEvent::Type::NONE)
{}

InputManager::InputManager():
	closed(false)
{}

void InputManager::addWindowObserver(WindowObserver* observer)
{
	windowObservers.push_back(observer);
}

void InputManager::removeWindowObserver(WindowObserver* observer)
{
	windowObservers.remove(observer);
}

void InputManager::removeWindowObservers()
{
	windowObservers.clear();
}

void InputManager::registerKeyboard(Keyboard* keyboard)
{
	keyboards.push_back(keyboard);
}

void InputManager::registerMouse(Mouse* mouse)
{
	mice.push_back(mouse);
}

void InputManager::registerGamepad(Gamepad* gamepad)
{
	gamepads.push_back(gamepad);
}

void InputManager::unregisterKeyboard(Keyboard* keyboard)
{
	keyboards.remove(keyboard);
}

void InputManager::unregisterMouse(Mouse* mouse)
{
	mice.remove(mouse);
}

void InputManager::unregisterGamepad(Gamepad* gamepad)
{
	gamepads.remove(gamepad);
}

bool InputManager::isRegistered(const Keyboard* keyboard) const
{
	for (auto it = keyboards.begin(); it != keyboards.end(); ++it)
	{
		if (*it == keyboard)
			return true;
	}
	
	return false;
}

bool InputManager::isRegistered(const Mouse* mouse) const
{
	for (auto it = mice.begin(); it != mice.end(); ++it)
	{
		if (*it == mouse)
			return true;
	}
	
	return false;
}

bool InputManager::isRegistered(const Gamepad* gamepad) const
{
	for (auto it = gamepads.begin(); it != gamepads.end(); ++it)
	{
		if (*it == gamepad)
			return true;
	}
	
	return false;
}

const Gamepad* InputManager::getGamepad(const std::string& name) const
{
	for (auto gamepad: gamepads)
	{
		if (gamepad->getName() == name)
			return gamepad;
	}
	
	return nullptr;
}

Gamepad* InputManager::getGamepad(const std::string& name)
{
	for (auto gamepad: gamepads)
	{
		if (gamepad->getName() == name)
			return gamepad;
	}
	
	return nullptr;
}

SDLInputManager::SDLInputManager()
{
	keyboard = new Keyboard("Default Keyboard");
	mouse = new Mouse("Default Mouse");
	
	registerKeyboard(keyboard);
	registerMouse(mouse);
	
	keyboard->setDisconnected(false);
	mouse->setDisconnected(false);
}

SDLInputManager::~SDLInputManager()
{
	unregisterKeyboard(keyboard);
	unregisterMouse(mouse);
	
	for (auto gamepad: allocatedGamepads)
	{
		unregisterGamepad(gamepad);
		delete gamepad;
	}
	
	delete keyboard;
	delete mouse;
}

void SDLInputManager::update()
{
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_KEYDOWN:
			{
				int scancode = event.key.keysym.scancode;
				keyboard->press(scancode);
				break;
			}
			
			case SDL_KEYUP:
			{
				int scancode = event.key.keysym.scancode;
				keyboard->release(scancode);
				break;
			}
			
			case SDL_MOUSEMOTION:
			{
				int x = event.motion.x;
				int y = event.motion.y;
				mouse->move(x, y);
				break;
			}
			
			case SDL_MOUSEBUTTONDOWN:
			{
				int button = event.button.button;
				mouse->press(button, event.button.x, event.button.y);
				break;
			}
			
			case SDL_MOUSEBUTTONUP:
			{
				int button = event.button.button;
				mouse->release(button, event.button.x, event.button.y);
				break;
			}
			
			case SDL_MOUSEWHEEL:
			{
				int direction = (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) ? -1 : 1;
				int x = event.wheel.x * direction;
				int y = event.wheel.y * direction;
				mouse->scroll(x, y);
				break;
			}
			
			case SDL_CONTROLLERBUTTONDOWN:
			{
				int instanceID = event.cbutton.which;
				auto it = gamepadMap.find(instanceID);
				if (it == gamepadMap.end())
				{
					std::cerr << std::string("Received event from invalid gamepad") << std::endl;
					break;
				}
				
				Gamepad* gamepad = it->second;
				int button = event.cbutton.button;
				gamepad->press(button);
				break;
			}
			
			case SDL_CONTROLLERBUTTONUP:
			{
				int instanceID = event.cbutton.which;
				auto it = gamepadMap.find(instanceID);
				if (it == gamepadMap.end())
				{
					std::cerr << std::string("Received event from invalid gamepad") << std::endl;
					break;
				}
				
				Gamepad* gamepad = it->second;
				int button = event.cbutton.button;
				gamepad->release(button);
				break;
			}
			
			case SDL_CONTROLLERAXISMOTION:
			{
				int instanceID = event.caxis.which;
				auto it = gamepadMap.find(instanceID);
				if (it == gamepadMap.end())
				{
					std::cerr << std::string("Received event from invalid gamepad") << std::endl;
					break;
				}
				
				Gamepad* gamepad = it->second;
				int axis = event.caxis.axis;
				bool negative;
				float value;
				if (event.caxis.value < 0)
				{
					negative = true;
					value = (float)event.caxis.value / -32768.0f;
				}
				else
				{
					negative = false;
					value = (float)event.caxis.value / 32767.0f;
				}
				
				gamepad->move(axis, negative, value);
				break;
			}
			
			case SDL_CONTROLLERDEVICEADDED:
			{
				SDL_GameController* controller = SDL_GameControllerOpen(event.cdevice.which);
				if (controller != nullptr)
				{
					// Find controller's joystick instance ID
					SDL_Joystick* joystick = SDL_GameControllerGetJoystick(controller);
                	int instanceID = SDL_JoystickInstanceID(joystick);
                	
					// Determine gamepad name
					std::string name = SDL_GameControllerName(controller);
					if (name.empty())
					{
						name = "Unknown Gamepad";
					}
					
					bool reconnected = false;
					const std::list<Gamepad*>* gamepads = getGamepads();
					for (auto it = gamepads->begin(); it != gamepads->end(); ++it)
					{
						// Check if this gamepad was previously connected
						if ((*it)->isDisconnected() && (*it)->getName() == name)
						{
							// Map to new instance ID
							Gamepad* gamepad = *it;
							gamepadMap[instanceID] = gamepad;
							gamepad->setDisconnected(false);
							reconnected = true;
							
							std::cout << std::string("Reconnected gamepad \"") << name << std::string("\" with ID ") << instanceID << std::endl;
							break;
						}
					}
					
					if (!reconnected)
					{
						// Create new gamepad
						Gamepad* gamepad = new Gamepad(name);
						
						// Add to list of allocated gamepads
						allocatedGamepads.push_back(gamepad);
					
						// Register with the input manager
						registerGamepad(gamepad);
					
						// Map instance ID to gamepad pointer
						gamepadMap[instanceID] = gamepad;
						
						// Connect gamepad
						gamepad->setDisconnected(false);
						
						std::cout << std::string("Connected gamepad \"") << name << std::string("\" with ID ") << instanceID << std::endl;
					}
				}
				break;
			}
			
			case SDL_CONTROLLERDEVICEREMOVED:
			{
				int instanceID = event.cdevice.which;
				
				// Find gamepad
				auto mapIt = gamepadMap.find(instanceID);
				if (mapIt == gamepadMap.end())
				{
					std::cerr << std::string("Attempted to remove nonexistent gamepad with ID ") << instanceID << std::endl;
					break;
				}
				Gamepad* gamepad = mapIt->second;
				
				// Remove from gamepad map
				gamepadMap.erase(mapIt);
				
				// Set disconnected flag
				gamepad->setDisconnected(true);
				
				std::cout << std::string("Disconnected gamepad \"") << gamepad->getName() << std::string("\" with ID ") << instanceID << std::endl;
				break;
			}
			
			case SDL_WINDOWEVENT:
			{
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					for (auto observer: windowObservers)
					{
						observer->windowResized(event.window.data1, event.window.data2);
					}
				}
				else if (event.window.event == SDL_WINDOWEVENT_CLOSE)
				{
					closed = true;
					
					for (auto observer: windowObservers)
					{
						observer->windowClosed();
					}
				}
				break;
			}
			
			case SDL_QUIT:
			{
				closed = true;
				
				for (auto observer: windowObservers)
				{
					observer->windowClosed();
				}
				break;
			}
			
			default:
				break;
		}
	}
}

void SDLInputManager::listen(InputEvent* inputEvent)
{
	int eventCount;
	
	// Gather events
	SDL_PumpEvents();
	
	// Check for key events
	eventCount = SDL_PeepEvents(&event, 1, SDL_PEEKEVENT, SDL_KEYDOWN, SDL_KEYDOWN);
	if (eventCount)
	{
		int scancode = event.key.keysym.scancode;
		inputEvent->type = InputEvent::Type::KEY;
		inputEvent->key.first = keyboard;
		inputEvent->key.second = scancode;
		return;
	}
	
	// Check for mouse button events
	eventCount = SDL_PeepEvents(&event, 1, SDL_PEEKEVENT, SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONDOWN);
	if (eventCount)
	{
		int button = event.button.button;
		inputEvent->type = InputEvent::Type::MOUSE_BUTTON;
		inputEvent->mouseButton.first = mouse;
		inputEvent->mouseButton.second = button;
		return;
	}
	
	// Check for mouse wheel events
	eventCount = SDL_PeepEvents(&event, 1, SDL_PEEKEVENT, SDL_MOUSEWHEEL, SDL_MOUSEWHEEL);
	if (eventCount)
	{
		int direction = (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) ? -1 : 1;
		int x = event.wheel.x * direction;
		int y = event.wheel.y * direction;
		inputEvent->type = InputEvent::Type::MOUSE_WHEEL;
		std::get<0>(inputEvent->mouseWheel) = mouse;
		std::get<1>(inputEvent->mouseWheel) = x;
		std::get<2>(inputEvent->mouseWheel) = y;
		return;
	}
	
	// Check for gamepad button events
	eventCount = SDL_PeepEvents(&event, 1, SDL_PEEKEVENT, SDL_CONTROLLERBUTTONDOWN, SDL_CONTROLLERBUTTONDOWN);
	if (eventCount)
	{
		int instanceID = event.cbutton.which;
		auto it = gamepadMap.find(instanceID);
		if (it == gamepadMap.end())
		{
			std::cerr << std::string("Received event from invalid gamepad") << std::endl;
			return;
		}
		
		Gamepad* gamepad = it->second;
		int button = event.cbutton.button;
		inputEvent->type = InputEvent::Type::GAMEPAD_BUTTON;
		inputEvent->gamepadButton.first = gamepad;
		inputEvent->gamepadButton.second = button;
		return;
	}
	
	// Check for gamepad axis events
	eventCount = SDL_PeepEvents(&event, 1, SDL_PEEKEVENT, SDL_CONTROLLERAXISMOTION, SDL_CONTROLLERAXISMOTION);
	if (eventCount)
	{
		int instanceID = event.caxis.which;
		auto it = gamepadMap.find(instanceID);
		if (it == gamepadMap.end())
		{
			std::cerr << std::string("Received event from invalid gamepad") << std::endl;
			return;
		}
		
		Gamepad* gamepad = it->second;
		int axis = event.caxis.axis;
		bool negative = event.caxis.value < 0;
		inputEvent->type = InputEvent::Type::GAMEPAD_AXIS;
		std::get<0>(inputEvent->gamepadAxis) = gamepad;
		std::get<1>(inputEvent->gamepadAxis) = axis;
		std::get<2>(inputEvent->gamepadAxis) = negative;
		return;
	}
}
