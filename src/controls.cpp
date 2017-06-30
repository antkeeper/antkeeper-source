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

#include "controls.hpp"
#include <sstream>
#include <fstream>
#include <vector>

Control::Control():
	deadzone(0.1f),
	currentValue(0.0f),
	previousValue(0.0f)
{}

Control::~Control()
{}

void Control::setDeadzone(float value)
{
	deadzone = value;
}

void Control::update()
{
	if (!boundMouseWheelAxes.empty())
	{
		currentValue = 0.0f;
	}
	
	previousValue = currentValue;
}

bool Control::isTriggered() const
{
	return currentValue > deadzone;
}

bool Control::wasTriggered() const
{
	return previousValue > deadzone;
}

bool Control::isUnbound() const
{
	return (boundKeys.empty() && boundMouseButtons.empty() && boundMouseWheelAxes.empty() && boundGamepadButtons.empty() && boundGamepadAxes.empty());
}

void Control::bindKey(Keyboard* keyboard, int scancode)
{
	// Check if already observing this keyboard
	bool observing = false;
	for (auto it: boundKeys)
	{
		if (it.first == keyboard)
		{
			observing = true;
			break;
		}
	}
	
	if (!observing)
		keyboard->addKeyObserver(static_cast<KeyObserver*>(this));
	boundKeys.push_back(std::pair<Keyboard*, int>(keyboard, scancode));
}

void Control::bindMouseButton(Mouse* mouse, int button)
{
	// Checking if already observing this mouse
	bool observing = false;
	for (auto it: boundMouseButtons)
	{
		if (it.first == mouse)
		{
			observing = true;
			break;
		}
	}
	
	if (!observing)
		mouse->addMouseButtonObserver(static_cast<MouseButtonObserver*>(this));
	boundMouseButtons.push_back(std::pair<Mouse*, int>(mouse, button));
}

void Control::bindMouseWheelAxis(Mouse* mouse, MouseWheelAxis axis)
{
	// Checking if already observing this mouse
	bool observing = false;
	for (auto it: boundMouseWheelAxes)
	{
		if (it.first == mouse)
		{
			observing = true;
			break;
		}
	}
	
	if (!observing)
		mouse->addMouseWheelObserver(static_cast<MouseWheelObserver*>(this));
	boundMouseWheelAxes.push_back(std::pair<Mouse*, MouseWheelAxis>(mouse, axis));
}

void Control::bindGamepadButton(Gamepad* gamepad, int button)
{
	bool observing = false;
	for (auto it: boundGamepadButtons)
	{
		if (it.first == gamepad)
		{
			observing = true;
			break;
		}
	}
	
	if (!observing)
		gamepad->addGamepadButtonObserver(static_cast<GamepadButtonObserver*>(this));
	boundGamepadButtons.push_back(std::pair<Gamepad*, int>(gamepad, button));
}

void Control::bindGamepadAxis(Gamepad* gamepad, int axis, bool negative)
{
	bool observing = false;
	for (auto it: boundGamepadAxes)
	{
		if (std::get<0>(it) == gamepad)
		{
			observing = true;
			break;
		}
	}
	
	if (!observing)
		gamepad->addGamepadAxisObserver(static_cast<GamepadAxisObserver*>(this));
	boundGamepadAxes.push_back(std::make_tuple(gamepad, axis, negative));
}

void Control::bind(const InputEvent& event)
{
	switch (event.type)
	{
		case InputEvent::Type::KEY:
			bindKey(event.key.first, event.key.second);
			break;
		
		case InputEvent::Type::MOUSE_BUTTON:
			bindMouseButton(event.mouseButton.first, event.mouseButton.second);
			break;
		
		case InputEvent::Type::MOUSE_WHEEL:
		{
			int x = std::get<1>(event.mouseWheel);
			int y = std::get<2>(event.mouseWheel);
			
			MouseWheelAxis axis;
			if (x > 0)
				axis = MouseWheelAxis::POSITIVE_X;
			else if (x < 0)
				axis = MouseWheelAxis::NEGATIVE_X;
			else if (y > 0)
				axis = MouseWheelAxis::POSITIVE_Y;
			else if (y < 0)
				axis = MouseWheelAxis::NEGATIVE_Y;
			else
				break;
			
			bindMouseWheelAxis(std::get<0>(event.mouseWheel), axis);
			break;
		}
		
		case InputEvent::Type::GAMEPAD_BUTTON:
			bindGamepadButton(event.gamepadButton.first, event.gamepadButton.second);
			break;
		
		case InputEvent::Type::GAMEPAD_AXIS:
			bindGamepadAxis(std::get<0>(event.gamepadAxis), std::get<1>(event.gamepadAxis), std::get<2>(event.gamepadAxis));
			break;
		
		default:
			break;
	}
}

