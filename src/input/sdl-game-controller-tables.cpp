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

#include "sdl-game-controller-tables.hpp"
#include "game-controller.hpp"

const game_controller_button sdl_button_table[15] =
{
	game_controller_button::a, // SDL_CONTROLLER_BUTTON_A,
	game_controller_button::b, // SDL_CONTROLLER_BUTTON_B,
	game_controller_button::x, // SDL_CONTROLLER_BUTTON_X,
	game_controller_button::y, // SDL_CONTROLLER_BUTTON_Y,
	game_controller_button::back, // SDL_CONTROLLER_BUTTON_BACK,
	game_controller_button::guide, // SDL_CONTROLLER_BUTTON_GUIDE,
	game_controller_button::start, // SDL_CONTROLLER_BUTTON_START,
	game_controller_button::left_stick, // SDL_CONTROLLER_BUTTON_LEFTSTICK,
	game_controller_button::right_stick, // SDL_CONTROLLER_BUTTON_RIGHTSTICK,
	game_controller_button::left_shoulder, // SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
	game_controller_button::right_shoulder, // SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
	game_controller_button::dpad_up, // SDL_CONTROLLER_BUTTON_DPAD_UP,
	game_controller_button::dpad_down, // SDL_CONTROLLER_BUTTON_DPAD_DOWN,
	game_controller_button::dpad_left, // SDL_CONTROLLER_BUTTON_DPAD_LEFT,
	game_controller_button::dpad_right, // SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
};

const game_controller_axis sdl_axis_table[6] =
{
	game_controller_axis::left_x, // SDL_CONTROLLER_AXIS_LEFTX,
	game_controller_axis::left_y, // SDL_CONTROLLER_AXIS_LEFTY,
	game_controller_axis::right_x, // SDL_CONTROLLER_AXIS_RIGHTX,
	game_controller_axis::right_y, // SDL_CONTROLLER_AXIS_RIGHTY,
	game_controller_axis::trigger_left, // SDL_CONTROLLER_AXIS_TRIGGERLEFT,
	game_controller_axis::trigger_right, // SDL_CONTROLLER_AXIS_TRIGGERRIGHT,
};

