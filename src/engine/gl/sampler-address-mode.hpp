// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_SAMPLER_ADDRESS_MODE_HPP
#define ANTKEEPER_GL_SAMPLER_ADDRESS_MODE_HPP

#include <cstdint>

namespace gl {

/// Behaviors of sampling with texture coordinates outside an image.
enum class sampler_address_mode: std::uint8_t
{
	/// Repeat wrap mode.
	repeat,
	
	/// Mirrored repeat wrap mode.
	mirrored_repeat,
	
	/// Clamp to edge wrap mode.
	clamp_to_edge,
	
	/// Clamp to border wrap mode.
	clamp_to_border,
	
	/// Mirror clamp to edge wrap mode.
	mirror_clamp_to_edge
};

} // namespace gl

#endif // ANTKEEPER_GL_SAMPLER_ADDRESS_MODE_HPP
