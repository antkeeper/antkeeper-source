/*
 * Copyright (C) 2021  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANTKEEPER_GL_SHADER_VARIABLE_TYPE_HPP
#define ANTKEEPER_GL_SHADER_VARIABLE_TYPE_HPP

namespace gl {

enum class shader_variable_type
{
	bool1,
	bool2,
	bool3,
	bool4,
	int1,
	int2,
	int3,
	int4,
	uint1,
	uint2,
	uint3,
	uint4,
	float1,
	float2,
	float3,
	float4,
	float2x2,
	float3x3,
	float4x4,
	texture_2d,
	texture_cube
};

} // namespace gl

#endif // ANTKEEPER_GL_SHADER_VARIABLE_TYPE_HPP

