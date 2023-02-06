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

#ifndef ANTKEEPER_APP_DISPLAY_HPP
#define ANTKEEPER_APP_DISPLAY_HPP

#include "math/vector.hpp"
#include <string>

namespace app {

/**
 * Virtual display.
 */
class display
{
public:
	/**
	 * Sets the index of the display.
	 *
	 * @param index Index of the display.
	 */
	inline void set_index(int index) noexcept
	{
		this->index = index;
	}
	
	/**
	 * Sets the name of the display.
	 *
	 * @param name Name of the display.
	 */
	inline void set_name(const std::string& name) noexcept
	{
		this->name = name;
	}
	
	/**
	 * Sets the size of the display.
	 *
	 * @param size Size of the display, in display units.
	 */
	inline void set_size(const math::vector<int, 2>& size) noexcept
	{
		this->size = size;
	}
	
	/**
	 * Sets the refresh rate of the display.
	 *
	 * @param rate Refresh rate, in Hz.
	 */
	inline void set_refresh_rate(int rate) noexcept
	{
		this->refresh_rate = rate;
	}
	
	/**
	 * Sets the DPI of the display.
	 *
	 * @param dpi DPI.
	 */
	inline void set_dpi(float dpi) noexcept
	{
		this->dpi = dpi;
	}

	/// Returns the index of the display.
	[[nodiscard]] inline int get_index() const noexcept
	{
		return index;
	}
	
	/// Returns the name of the display.
	[[nodiscard]] inline const std::string& get_name() const noexcept
	{
		return name;
	}
	
	/// Returns the size of the display, in display units.
	[[nodiscard]] inline const math::vector<int, 2>& get_size() const noexcept
	{
		return size;
	}
	
	/// Returns the refresh rate of the display, in Hz.
	[[nodiscard]] inline int get_refresh_rate() const noexcept
	{
		return refresh_rate;
	}
	
	/// Returns the DPI of the display.
	[[nodiscard]] inline float get_dpi() const noexcept
	{
		return dpi;
	}
	
private:
	int index;
	std::string name;
	math::vector<int, 2> size;
	int refresh_rate;
	float dpi;
};

} // namespace app

#endif // ANTKEEPER_APP_DISPLAY_HPP