void Control::unbind()
{
	while (!boundKeys.empty())
	{
		// Remove the first bound key and stop observing its keyboard
		Keyboard* keyboard = boundKeys.front().first;
		keyboard->removeKeyObserver(static_cast<KeyObserver*>(this));
		boundKeys.pop_front();
		
		// Remove other bound keys which are associated with the keyboard
		auto it = boundKeys.begin();
		while (it != boundKeys.end())
		{
			if (it->first == keyboard)
				boundKeys.erase(it++);
			else
				++it;
		}
	}
	
	while (!boundMouseButtons.empty())
	{
		// Remove the first bound mouse button and stop observing its mouse
		Mouse* mouse = boundMouseButtons.front().first;
		mouse->removeMouseButtonObserver(static_cast<MouseButtonObserver*>(this));
		boundMouseButtons.pop_front();
		
		// Remove other bound mouse buttons which are associated with the mouse
		auto it = boundMouseButtons.begin();
		while (it != boundMouseButtons.end())
		{
			if (it->first == mouse)
				boundMouseButtons.erase(it++);
			else
				++it;
		}
	}
	
	while (!boundMouseWheelAxes.empty())
	{
		// Remove the first bound mouse button and stop observing its mouse
		Mouse* mouse = boundMouseWheelAxes.front().first;
		mouse->removeMouseWheelObserver(static_cast<MouseWheelObserver*>(this));
		boundMouseWheelAxes.pop_front();
		
		// Remove other bound mouse buttons which are associated with the mouse
		auto it = boundMouseWheelAxes.begin();
		while (it != boundMouseWheelAxes.end())
		{
			if (it->first == mouse)
				boundMouseWheelAxes.erase(it++);
			else
				++it;
		}
	}
	
	while (!boundGamepadButtons.empty())
	{
		// Remove the first bound gamepad button and stop observing its gamepad
		Gamepad* gamepad = boundGamepadButtons.front().first;
		gamepad->removeGamepadButtonObserver(static_cast<GamepadButtonObserver*>(this));
		boundGamepadButtons.pop_front();
		
		// Remove other bound gamepad buttons which are associated with the gamepad
		auto it = boundGamepadButtons.begin();
		while (it != boundGamepadButtons.end())
		{
			if (it->first == gamepad)
				boundGamepadButtons.erase(it++);
			else
				++it;
		}
	}
	
	while (!boundGamepadAxes.empty())
	{
		// Remove the first bound gamepad axis and stop observing its gamepad
		Gamepad* gamepad = std::get<0>(boundGamepadAxes.front());
		gamepad->removeGamepadAxisObserver(static_cast<GamepadAxisObserver*>(this));
		boundGamepadAxes.pop_front();
		
		// Remove other bound gamepad axes which are associated with the gamepad
		auto it = boundGamepadAxes.begin();
		while (it != boundGamepadAxes.end())
		{
			if (std::get<0>(*it) == gamepad)
				boundGamepadAxes.erase(it++);
			else
				++it;
		}
	}
}

void Control::keyPressed(int scancode)
{
	for (auto it: boundKeys)
	{
		if (it.second == scancode)
		{
			currentValue = 1.0f;
			break;
		}
	}
}

void Control::keyReleased(int scancode)
{
	for (auto it: boundKeys)
	{
		if (it.second == scancode)
		{
			currentValue = 0.0f;
			break;
		}
	}
}

void Control::mouseButtonPressed(int button, int x, int y)
{
	for (auto it: boundMouseButtons)
	{
		if (it.second == button)
		{
			currentValue = 1.0f;
			break;
		}
	}
}

void Control::mouseButtonReleased(int button, int x, int y)
{
	for (auto it: boundMouseButtons)
	{
		if (it.second == button)
		{
			currentValue = 0.0f;
			break;
		}
	}
}

void Control::mouseWheelScrolled(int x, int y)
{
	for (auto it: boundMouseWheelAxes)
	{
		if (it.second == MouseWheelAxis::POSITIVE_X && x > 0)
		{
			currentValue += x;
			break;
		}
		else if (it.second == MouseWheelAxis::NEGATIVE_X && x < 0)
		{
			currentValue -= x;
			break;
		}
		else if (it.second == MouseWheelAxis::POSITIVE_Y && y > 0)
		{
			currentValue += y;
			break;
		}
		else if (it.second == MouseWheelAxis::NEGATIVE_Y && y < 0)
		{
			currentValue -= y;
			break;
		}
	}
}

