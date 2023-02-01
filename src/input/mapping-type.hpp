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

#ifndef ANTKEEPER_INPUT_MAPPING_TYPE_HPP
#define ANTKEEPER_INPUT_MAPPING_TYPE_HPP

namespace input {

/**
 * Input mapping types.
 *
 * @see input::mapping
 */
enum class mapping_type
{
	/// Gamepad axis mapping.
	gamepad_axis,
	
	/// Gamepad button mapping.
	gamepad_button,
	
	/// Key mapping.
	key,
	
	/// Mouse button mapping.
	mouse_button,
	
	/// Mouse motion mapping.
	mouse_motion,
	
	/// Mouse scroll mapping.
	mouse_scroll
};

} // namespace input

#endif // ANTKEEPER_INPUT_MAPPING_TYPE_HPP
