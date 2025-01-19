// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_INDEX_TYPE_HPP
#define ANTKEEPER_GL_INDEX_TYPE_HPP

#include <cstdint>

namespace gl {

/// Index types for index buffers.
enum class index_type: std::uint8_t
{
	/// 8-bit unsigned integer values.
	uint8,
	
	/// 16-bit unsigned integer values.
	uint16,
	
	/// 32-bit unsigned integer values.
	uint32
};

} // namespace gl

#endif // ANTKEEPER_GL_INDEX_TYPE_HPP
