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

#ifndef ANTKEEPER_RENDER_MATERIAL_SHADOW_MODE_HPP
#define ANTKEEPER_RENDER_MATERIAL_SHADOW_MODE_HPP

#include <cstdint>

namespace render {

/**
 * Material shadow casting modes.
 */
enum class material_shadow_mode: std::uint8_t
{
	/// Material does not cast shadows.
	none,
	
	/// Material casts fully opaque shadows.
	opaque
};

} // namespace render

#endif // ANTKEEPER_RENDER_MATERIAL_SHADOW_MODE_HPP