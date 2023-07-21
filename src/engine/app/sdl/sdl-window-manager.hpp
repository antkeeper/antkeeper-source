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

#ifndef ANTKEEPER_APP_SDL_WINDOW_MANAGER_HPP
#define ANTKEEPER_APP_SDL_WINDOW_MANAGER_HPP

#include <engine/app/window-manager.hpp>
#include <engine/app/display.hpp>
#include <SDL2/SDL.h>
#include <unordered_map>
#include <vector>

namespace app {

class sdl_window;

/**
 * 
 */
class sdl_window_manager: public window_manager
{
public:
	/**
	 * Constructs an SDL window manager.
	 */
	sdl_window_manager();
	
	/**
	 * Destructs an SDL window manager.
	 */
	virtual ~sdl_window_manager();
	
	virtual void update();
	
	/// @copydoc window::window()
	[[nodiscard]] virtual std::shared_ptr<window> create_window
	(
		const std::string& title,
		const math::ivec2& windowed_position,
		const math::ivec2& windowed_size,
		bool maximized,
		bool fullscreen,
		bool v_sync
	);
	
	[[nodiscard]] virtual std::size_t get_display_count() const;
	[[nodiscard]] virtual const display& get_display(std::size_t index) const;
	
private:
	sdl_window* get_window(SDL_Window* internal_window);
	void update_display(int sdl_display_index);
	
	std::vector<display> m_displays;
	std::unordered_map<SDL_Window*, app::sdl_window*> m_window_map;
};

} // namespace app

#endif // ANTKEEPER_APP_SDL_WINDOW_MANAGER_HPP
