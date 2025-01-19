// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_SAMPLER_MIPMAP_MODE_HPP
#define ANTKEEPER_GL_SAMPLER_MIPMAP_MODE_HPP

#include <cstdint>

namespace gl {

/// Mipmap modes used for texture lookups.
enum class sampler_mipmap_mode: std::uint8_t
{
	/// Nearest filtering.
	nearest,
	
	/// Linear filtering.
	linear
};

} // namespace gl

#endif // ANTKEEPER_GL_SAMPLER_MIPMAP_MODE_HPP
