// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_SAMPLER_FILTER_HPP
#define ANTKEEPER_GL_SAMPLER_FILTER_HPP

#include <cstdint>

namespace gl {

/// Filters used for texture lookups.
enum class sampler_filter: std::uint8_t
{
	/// Nearest filtering.
	nearest,
	
	/// Linear filtering.
	linear
};

} // namespace gl

#endif // ANTKEEPER_GL_SAMPLER_FILTER_HPP
