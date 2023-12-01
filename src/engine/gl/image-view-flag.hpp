// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_IMAGE_VIEW_FLAG_HPP
#define ANTKEEPER_GL_IMAGE_VIEW_FLAG_HPP

#include <cstdint>

namespace gl {

/// Image flags.
enum class image_view_flag: std::uint8_t
{
	/// Image array view.
	array = 0b00000001,
	
	/// Cube map view.
	cube = 0b00000010
};

} // namespace gl

#endif // ANTKEEPER_GL_IMAGE_VIEW_FLAG_HPP
