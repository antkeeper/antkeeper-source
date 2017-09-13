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

#ifndef INPUT_HPP
#define INPUT_HPP

#include <emergent/emergent.hpp>
using namespace Emergent;

#include <string>
#include <iostream>
#include <map>
#include <list>
#include <SDL2/SDL.h>

class KeyObserver
{
public:
	virtual void keyPressed(int scancode) = 0;
	virtual void keyReleased(int scancode) = 0;
};

class MouseMotionObserver
{
public:
	virtual void mouseMoved(int x, int y) = 0;
};

class MouseButtonObserver
{
public:
	virtual void mouseButtonPressed(int button, int x, int y) = 0;
	virtual void mouseButtonReleased(int button, int x, int y) = 0;
};

class MouseWheelObserver
{
public:
	virtual void mouseWheelScrolled(int x, int y) = 0;
};

class GamepadButtonObserver
{
public:
	virtual void gamepadButtonPressed(int button) = 0;
	virtual void gamepadButtonReleased(int button) = 0;
};

class GamepadAxisObserver
{
public:
	virtual void gamepadAxisMoved(int axis, bool negative, float value) = 0;
};

class WindowObserver
{
public:
	virtual void windowClosed() = 0;
	virtual void windowResized(int width, int height) = 0;
};

class InputDevice
{
public:
	enum class Type
	{
		KEYBOARD,
		MOUSE,
		GAMEPAD
	};
	
	InputDevice(const std::string& name);
	const std::string& getName() const;
	virtual InputDevice::Type getType() const = 0;
	
	void setDisconnected(bool disconnected);
	bool isDisconnected() const;
	
private:
	std::string name;
	bool disconnected;
};

inline const std::string& InputDevice::getName() const
{
	return name;
}

inline bool InputDevice::isDisconnected() const
{
	return disconnected;
}

class Keyboard: public InputDevice
{
public:
	Keyboard(const std::string& name);
	virtual ~Keyboard();
	
	InputDevice::Type getType() const;
	
	void addKeyObserver(KeyObserver* observer);
	void removeKeyObserver(KeyObserver* observer);
	void removeKeyObservers();
	
	void press(int scancode);
	void release(int scancode);
	
private:
	std::list<KeyObserver*> keyObservers;
};

inline InputDevice::Type Keyboard::getType() const
{
	return InputDevice::Type::KEYBOARD;
}

class Mouse: public InputDevice
{
public:
	Mouse(const std::string& name);
	virtual ~Mouse();
	
	InputDevice::Type getType() const;
	
	void addMouseMotionObserver(MouseMotionObserver* observer);
	void addMouseButtonObserver(MouseButtonObserver* observer);
	void addMouseWheelObserver(MouseWheelObserver* observer);
	void removeMouseMotionObserver(MouseMotionObserver* observer);
	void removeMouseButtonObserver(MouseButtonObserver* observer);
	void removeMouseWheelObserver(MouseWheelObserver* observer);
	void removeMouseMotionObservers();
	void removeMouseButtonObservers();
	void removeMouseWheelObservers();
	
	void press(int button, int x, int y);
	void release(int button, int x, int y);
	void move(int x, int y);
	void scroll(int x, int y);
	
	const glm::ivec2& getCurrentPosition() const;
	const glm::ivec2& getPreviousPosition() const;
	
private:
	void processFlaggedMotionObservers();
	void processFlaggedButtonObservers();
	void processFlaggedWheelObservers();

	glm::ivec2 currentPosition;
	glm::ivec2 previousPosition;
	std::list<MouseMotionObserver*> motionObservers;
	std::list<MouseButtonObserver*> buttonObservers;
	std::list<MouseWheelObserver*> wheelObservers;
	bool notifyingMotionObservers;
	bool notifyingButtonObservers;
	bool notifyingWheelObservers;
	std::list<MouseMotionObserver*> additionFlaggedMotionObservers;
	std::list<MouseButtonObserver*> additionFlaggedButtonObservers;
	std::list<MouseWheelObserver*> additionFlaggedWheelObservers;
	std::list<MouseMotionObserver*> removalFlaggedMotionObservers;
	std::list<MouseButtonObserver*> removalFlaggedButtonObservers;
	std::list<MouseWheelObserver*> removalFlaggedWheelObservers;
};

