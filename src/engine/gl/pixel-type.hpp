// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_PIXEL_TYPE_HPP
#define ANTKEEPER_GL_PIXEL_TYPE_HPP

#include <cstdint>

namespace gl {

/// Pixel types.
enum class pixel_type: std::uint8_t
{
	int_8,
	uint_8,
	int_16,
	uint_16,
	int_32,
	uint_32,
	float_16,
	float_32
};

} // namespace gl

#endif // ANTKEEPER_GL_PIXEL_TYPE_HPP