void Control::gamepadButtonPressed(int button)
{
	for (auto it: boundGamepadButtons)
	{
		if (it.second == button)
		{
			currentValue = 1.0f;
			break;
		}
	}
}

void Control::gamepadButtonReleased(int button)
{
	for (auto it: boundGamepadButtons)
	{
		if (it.second == button)
		{
			currentValue = 0.0f;
			break;
		}
	}
}

void Control::gamepadAxisMoved(int axis, bool negative, float value)
{
	for (auto it: boundGamepadAxes)
	{
		if (std::get<1>(it) == axis && std::get<2>(it) == negative)
		{
			currentValue = value;
			break;
		}
	}
}

const std::list<std::pair<Keyboard*, int>>* Control::getBoundKeys() const
{
	return &boundKeys;
}

const std::list<std::pair<Mouse*, int>>* Control::getBoundMouseButtons() const
{
	return &boundMouseButtons;
}

const std::list<std::pair<Mouse*, MouseWheelAxis>>* Control::getBoundMouseWheelAxes() const
{
	return &boundMouseWheelAxes;
}

const std::list<std::pair<Gamepad*, int>>* Control::getBoundGamepadButtons() const
{
	return &boundGamepadButtons;
}

const std::list<std::tuple<Gamepad*, int, bool>>* Control::getBoundGamepadAxes() const
{
	return &boundGamepadAxes;
}

ControlProfile::ControlProfile(InputManager* inputManager):
	inputManager(inputManager)
{}

void ControlProfile::registerControl(const std::string& name, Control* control)
{
	controls[name] = control;
}

bool ControlProfile::save(const std::string& filename)
{
	// Open control profile
	std::ofstream file(filename.c_str());
	if (!file.is_open())
	{
		std::cerr << "Failed to open control profile \"" << filename << "\"" << std::endl;
		return false;
	}
	
	for (auto it = controls.begin(); it != controls.end(); ++it)
	{
		Control* control = it->second;
		const std::list<std::pair<Keyboard*, int>>* boundKeys = control->getBoundKeys();
		const std::list<std::pair<Mouse*, int>>* boundMouseButtons = control->getBoundMouseButtons();
		const std::list<std::pair<Mouse*, MouseWheelAxis>>* boundMouseWheelAxes = control->getBoundMouseWheelAxes();
		const std::list<std::pair<Gamepad*, int>>* boundGamepadButtons = control->getBoundGamepadButtons();
		const std::list<std::tuple<Gamepad*, int, bool>>* boundGamepadAxes = control->getBoundGamepadAxes();
		
		for (auto boundKey: *boundKeys)
		{
			int key = boundKey.second;
			file << "control\t" << it->first << "\tkeyboard\tkey\t" << key << '\n';
		}
		
		for (auto boundMouseButton: *boundMouseButtons)
		{
			int button = boundMouseButton.second;
			file << "control\t" << it->first << "\tmouse\tbutton\t" << button << '\n';
		}
		
		for (auto boundMouseWheelAxis: *boundMouseWheelAxes)
		{
			MouseWheelAxis axis = boundMouseWheelAxis.second;
			
			file << "control\t" << it->first << "\tmouse\twheel\t";
			if (axis == MouseWheelAxis::POSITIVE_X)
				file << "+x";
			else if (axis == MouseWheelAxis::NEGATIVE_X)
				file << "-x";
			else if (axis == MouseWheelAxis::POSITIVE_Y)
				file << "+y";
			else if (axis == MouseWheelAxis::NEGATIVE_Y)
				file << "-y";
			else
				file << "unknown";
			file << '\n';
		}
		
		for (auto boundGamepadButton: *boundGamepadButtons)
		{
			const std::string& gamepadName = boundGamepadButton.first->getName();
			int button = boundGamepadButton.second;
			file << "control\t" << it->first << "\tgamepad\t" << gamepadName << "\tbutton\t" << button << '\n';
		}
		
		for (auto boundGamepadAxis: *boundGamepadAxes)
		{
			const std::string& gamepadName = std::get<0>(boundGamepadAxis)->getName();
			int axis = std::get<1>(boundGamepadAxis);
			bool negative = std::get<2>(boundGamepadAxis);
			
			std::stringstream axisstream;
			if (negative)
				axisstream << "-";
			else
				axisstream << "+";
			axisstream << axis;
			
			file << "control\t" << it->first << "\tgamepad\t" << gamepadName << "\taxis\t" << axisstream.str() << '\n';
		}
	}
	
	file.close();
	
	return true;
}