inline InputDevice::Type Mouse::getType() const
{
	return InputDevice::Type::MOUSE;
}

inline const glm::ivec2& Mouse::getCurrentPosition() const
{
	return currentPosition;
}

inline const glm::ivec2& Mouse::getPreviousPosition() const
{
	return previousPosition;
}

class Gamepad: public InputDevice
{
public:
	Gamepad(const std::string& name);
	virtual ~Gamepad();
	
	InputDevice::Type getType() const;
	
	void addGamepadButtonObserver(GamepadButtonObserver* observer);
	void removeGamepadButtonObserver(GamepadButtonObserver* observer);
	void removeGamepadButtonObservers();
	
	void addGamepadAxisObserver(GamepadAxisObserver* observer);
	void removeGamepadAxisObserver(GamepadAxisObserver* observer);
	void removeGamepadAxisObservers();
	
	void press(int button);
	void release(int button);
	void move(int axis, bool negative, float value);
	
private:
	std::list<GamepadButtonObserver*> buttonObservers;
	std::list<GamepadAxisObserver*> axisObservers;
};

inline InputDevice::Type Gamepad::getType() const
{
	return InputDevice::Type::GAMEPAD;
}

struct InputEvent
{
public:
	enum class Type
	{
		NONE,
		KEY,
		MOUSE_BUTTON,
		MOUSE_WHEEL,
		GAMEPAD_BUTTON,
		GAMEPAD_AXIS
	};
	
	InputEvent();
	
	InputEvent::Type type;
	std::pair<Keyboard*, int> key;
	std::pair<Mouse*, int> mouseButton;
	std::tuple<Mouse*, int, int> mouseWheel;
	std::pair<Gamepad*, int> gamepadButton;
	std::tuple<Gamepad*, int, int> gamepadAxis;
};

class InputManager
{
public:
	InputManager();
	
	// Processes input events
	virtual void update() = 0;
	
	// Listens for the next input event, should be called BEFORE update()
	virtual void listen(InputEvent* inputEvent) = 0;
	
	bool wasClosed() const;
	
	void addWindowObserver(WindowObserver* observer);
	void removeWindowObserver(WindowObserver* observer);
	void removeWindowObservers();
	
	void registerKeyboard(Keyboard* keyboard);
	void registerMouse(Mouse* mouse);
	void registerGamepad(Gamepad* gamepad);
	
	void unregisterKeyboard(Keyboard* keyboard);
	void unregisterMouse(Mouse* mouse);
	void unregisterGamepad(Gamepad* gamepad);
	
	bool isRegistered(const Keyboard* keyboard) const;
	bool isRegistered(const Mouse* mouse) const;
	bool isRegistered(const Gamepad* gamepad) const;
	
	const Gamepad* getGamepad(const std::string& name) const;
	Gamepad* getGamepad(const std::string& name);
	
	const std::list<Keyboard*>* getKeyboards() const;
	const std::list<Mouse*>* getMice() const;
	const std::list<Gamepad*>* getGamepads() const;
	
protected:
	bool closed;
	std::list<WindowObserver*> windowObservers;
	
private:
	std::list<Keyboard*> keyboards;
	std::list<Mouse*> mice;
	std::list<Gamepad*> gamepads;
};

inline bool InputManager::wasClosed() const
{
	return closed;
}

inline const std::list<Keyboard*>* InputManager::getKeyboards() const
{
	return &keyboards;
}

inline const std::list<Mouse*>* InputManager::getMice() const
{
	return &mice;
}

inline const std::list<Gamepad*>* InputManager::getGamepads() const
{
	return &gamepads;
}

class SDLInputManager: public InputManager
{
public:
	SDLInputManager();
	~SDLInputManager();
	
	virtual void update();
	virtual void listen(InputEvent* event);
	
private:
	Keyboard* keyboard;
	Mouse* mouse;
	std::map<int, Gamepad*> gamepadMap;
	SDL_Event event;
	std::list<Gamepad*> allocatedGamepads;
};

#endif

