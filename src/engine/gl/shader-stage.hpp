// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_SHADER_STAGE_HPP
#define ANTKEEPER_GL_SHADER_STAGE_HPP

#include <cstdint>

namespace gl {

/**
 * Enumerates all supported shader stages.
 */
enum class shader_stage: std::uint8_t
{
	/// Vertex shader stage.
	vertex,
	
	/// Fragment shader stage.
	fragment,
	
	/// Geometry shader stage.
	geometry
};

} // namespace gl

#endif // ANTKEEPER_GL_SHADER_STAGE_HPP
