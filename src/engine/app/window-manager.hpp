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

#ifndef ANTKEEPER_APP_WINDOW_MANAGER_HPP
#define ANTKEEPER_APP_WINDOW_MANAGER_HPP

#include <engine/app/display.hpp>
#include <engine/app/window.hpp>
#include <engine/math/vector.hpp>
#include <memory>
#include <string>

namespace app {

/**
 * 
 */
class window_manager
{
public:
	/**
	 * Allocates and returns a window manager.
	 */
	static window_manager* instance();
	
	/// Destructs a window manager.
	virtual ~window_manager() = default;
	
	/**
	 * Updates all managed windows. This should be called once per frame.
	 */
	virtual void update() = 0;
	
	/**
	 * Constructs a window.
	 *
	 * @param title Title of the window.
	 * @param windowed_position Windowed (non-maximized, non-fullscreen) position of the window, in display units.
	 * @param windowed_size Windowed (non-maximized, non-fullscreen) size of the window, in display units.
	 * @param maximized `true` if the window should start maximized, `false` otherwise.
	 * @param fullscreen `true` if the window should start fullscreen, `false` otherwise.
	 * @param v_sync `true` if v-sync should be enabled, `false` otherwise.
	 */
	[[nodiscard]] virtual std::shared_ptr<window> create_window
	(
		const std::string& title,
		const math::vector<int, 2>& windowed_position,
		const math::vector<int, 2>& windowed_size,
		bool maximized,
		bool fullscreen,
		bool v_sync
	) = 0;
	
	/// Returns the number of available displays.
	[[nodiscard]] virtual std::size_t get_display_count() const = 0;
	
	/**
	 * Returns the display with the given index.
	 *
	 * @param index Index of a display.
	 *
	 * @return Display with the given index.
	 */
	[[nodiscard]] virtual const display& get_display(std::size_t index) const = 0;
};

} // namespace app

#endif // ANTKEEPER_APP_WINDOW_MANAGER_HPP
