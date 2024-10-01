// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RENDER_MATERIAL_VARIABLE_TYPE_HPP
#define ANTKEEPER_RENDER_MATERIAL_VARIABLE_TYPE_HPP

#include <cstdint>

namespace render {

/// Material variable data types.
enum class material_variable_type: std::uint8_t
{
	bvec1,
	bvec2,
	bvec3,
	bvec4,
	ivec1,
	ivec2,
	ivec3,
	ivec4,
	uvec1,
	uvec2,
	uvec3,
	uvec4,
	fvec1,
	fvec2,
	fvec3,
	fvec4,
	fmat2,
	fmat3,
	fmat4,
	texture_1d,
	texture_2d,
	texture_3d,
	texture_cube
};

} // namespace render

#endif // ANTKEEPER_RENDER_MATERIAL_VARIABLE_TYPE_HPP
