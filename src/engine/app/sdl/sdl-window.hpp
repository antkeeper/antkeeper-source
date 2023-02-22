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

#ifndef ANTKEEPER_APP_SDL_WINDOW_HPP
#define ANTKEEPER_APP_SDL_WINDOW_HPP

#include <engine/app/window.hpp>
#include <SDL2/SDL.h>

namespace app {

/**
 * 
 */
class sdl_window: public window
{
public:
	sdl_window
	(
		const std::string& title,
		const math::vector<int, 2>& windowed_position,
		const math::vector<int, 2>& windowed_size,
		bool maximized,
		bool fullscreen,
		bool v_sync
	);
	
	sdl_window(const sdl_window&) = delete;
	sdl_window(sdl_window&&) = delete;
	sdl_window& operator=(const sdl_window&) = delete;
	sdl_window& operator=(sdl_window&&) = delete;
	
	virtual ~sdl_window();
	virtual void set_title(const std::string& title);
	virtual void set_position(const math::vector<int, 2>& position);
	virtual void set_size(const math::vector<int, 2>& size);
	virtual void set_minimum_size(const math::vector<int, 2>& size);
	virtual void set_maximum_size(const math::vector<int, 2>& size);
	virtual void set_maximized(bool maximized);
	virtual void set_fullscreen(bool fullscreen);
	virtual void set_v_sync(bool v_sync);
	virtual void make_current();
	virtual void swap_buffers();
	
	[[nodiscard]] inline virtual gl::rasterizer* get_rasterizer() noexcept
	{
		return rasterizer;
	}
	
private:
	friend class sdl_window_manager;
	
	SDL_Window* internal_window;
	SDL_GLContext internal_context;
	gl::rasterizer* rasterizer;
};

} // namespace app

#endif // ANTKEEPER_APP_SDL_WINDOW_HPP
