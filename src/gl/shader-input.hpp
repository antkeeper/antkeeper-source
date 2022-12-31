/*
 * Copyright (C) 2023  Christopher J. Howard
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

#ifndef ANTKEEPER_GL_SHADER_INPUT_HPP
#define ANTKEEPER_GL_SHADER_INPUT_HPP

#include "utility/fundamental-types.hpp"
#include <string>

namespace gl {

class shader_program;
class texture_1d;
class texture_2d;
class texture_3d;
class texture_cube;
enum class shader_variable_type;

/**
 * Port through which data can be uploaded to shader variables.
 */
class shader_input
{
public:
	/**
	 * Returns the type of data which can be passed through this input.
	 */
	shader_variable_type get_data_type() const;
	
	/**
	 * Returns `true` if the input data is stored in an array.
	 */
	bool is_array() const;
	
	/**
	 * Returns the number of elements the array can contain, or `1` if the data is not stored in an array.
	 */
	std::size_t get_element_count() const;
	
	/**
	 * Uploads a value to the shader.
	 *
	 * @param value Value to upload.
	 * @return `true` if the value was uploaded successfully, `false` otherwise.
	 */
	///@{
	bool upload(const bool& value) const;
	bool upload(const bool2& value) const;
	bool upload(const bool3& value) const;
	bool upload(const bool4& value) const;
	bool upload(const int& value) const;
	bool upload(const int2& value) const;
	bool upload(const int3& value) const;
	bool upload(const int4& value) const;
	bool upload(const unsigned int& value) const;
	bool upload(const uint2& value) const;
	bool upload(const uint3& value) const;
	bool upload(const uint4& value) const;
	bool upload(const float& value) const;
	bool upload(const float2& value) const;
	bool upload(const float3& value) const;
	bool upload(const float4& value) const;
	bool upload(const float2x2& value) const;
	bool upload(const float3x3& value) const;
	bool upload(const float4x4& value) const;
	bool upload(const texture_1d* value) const;
	bool upload(const texture_2d* value) const;
	bool upload(const texture_3d* value) const;
	bool upload(const texture_cube* value) const;
	///@}
	
	/**
	 * Uploads a single array element to the shader.
	 *
	 * @param index Index of an array element.
	 * @param values Value to upload.
	 * @return `true` if the value was uploaded successfully, `false` otherwise.
	 */
	///@{
	bool upload(std::size_t index, const bool& value) const;
	bool upload(std::size_t index, const bool2& value) const;
	bool upload(std::size_t index, const bool3& value) const;
	bool upload(std::size_t index, const bool4& value) const;
	bool upload(std::size_t index, const int& value) const;
	bool upload(std::size_t index, const int2& value) const;
	bool upload(std::size_t index, const int3& value) const;
	bool upload(std::size_t index, const int4& value) const;
	bool upload(std::size_t index, const unsigned int& value) const;
	bool upload(std::size_t index, const uint2& value) const;
	bool upload(std::size_t index, const uint3& value) const;
	bool upload(std::size_t index, const uint4& value) const;
	bool upload(std::size_t index, const float& value) const;
	bool upload(std::size_t index, const float2& value) const;
	bool upload(std::size_t index, const float3& value) const;
	bool upload(std::size_t index, const float4& value) const;
	bool upload(std::size_t index, const float2x2& value) const;
	bool upload(std::size_t index, const float3x3& value) const;
	bool upload(std::size_t index, const float4x4& value) const;
	bool upload(std::size_t index, const texture_1d* value) const;
	bool upload(std::size_t index, const texture_2d* value) const;
	bool upload(std::size_t index, const texture_3d* value) const;
	bool upload(std::size_t index, const texture_cube* value) const;
	///@}
	
	/**
	 * Uploads a range of array elements to the shader.
	 *
	 * @param index Index of the first array element.
	 * @param values Pointer to an array of values.
	 * @param count Number of elements to upload.
	 * @return `true` if the value was fed successfully, `false` otherwise.
	 */
	///@{
	bool upload(std::size_t index, const bool* values, std::size_t count) const;
	bool upload(std::size_t index, const bool2* values, std::size_t count) const;
	bool upload(std::size_t index, const bool3* values, std::size_t count) const;
	bool upload(std::size_t index, const bool4* values, std::size_t count) const;
	bool upload(std::size_t index, const int* values, std::size_t count) const;
	bool upload(std::size_t index, const int2* values, std::size_t count) const;
	bool upload(std::size_t index, const int3* values, std::size_t count) const;
	bool upload(std::size_t index, const int4* values, std::size_t count) const;
	bool upload(std::size_t index, const unsigned int* values, std::size_t count) const;
	bool upload(std::size_t index, const uint2* values, std::size_t count) const;
	bool upload(std::size_t index, const uint3* values, std::size_t count) const;
	bool upload(std::size_t index, const uint4* values, std::size_t count) const;
	bool upload(std::size_t index, const float* values, std::size_t count) const;
	bool upload(std::size_t index, const float2* values, std::size_t count) const;
	bool upload(std::size_t index, const float3* values, std::size_t count) const;
	bool upload(std::size_t index, const float4* values, std::size_t count) const;
	bool upload(std::size_t index, const float2x2* values, std::size_t count) const;
	bool upload(std::size_t index, const float3x3* values, std::size_t count) const;
	bool upload(std::size_t index, const float4x4* values, std::size_t count) const;
	bool upload(std::size_t index, const texture_1d** values, std::size_t count) const;
	bool upload(std::size_t index, const texture_2d** values, std::size_t count) const;
	bool upload(std::size_t index, const texture_3d** values, std::size_t count) const;
	bool upload(std::size_t index, const texture_cube** values, std::size_t count) const;
	///@}
	
private:
	friend class shader_program;

	/**
	 * Creates a shader input.
	 *
	 * @param program Shader program with which this input is associated.
	 * @param gl_uniform_location Location of the shader uniform with which this shader input is associated.
	 * @param name Name of the input.
	 * @param data_type Type of data which can be passed through this input.
	 * @param element_count Number of elements which the array can contain, or `0` if input data is not stored in an array.
	 * @param texture_unit Texture unit to which texture shader variables can be bound, or `-1` if the data type is not a texture type.
	 */
	shader_input(shader_program* program, std::size_t input_index, int gl_uniform_location, const std::string& name, shader_variable_type data_type, std::size_t element_count, int texture_unit);
	
	/**
	 * Destroys a shader input.
	 */
	~shader_input();
	
	shader_program* program;
	std::size_t input_index;
	int gl_uniform_location;
	std::string name;
	shader_variable_type data_type;
	std::size_t element_count;
	int texture_unit;
};

inline shader_variable_type shader_input::get_data_type() const
{
	return data_type;
}

inline bool shader_input::is_array() const
{
	return (element_count > 1);
}

inline std::size_t shader_input::get_element_count() const
{
	return element_count;
}

} // namespace gl

#endif // ANTKEEPER_GL_SHADER_INPUT_HPP

