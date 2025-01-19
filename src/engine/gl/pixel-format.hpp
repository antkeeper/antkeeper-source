// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_PIXEL_FORMAT_HPP
#define ANTKEEPER_GL_PIXEL_FORMAT_HPP

#include <cstdint>

namespace gl {

enum class pixel_format: std::uint8_t
{
	/// Depth
	d,
	
	/// Depth, stencil
	ds,
	
	/// Red
	r,
	
	/// Red, green
	rg,
	
	/// Red, green, blue
	rgb,
	
	/// Blue, green, red
	bgr,
	
	/// Red, green, blue, alpha
	rgba,
	
	/// Blue, green, red, alpha
	bgra
};

} // namespace gl

#endif // ANTKEEPER_GL_PIXEL_FORMAT_HPP
