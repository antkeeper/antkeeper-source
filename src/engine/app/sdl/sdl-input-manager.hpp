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

#ifndef ANTKEEPER_APP_SDL_INPUT_MANAGER_HPP
#define ANTKEEPER_APP_SDL_INPUT_MANAGER_HPP

#include <engine/app/input-manager.hpp>
#include <memory>

namespace app {

class sdl_window;

/**
 * Input manager implementation using SDL2.
 */
class sdl_input_manager: public input_manager
{
public:
	/**
	 * Constructs an SDL input manager.
	 */
	sdl_input_manager();
	
	/**
	 * Destructs an SDL input manager.
	 */
	virtual ~sdl_input_manager();
	
	void update() override;
	void set_cursor_visible(bool visible) override;
	void set_relative_mouse_mode(bool enabled) override;

private:
	input::keyboard keyboard;
	input::mouse mouse;
	std::unordered_map<int, std::unique_ptr<input::gamepad>> gamepad_map;
};

} // namespace app

#endif // ANTKEEPER_APP_SDL_INPUT_MANAGER_HPP
