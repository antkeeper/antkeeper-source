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

#ifndef ANTKEEPER_GL_SCISSOR_REGION_HPP
#define ANTKEEPER_GL_SCISSOR_REGION_HPP

#include <cstdint>

namespace gl {

/**
 * Scissor region offset and extents.
 */
struct scissor_region
{
	/// X-coordinate offset of the scissor region.
	std::int32_t x{};
	
	/// Y-coordinate offset of the scissor region.
	std::int32_t y{};
	
	/// Width of the scissor region.
	std::uint32_t width;
	
	/// Height of the scissor region.
	std::uint32_t height;
};

} // namespace gl

#endif // ANTKEEPER_GL_SCISSOR_REGION_HPP
