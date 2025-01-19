// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RENDER_ANTI_ALIASING_METHOD_HPP
#define ANTKEEPER_RENDER_ANTI_ALIASING_METHOD_HPP

#include <cstdint>

namespace render {

/// Anti-aliasing methods.
enum class anti_aliasing_method: std::uint8_t
{
	/// No anti-aliasing.
	none,
	
	/// Fast approximate anti-aliasing (FXAA).
	fxaa
};

} // namespace render

#endif // ANTKEEPER_RENDER_ANTI_ALIASING_METHOD_HPP
