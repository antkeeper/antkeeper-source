// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
