// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_IMAGE_FLAG_HPP
#define ANTKEEPER_GL_IMAGE_FLAG_HPP

#include <cstdint>

namespace gl {

/// Image flags.
enum class image_flag: std::uint8_t
{
	/// Cube map view compatible image.
	cube_compatible = 0b00000001
};

} // namespace gl

#endif // ANTKEEPER_GL_IMAGE_FLAG_HPP
