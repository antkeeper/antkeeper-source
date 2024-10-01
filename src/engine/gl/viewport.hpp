// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_VIEWPORT_HPP
#define ANTKEEPER_GL_VIEWPORT_HPP

#include <cstdint>

namespace gl {

/// Viewport position, dimensions, and depth range.
struct viewport
{
	/// X-coordinate of the viewport's lower left corner.
	float x{0.0f};
	
	/// Y-coordinate of the viewport's lower left corner.
	float y{0.0f};
	
	/// Width of the viewport.
	float width{0.0f};
	
	/// Height of the viewport.
	float height{0.0f};
	
	/// Minimum depth range of the viewport.
	float min_depth{0.0f};
	
	/// Maximum depth range of the viewport.
	float max_depth{1.0f};
};

} // namespace gl

#endif // ANTKEEPER_GL_VIEWPORT_HPP