bool ControlProfile::load(const std::string& filename)
{
	// Open control profile
	std::ifstream file(filename.c_str());
	if (!file.is_open())
	{
		std::cerr << "Failed to open control profile \"" << filename << "\"" << std::endl;
		return false;
	}
	
	// Read profile
	std::string line;
	while (file.good() && std::getline(file, line))
	{
		// Tokenize line (tab-delimeted)
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream linestream(line);
		while (std::getline(linestream, token, '\t'))
			tokens.push_back(token);
		
		if (tokens.empty() || tokens[0][0] == '#')
			continue;
		
		if (tokens[0] == "control" && tokens.size() >= 5)
		{
			// Use control name to get control pointer
			auto it = controls.find(tokens[1]);
			if (it == controls.end())
			{
				std::cerr << "Attempted to load unregistered control \"" << tokens[1] << "\" from control profile \"" << filename << "\"" << std::endl;
				continue;
			}
			Control* control = it->second;
			
			// Find input device
			if (tokens[2] == "keyboard")
			{
				Keyboard* keyboard = inputManager->getKeyboards()->front();
				
				if (tokens[3] == "key")
				{
					std::stringstream keystream(tokens[4]);
					int scancode = -1;
					keystream >> scancode;
					
					control->bindKey(keyboard, scancode);
				}
				else
				{
					std::cerr << "Invalid line \"" << line << "\" in control profile \"" << filename << "\"" << std::endl;
				}
			}
			else if (tokens[2] == "mouse")
			{
				Mouse* mouse = inputManager->getMice()->front();
				
				if (tokens[3] == "button")
				{
					std::stringstream buttonstream(tokens[4]);
					int button = -1;
					buttonstream >> button;
					
					control->bindMouseButton(mouse, button);
				}
				else if (tokens[3] == "wheel")
				{
					MouseWheelAxis axis;
					if (tokens[4] == "+x")
						axis = MouseWheelAxis::POSITIVE_X;
					else if (tokens[4] == "-x")
						axis = MouseWheelAxis::NEGATIVE_X;
					else if (tokens[4] == "+y")
						axis = MouseWheelAxis::POSITIVE_Y;
					else if (tokens[4] == "-y")
						axis = MouseWheelAxis::NEGATIVE_Y;
					else
					{
						std::cerr << "Invalid line \"" << line << "\" in control profile \"" << filename << "\"" << std::endl;
						continue;
					}
					
					control->bindMouseWheelAxis(mouse, axis);
				}
				else
				{
					std::cerr << "Invalid line \"" << line << "\" in control profile \"" << filename << "\"" << std::endl;
					continue;
				}
			}
			else if (tokens[2] == "gamepad")
			{
				if (tokens.size() != 6)
				{
					std::cerr << "Invalid line \"" << line << "\" in control profile \"" << filename << "\"" << std::endl;
					continue;
				}
				
				Gamepad* gamepad = inputManager->getGamepad(tokens[3]);
				if (!gamepad)
				{
					gamepad = new Gamepad(tokens[3]);
					gamepad->setDisconnected(true);
					inputManager->registerGamepad(gamepad);
				}
				
				if (tokens[4] == "button")
				{
					std::stringstream buttonstream(tokens[5]);
					int button = -1;
					buttonstream >> button;
					
					control->bindGamepadButton(gamepad, button);
				}
				else if (tokens[4] == "axis")
				{
					bool negative = (tokens[5][0] == '-');
					
					std::stringstream axisstream(tokens[5].substr(1, tokens[5].length() - 1));
					int axis = -1;
					axisstream >> axis;
					
					control->bindGamepadAxis(gamepad, axis, negative);
				}
				else
				{
					std::cerr << "Invalid line \"" << line << "\" in control profile \"" << filename << "\"" << std::endl;
					continue;
				}
			}
			else
			{
				std::cerr << "Unsupported input device \"" << tokens[3] << "\" in control profile \"" << filename << "\"" << std::endl;
				continue;
			}
		}
	}
	
	file.close();
	
	return true;
}

void ControlProfile::update()
{
	for (auto it = controls.begin(); it != controls.end(); ++it)
	{
		it->second->update();
	}
}

