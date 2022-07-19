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

#include "gl/shader-input.hpp"
#include "gl/texture-1d.hpp"
#include "gl/texture-2d.hpp"
#include "gl/texture-3d.hpp"
#include "gl/texture-cube.hpp"
#include <glad/glad.h>

namespace gl {

shader_input::shader_input(shader_program* program, std::size_t input_index, int gl_uniform_location, const std::string& name, shader_variable_type data_type, std::size_t element_count, int texture_unit):
	program(program),
	input_index(input_index),
	gl_uniform_location(gl_uniform_location),
	name(name),
	data_type(data_type),
	element_count(element_count),
	texture_unit(texture_unit)
{}

shader_input::~shader_input()
{}

bool shader_input::upload(const bool& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform1i(gl_uniform_location, static_cast<GLint>(value));
	return true;
}

bool shader_input::upload(const bool2& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	const GLint values[] = {value[0], value[1]};
	glUniform2iv(gl_uniform_location, 1, values);
	return true;
}

bool shader_input::upload(const bool3& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	const GLint values[] = {value[0], value[1], value[2]};
	glUniform3iv(gl_uniform_location, 1, values);
	return true;
}

bool shader_input::upload(const bool4& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	const GLint values[] = {value[0], value[1], value[2], value[3]};
	glUniform4iv(gl_uniform_location, 1, values);
	return true;
}

bool shader_input::upload(const int& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform1i(gl_uniform_location, value);
	return true;
}

bool shader_input::upload(const int2& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform2iv(gl_uniform_location, 1, value.data());
	return true;
}

bool shader_input::upload(const int3& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform3iv(gl_uniform_location, 1, value.data());
	return true;
}

bool shader_input::upload(const int4& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform4iv(gl_uniform_location, 1, value.data());
	return true;
}

bool shader_input::upload(const unsigned int& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform1ui(gl_uniform_location, value);
	return true;
}

bool shader_input::upload(const uint2& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform2uiv(gl_uniform_location, 1, value.data());
	return true;
}

bool shader_input::upload(const uint3& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform3uiv(gl_uniform_location, 1, value.data());
	return true;
}

bool shader_input::upload(const uint4& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform4uiv(gl_uniform_location, 1, value.data());
	return true;
}

bool shader_input::upload(const float& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform1f(gl_uniform_location, value);
	return true;
}

bool shader_input::upload(const float2& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform2fv(gl_uniform_location, 1, value.data());
	return true;
}

bool shader_input::upload(const float3& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform3fv(gl_uniform_location, 1, value.data());
	return true;
}

bool shader_input::upload(const float4& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform4fv(gl_uniform_location, 1, value.data());
	return true;
}

bool shader_input::upload(const float2x2& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniformMatrix2fv(gl_uniform_location, 1, GL_FALSE, value[0].data());
	return true;
}

bool shader_input::upload(const float3x3& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniformMatrix3fv(gl_uniform_location, 1, GL_FALSE, value[0].data());
	return true;
}

bool shader_input::upload(const float4x4& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniformMatrix4fv(gl_uniform_location, 1, GL_FALSE, value[0].data());
	return true;
}

bool shader_input::upload(const texture_1d* value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	// Bind texture to a texture unit reserved by this shader input
	glActiveTexture(GL_TEXTURE0 + texture_unit);
	glBindTexture(GL_TEXTURE_1D, value->gl_texture_id);
	
	// Upload texture unit index to shader
	glUniform1i(gl_uniform_location, texture_unit);
	
	return true;
}

bool shader_input::upload(const texture_2d* value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	// Bind texture to a texture unit reserved by this shader input
	glActiveTexture(GL_TEXTURE0 + texture_unit);
	glBindTexture(GL_TEXTURE_2D, value->gl_texture_id);
	
	// Upload texture unit index to shader
	glUniform1i(gl_uniform_location, texture_unit);
	
	return true;
}

bool shader_input::upload(const texture_3d* value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	// Bind texture to a texture unit reserved by this shader input
	glActiveTexture(GL_TEXTURE0 + texture_unit);
	glBindTexture(GL_TEXTURE_3D, value->gl_texture_id);
	
	// Upload texture unit index to shader
	glUniform1i(gl_uniform_location, texture_unit);
	
	return true;
}

bool shader_input::upload(const texture_cube* value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	// Bind texture to a texture unit reserved by this shader input
	glActiveTexture(GL_TEXTURE0 + texture_unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, value->gl_texture_id);
	
	// Upload texture unit index to shader
	glUniform1i(gl_uniform_location, texture_unit);
	
	return true;
}

bool shader_input::upload(std::size_t index, const bool& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform1i(gl_uniform_location + static_cast<int>(index), static_cast<GLint>(value));
	return true;
}

bool shader_input::upload(std::size_t index, const bool2& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	const GLint values[] = {value[0], value[1]};
	glUniform2iv(gl_uniform_location + static_cast<int>(index), 1, values);
	return true;
}

bool shader_input::upload(std::size_t index, const bool3& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	const GLint values[] = {value[0], value[1], value[3]};
	glUniform3iv(gl_uniform_location + static_cast<int>(index), 1, values);
	return true;
}

bool shader_input::upload(std::size_t index, const bool4& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	const GLint values[] = {value[0], value[1], value[3], value[4]};
	glUniform4iv(gl_uniform_location + static_cast<int>(index), 1, values);
	return true;
}

bool shader_input::upload(std::size_t index, const int& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform1i(gl_uniform_location + static_cast<int>(index), value);
	return true;
}

bool shader_input::upload(std::size_t index, const int2& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform2iv(gl_uniform_location + static_cast<int>(index), 1, value.data());
	return true;
}

bool shader_input::upload(std::size_t index, const int3& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform3iv(gl_uniform_location + static_cast<int>(index), 1, value.data());
	return true;
}

bool shader_input::upload(std::size_t index, const int4& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform4iv(gl_uniform_location + static_cast<int>(index), 1, value.data());
	return true;
}

bool shader_input::upload(std::size_t index, const unsigned int& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform1ui(gl_uniform_location + static_cast<int>(index), value);
	return true;
}

bool shader_input::upload(std::size_t index, const uint2& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform2uiv(gl_uniform_location + static_cast<int>(index), 1, value.data());
	return true;
}

bool shader_input::upload(std::size_t index, const uint3& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform3uiv(gl_uniform_location + static_cast<int>(index), 1, value.data());
	return true;
}

bool shader_input::upload(std::size_t index, const uint4& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform4uiv(gl_uniform_location + static_cast<int>(index), 1, value.data());
	return true;
}

bool shader_input::upload(std::size_t index, const float& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform1f(gl_uniform_location + static_cast<int>(index), value);
	return true;
}

bool shader_input::upload(std::size_t index, const float2& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform2fv(gl_uniform_location + static_cast<int>(index), 1, value.data());
	return true;
}

bool shader_input::upload(std::size_t index, const float3& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform3fv(gl_uniform_location + static_cast<int>(index), 1, value.data());
	return true;
}

bool shader_input::upload(std::size_t index, const float4& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform4fv(gl_uniform_location + static_cast<int>(index), 1, value.data());
	return true;
}

bool shader_input::upload(std::size_t index, const float2x2& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniformMatrix2fv(gl_uniform_location + static_cast<int>(index) * 2, 1, GL_FALSE, value[0].data());
	return true;
}

bool shader_input::upload(std::size_t index, const float3x3& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniformMatrix3fv(gl_uniform_location + static_cast<int>(index) * 3, 1, GL_FALSE, value[0].data());
	return true;
}

bool shader_input::upload(std::size_t index, const float4x4& value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniformMatrix4fv(gl_uniform_location + static_cast<int>(index) * 4, 1, GL_FALSE, value[0].data());
	return true;
}

bool shader_input::upload(std::size_t index, const texture_1d* value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	// Bind texture to a texture unit reserved by this shader input
	glActiveTexture(GL_TEXTURE0 + texture_unit + static_cast<int>(index));
	glBindTexture(GL_TEXTURE_1D, value->gl_texture_id);
	
	// Upload texture unit index to shader
	glUniform1i(gl_uniform_location + static_cast<int>(index), texture_unit + static_cast<int>(index));
	
	return true;
}

bool shader_input::upload(std::size_t index, const texture_2d* value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	// Bind texture to a texture unit reserved by this shader input
	glActiveTexture(GL_TEXTURE0 + texture_unit + static_cast<int>(index));
	glBindTexture(GL_TEXTURE_2D, value->gl_texture_id);
	
	// Upload texture unit index to shader
	glUniform1i(gl_uniform_location + static_cast<int>(index), texture_unit + static_cast<int>(index));
	
	return true;
}

bool shader_input::upload(std::size_t index, const texture_3d* value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	// Bind texture to a texture unit reserved by this shader input
	glActiveTexture(GL_TEXTURE0 + texture_unit + static_cast<int>(index));
	glBindTexture(GL_TEXTURE_3D, value->gl_texture_id);
	
	// Upload texture unit index to shader
	glUniform1i(gl_uniform_location + static_cast<int>(index), texture_unit + static_cast<int>(index));
	
	return true;
}

bool shader_input::upload(std::size_t index, const texture_cube* value) const
{
	if (gl_uniform_location == -1)
		return false;
	
	// Bind texture to a texture unit reserved by this shader input
	glActiveTexture(GL_TEXTURE0 + texture_unit + static_cast<int>(index));
	glBindTexture(GL_TEXTURE_CUBE_MAP, value->gl_texture_id);
	
	// Upload texture unit index to shader
	glUniform1i(gl_uniform_location + static_cast<int>(index), texture_unit + static_cast<int>(index));
	
	return true;
}

bool shader_input::upload(std::size_t index, const bool* values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;
	
	int* int_values = new int[count];
	for (std::size_t i = 0; i < count; ++i)
		int_values[i] = values[i];
	glUniform1iv(gl_uniform_location + static_cast<int>(index), static_cast<GLsizei>(count), &(*int_values));
	delete[] int_values;

	return true;
}

bool shader_input::upload(std::size_t index, const bool2* values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;

	int2* int2_values = new int2[count];
	for (std::size_t i = 0; i < count; ++i)
		int2_values[i] = {values[i][0], values[i][1]};
	glUniform2iv(gl_uniform_location + static_cast<int>(index), static_cast<GLsizei>(count), &((*int2_values)[0]));
	delete[] int2_values;

	return true;
}

bool shader_input::upload(std::size_t index, const bool3* values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;

	int3* int3_values = new int3[count];
	for (std::size_t i = 0; i < count; ++i)
		int3_values[i] = {values[i][0], values[i][1], values[i][2]};
	glUniform3iv(gl_uniform_location + static_cast<int>(index), static_cast<GLsizei>(count), &((*int3_values)[0]));
	delete[] int3_values;

	return true;
}

bool shader_input::upload(std::size_t index, const bool4* values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;

	int4* int4_values = new int4[count];
	for (std::size_t i = 0; i < count; ++i)
		int4_values[i] = {values[i][0], values[i][1], values[i][2], values[i][3]};
	glUniform4iv(gl_uniform_location + static_cast<int>(index), static_cast<GLsizei>(count), &((*int4_values)[0]));
	delete[] int4_values;

	return true;
}

bool shader_input::upload(std::size_t index, const int* values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform1iv(gl_uniform_location + static_cast<int>(index), static_cast<GLsizei>(count), &(*values));
	return true;
}

bool shader_input::upload(std::size_t index, const int2* values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform2iv(gl_uniform_location + static_cast<int>(index), static_cast<GLsizei>(count), (*values).data());
	return true;
}

bool shader_input::upload(std::size_t index, const int3* values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform3iv(gl_uniform_location + static_cast<int>(index), static_cast<GLsizei>(count), (*values).data());
	return true;
}

bool shader_input::upload(std::size_t index, const int4* values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform4iv(gl_uniform_location + static_cast<int>(index), static_cast<GLsizei>(count), (*values).data());
	return true;
}

bool shader_input::upload(std::size_t index, const unsigned int* values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform1uiv(gl_uniform_location + static_cast<int>(index), static_cast<GLsizei>(count), &(*values));
	return true;
}

bool shader_input::upload(std::size_t index, const uint2* values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform2uiv(gl_uniform_location + static_cast<int>(index), static_cast<GLsizei>(count), (*values).data());
	return true;
}

bool shader_input::upload(std::size_t index, const uint3* values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform3uiv(gl_uniform_location + static_cast<int>(index), static_cast<GLsizei>(count), (*values).data());
	return true;
}

bool shader_input::upload(std::size_t index, const uint4* values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform4uiv(gl_uniform_location + static_cast<int>(index), static_cast<GLsizei>(count), (*values).data());
	return true;
}

bool shader_input::upload(std::size_t index, const float* values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform1fv(gl_uniform_location + static_cast<int>(index), static_cast<GLsizei>(count), &(*values));
	return true;
}

bool shader_input::upload(std::size_t index, const float2* values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform2fv(gl_uniform_location + static_cast<int>(index), static_cast<GLsizei>(count), (*values).data());
	return true;
}

bool shader_input::upload(std::size_t index, const float3* values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform3fv(gl_uniform_location + static_cast<int>(index), static_cast<GLsizei>(count), (*values).data());
	return true;
}

bool shader_input::upload(std::size_t index, const float4* values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniform4fv(gl_uniform_location + static_cast<int>(index), static_cast<GLsizei>(count), (*values).data());
	return true;
}

bool shader_input::upload(std::size_t index, const float2x2* values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniformMatrix2fv(gl_uniform_location + static_cast<int>(index) * 2, static_cast<GLsizei>(count), GL_FALSE, (*values)[0].data());
	return true;
}

bool shader_input::upload(std::size_t index, const float3x3* values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniformMatrix3fv(gl_uniform_location + static_cast<int>(index) * 3, static_cast<GLsizei>(count), GL_FALSE, (*values)[0].data());
	return true;
}

bool shader_input::upload(std::size_t index, const float4x4* values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;
	
	glUniformMatrix4fv(gl_uniform_location + static_cast<int>(index) * 4, static_cast<GLsizei>(count), GL_FALSE, (*values)[0].data());
	return true;
}

bool shader_input::upload(std::size_t index, const texture_1d** values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;
	
	for (std::size_t i = 0; i < count; ++i)
	{
		// Bind texture to a texture unit reserved by this shader input
		glActiveTexture(GL_TEXTURE0 + texture_unit + static_cast<int>(index + i));
		glBindTexture(GL_TEXTURE_1D, values[i]->gl_texture_id);
		
		// Upload texture unit index to shader
		glUniform1i(gl_uniform_location + static_cast<int>(index + i), texture_unit + static_cast<int>(index + i));
	}
	
	return true;
}

bool shader_input::upload(std::size_t index, const texture_2d** values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;
	
	for (std::size_t i = 0; i < count; ++i)
	{
		// Bind texture to a texture unit reserved by this shader input
		glActiveTexture(GL_TEXTURE0 + texture_unit + static_cast<int>(index + i));
		glBindTexture(GL_TEXTURE_2D, values[i]->gl_texture_id);
		
		// Upload texture unit index to shader
		glUniform1i(gl_uniform_location + static_cast<int>(index + i), texture_unit + static_cast<int>(index + i));
	}
	
	return true;
}

bool shader_input::upload(std::size_t index, const texture_3d** values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;
	
	for (std::size_t i = 0; i < count; ++i)
	{
		// Bind texture to a texture unit reserved by this shader input
		glActiveTexture(GL_TEXTURE0 + texture_unit + static_cast<int>(index + i));
		glBindTexture(GL_TEXTURE_3D, values[i]->gl_texture_id);
		
		// Upload texture unit index to shader
		glUniform1i(gl_uniform_location + static_cast<int>(index + i), texture_unit + static_cast<int>(index + i));
	}
	
	return true;
}

bool shader_input::upload(std::size_t index, const texture_cube** values, std::size_t count) const
{
	if (gl_uniform_location == -1)
		return false;
	
	for (std::size_t i = 0; i < count; ++i)
	{
		// Bind texture to a texture unit reserved by this shader input
		glActiveTexture(GL_TEXTURE0 + texture_unit + static_cast<int>(index + i));
		glBindTexture(GL_TEXTURE_CUBE_MAP, values[i]->gl_texture_id);
		
		// Upload texture unit index to shader
		glUniform1i(gl_uniform_location + static_cast<int>(index + i), texture_unit + static_cast<int>(index + i));
	}
	
	return true;
}

} // namespace gl
