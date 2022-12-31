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

#include "sdl-game-controller-tables.hpp"
#include "gamepad.hpp"

namespace input {

const gamepad_button sdl_button_table[15] =
{
	gamepad_button::a, // SDL_CONTROLLER_BUTTON_A,
	gamepad_button::b, // SDL_CONTROLLER_BUTTON_B,
	gamepad_button::x, // SDL_CONTROLLER_BUTTON_X,
	gamepad_button::y, // SDL_CONTROLLER_BUTTON_Y,
	gamepad_button::back, // SDL_CONTROLLER_BUTTON_BACK,
	gamepad_button::guide, // SDL_CONTROLLER_BUTTON_GUIDE,
	gamepad_button::start, // SDL_CONTROLLER_BUTTON_START,
	gamepad_button::left_stick, // SDL_CONTROLLER_BUTTON_LEFTSTICK,
	gamepad_button::right_stick, // SDL_CONTROLLER_BUTTON_RIGHTSTICK,
	gamepad_button::left_shoulder, // SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
	gamepad_button::right_shoulder, // SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
	gamepad_button::dpad_up, // SDL_CONTROLLER_BUTTON_DPAD_UP,
	gamepad_button::dpad_down, // SDL_CONTROLLER_BUTTON_DPAD_DOWN,
	gamepad_button::dpad_left, // SDL_CONTROLLER_BUTTON_DPAD_LEFT,
	gamepad_button::dpad_right, // SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
};

const gamepad_axis sdl_axis_table[6] =
{
	gamepad_axis::left_x, // SDL_CONTROLLER_AXIS_LEFTX,
	gamepad_axis::left_y, // SDL_CONTROLLER_AXIS_LEFTY,
	gamepad_axis::right_x, // SDL_CONTROLLER_AXIS_RIGHTX,
	gamepad_axis::right_y, // SDL_CONTROLLER_AXIS_RIGHTY,
	gamepad_axis::left_trigger, // SDL_CONTROLLER_AXIS_TRIGGERLEFT,
	gamepad_axis::right_trigger, // SDL_CONTROLLER_AXIS_TRIGGERRIGHT,
};

} // namespace input
