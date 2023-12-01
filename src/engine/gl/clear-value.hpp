// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_CLEAR_VALUE_HPP
#define ANTKEEPER_GL_CLEAR_VALUE_HPP

#include <array>
#include <cstdint>


namespace gl {

/**
 * Clear value.
 */
struct clear_value
{
	/// Color clear values.
	std::array<float, 4> color{};
	
	/// Depth clear value.
	float depth{};
	
	/// Stencil clear value.
	std::uint32_t stencil{};
};

} // namespace gl

#endif // ANTKEEPER_GL_CLEAR_VALUE_HPP
