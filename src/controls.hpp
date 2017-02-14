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

#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include "input.hpp"
#include <list>
#include <utility>
#include <tuple>
#include <map>

class Control:
	public KeyObserver,
	public MouseButtonObserver,
	public GamepadButtonObserver,
	public GamepadAxisObserver
{
public:
	Control();
	virtual ~Control();
	void setDeadzone(float value);
	void update();
	
	float getDeadzone() const;
	float getCurrentValue() const;
	float getPreviousValue() const;
	bool isTriggered() const;
	bool wasTriggered() const;
	bool isUnbound() const;
	
	void bindKey(Keyboard* keyboard, int scancode);
	void bindMouseButton(Mouse* mouse, int button);
	void bindGamepadButton(Gamepad* gamepad, int button);
	void bindGamepadAxis(Gamepad* gamepad, int axis, bool negative);
	void bind(const InputEvent& event);
	
	void unbind();
	
	virtual void keyPressed(int scancode);
	virtual void keyReleased(int scancode);
	virtual void mouseButtonPressed(int button, int x, int y);
	virtual void mouseButtonReleased(int button, int x, int y);
	virtual void gamepadButtonPressed(int button);
	virtual void gamepadButtonReleased(int button);
	virtual void gamepadAxisMoved(int axis, bool negative, float value);
	
	const std::list<std::pair<Keyboard*, int>>* getBoundKeys() const;
	const std::list<std::pair<Mouse*, int>>* getBoundMouseButtons() const;
	const std::list<std::pair<Gamepad*, int>>* getBoundGamepadButtons() const;
	const std::list<std::tuple<Gamepad*, int, bool>>* getBoundGamepadAxes() const;
	
private:
	float deadzone;
	float currentValue;
	float previousValue;
	
	std::list<std::pair<Keyboard*, int>> boundKeys;
	std::list<std::pair<Mouse*, int>> boundMouseButtons;
	std::list<std::pair<Gamepad*, int>> boundGamepadButtons;
	std::list<std::tuple<Gamepad*, int, bool>> boundGamepadAxes;
};

inline float Control::getDeadzone() const
{
	return deadzone;
}

inline float Control::getCurrentValue() const
{
	return currentValue;
}

inline float Control::getPreviousValue() const
{
	return previousValue;
}

class ControlProfile
{
public:
	ControlProfile(InputManager* inputManager);
	
	void registerControl(const std::string& name, Control* control);
	
	bool save(const std::string& filename);
	bool load(const std::string& filename);
	
	// Calls Control::update() on each control registered with this profile
	void update();
	
	const std::map<std::string, Control*>* getControlMap() const;
	
private:
	InputManager* inputManager;
	std::map<std::string, Control*> controls;
};

inline const std::map<std::string, Control*>* ControlProfile::getControlMap() const
{
	return &controls;
}

#endif

