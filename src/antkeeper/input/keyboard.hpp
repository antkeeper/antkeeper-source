/*
 * Copyright (C) 2020  Christopher J. Howard
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

#ifndef ANTKEEPER_KEYBOARD_HPP
#define ANTKEEPER_KEYBOARD_HPP

#include "input-device.hpp"
#include <map>
#include <string>

enum class scancode;

/**
 * A virtual keyboard which can generate keyboard-related input events and pass them to an event dispatcher.
 */
class keyboard: public input_device
{
public:
	/**
	 * Returns the UTF-8 encoded name of a scancode.
	 */
	static const char* get_scancode_name(scancode scancode);

	/**
	 * Returns the scancode corresponding to a scancode name.
	 *
	 * @param name Name of a scancode.
	 * @return Corresponding scancode, or nullptr if a matching scancode was not found.
	 */
	static scancode get_scancode_from_name(const char* name);

	/**
	 * Creates a keyboard input device.
	 */
	keyboard();

	/// Destroys a keyboard input device.
	virtual ~keyboard();
	
	/**
	 * Simulates a key press.
	 *
	 * @param scancode scancode of the simulated key press.
	 */
	void press(scancode scancode);

	/**
	 * Simulates a key release.
	 *
	 * @param scancode scancode of the simulated key release.
	 */
	void release(scancode scancode);

private:
	static std::map<std::string, scancode> build_scancode_map();
	static const char* scancode_names[];
	static std::map<std::string, scancode> scancode_map;
};

#endif // ANTKEEPER_KEYBOARD_HPP

