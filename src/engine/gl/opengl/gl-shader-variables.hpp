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

#ifndef ANTKEEPER_GL_GL_SHADER_VARIABLES_HPP
#define ANTKEEPER_GL_GL_SHADER_VARIABLES_HPP

#include <engine/gl/shader-variable.hpp>
#include <glad/glad.h>
#include <vector>

namespace gl {

/**
 * Boolean shader variable implementation using OpenGL.
 */
class gl_shader_bool: public shader_variable
{
public:
	gl_shader_bool(std::size_t size, GLint gl_uniform_location);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::bool1;
	}
	
	void update(bool value) const noexcept override;
	void update(bool value, std::size_t index) const override;
	void update(std::span<const bool> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
	mutable std::vector<GLint> ivalues;
};

/**
 * 2-dimensional boolean vector shader variable implementation using OpenGL.
 */
class gl_shader_bool2: public shader_variable
{
public:
	gl_shader_bool2(std::size_t size, GLint gl_uniform_location);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::bool2;
	}
	
	void update(const bool2& value) const noexcept override;
	void update(const bool2& value, std::size_t index) const override;
	void update(std::span<const bool2> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
	mutable std::vector<GLint> ivalues;
};

/**
 * 3-dimensional boolean vector shader variable implementation using OpenGL.
 */
class gl_shader_bool3: public shader_variable
{
public:
	gl_shader_bool3(std::size_t size, GLint gl_uniform_location);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::bool3;
	}
	
	void update(const bool3& value) const noexcept override;
	void update(const bool3& value, std::size_t index) const override;
	void update(std::span<const bool3> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
	mutable std::vector<GLint> ivalues;
};

/**
 * 4-dimensional boolean vector shader variable implementation using OpenGL.
 */
class gl_shader_bool4: public shader_variable
{
public:
	gl_shader_bool4(std::size_t size, GLint gl_uniform_location);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::bool4;
	}
	
	void update(const bool4& value) const noexcept override;
	void update(const bool4& value, std::size_t index) const override;
	void update(std::span<const bool4> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
	mutable std::vector<GLint> ivalues;
};

/**
 * Signed integer shader variable implementation using OpenGL.
 */
class gl_shader_int: public shader_variable
{
public:
	gl_shader_int(std::size_t size, GLint gl_uniform_location);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::int1;
	}
	
	void update(int value) const noexcept override;
	void update(int value, std::size_t index) const override;
	void update(std::span<const int> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
};

/**
 * 2-dimensional signed integer vector shader variable implementation using OpenGL.
 */
class gl_shader_int2: public shader_variable
{
public:
	gl_shader_int2(std::size_t size, GLint gl_uniform_location);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::int2;
	}
	
	void update(const int2& value) const noexcept override;
	void update(const int2& value, std::size_t index) const override;
	void update(std::span<const int2> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
};

/**
 * 3-dimensional signed integer vector shader variable implementation using OpenGL.
 */
class gl_shader_int3: public shader_variable
{
public:
	gl_shader_int3(std::size_t size, GLint gl_uniform_location);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::int3;
	}
	
	void update(const int3& value) const noexcept override;
	void update(const int3& value, std::size_t index) const override;
	void update(std::span<const int3> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
};

/**
 * 4-dimensional signed integer vector shader variable implementation using OpenGL.
 */
class gl_shader_int4: public shader_variable
{
public:
	gl_shader_int4(std::size_t size, GLint gl_uniform_location);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::int4;
	}
	
	void update(const int4& value) const noexcept override;
	void update(const int4& value, std::size_t index) const override;
	void update(std::span<const int4> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
};

/**
 * Unsigned integer shader variable implementation using OpenGL.
 */
class gl_shader_uint: public shader_variable
{
public:
	gl_shader_uint(std::size_t size, GLint gl_uniform_location);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::uint1;
	}
	
	void update(unsigned int value) const noexcept override;
	void update(unsigned int value, std::size_t index) const override;
	void update(std::span<const unsigned int> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
};

/**
 * 2-dimensional unsigned integer vector shader variable implementation using OpenGL.
 */
class gl_shader_uint2: public shader_variable
{
public:
	gl_shader_uint2(std::size_t size, GLint gl_uniform_location);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::uint2;
	}
	
	void update(const uint2& value) const noexcept override;
	void update(const uint2& value, std::size_t index) const override;
	void update(std::span<const uint2> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
};

/**
 * 3-dimensional unsigned integer vector shader variable implementation using OpenGL.
 */
class gl_shader_uint3: public shader_variable
{
public:
	gl_shader_uint3(std::size_t size, GLint gl_uniform_location);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::uint3;
	}
	
	void update(const uint3& value) const noexcept override;
	void update(const uint3& value, std::size_t index) const override;
	void update(std::span<const uint3> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
};

/**
 * 4-dimensional unsigned integer vector shader variable implementation using OpenGL.
 */
class gl_shader_uint4: public shader_variable
{
public:
	gl_shader_uint4(std::size_t size, GLint gl_uniform_location);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::uint4;
	}
	
	void update(const uint4& value) const noexcept override;
	void update(const uint4& value, std::size_t index) const override;
	void update(std::span<const uint4> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
};

/**
 * Floating-point shader variable implementation using OpenGL.
 */
class gl_shader_float: public shader_variable
{
public:
	gl_shader_float(std::size_t size, GLint gl_uniform_location);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::float1;
	}
	
	void update(float value) const noexcept override;
	void update(float value, std::size_t index) const override;
	void update(std::span<const float> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
};

/**
 * 2-dimensional floating-point vector shader variable implementation using OpenGL.
 */
class gl_shader_float2: public shader_variable
{
public:
	gl_shader_float2(std::size_t size, GLint gl_uniform_location);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::float2;
	}
	
	void update(const float2& value) const noexcept override;
	void update(const float2& value, std::size_t index) const override;
	void update(std::span<const float2> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
};

/**
 * 3-dimensional floating-point vector shader variable implementation using OpenGL.
 */
class gl_shader_float3: public shader_variable
{
public:
	gl_shader_float3(std::size_t size, GLint gl_uniform_location);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::float3;
	}
	
	void update(const float3& value) const noexcept override;
	void update(const float3& value, std::size_t index) const override;
	void update(std::span<const float3> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
};

/**
 * 4-dimensional floating-point vector shader variable implementation using OpenGL.
 */
class gl_shader_float4: public shader_variable
{
public:
	gl_shader_float4(std::size_t size, GLint gl_uniform_location);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::float4;
	}
	
	void update(const float4& value) const noexcept override;
	void update(const float4& value, std::size_t index) const override;
	void update(std::span<const float4> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
};

/**
 * 2x2 floating-point matrix shader variable implementation using OpenGL.
 */
class gl_shader_float2x2: public shader_variable
{
public:
	gl_shader_float2x2(std::size_t size, GLint gl_uniform_location);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::float2x2;
	}
	
	void update(const float2x2& value) const noexcept override;
	void update(const float2x2& value, std::size_t index) const override;
	void update(std::span<const float2x2> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
};

/**
 * 3x3 floating-point matrix shader variable implementation using OpenGL.
 */
class gl_shader_float3x3: public shader_variable
{
public:
	gl_shader_float3x3(std::size_t size, GLint gl_uniform_location);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::float3x3;
	}
	
	void update(const float3x3& value) const noexcept override;
	void update(const float3x3& value, std::size_t index) const override;
	void update(std::span<const float3x3> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
};

/**
 * 4x4 floating-point matrix shader variable implementation using OpenGL.
 */
class gl_shader_float4x4: public shader_variable
{
public:
	gl_shader_float4x4(std::size_t size, GLint gl_uniform_location);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::float4x4;
	}
	
	void update(const float4x4& value) const noexcept override;
	void update(const float4x4& value, std::size_t index) const override;
	void update(std::span<const float4x4> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
};

/**
 * 1-dimensional texture shader variable implementation using OpenGL.
 */
class gl_shader_texture_1d: public shader_variable
{
public:
	gl_shader_texture_1d(std::size_t size, GLint gl_uniform_location, GLint gl_first_texture_index);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::texture_1d;
	}
	
	void update(const texture_1d& value) const noexcept override;
	void update(const texture_1d& value, std::size_t index) const override;
	void update(std::span<const texture_1d* const> values, std::size_t index = 0) const override;
	void update(std::span<const std::shared_ptr<texture_1d>> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
	std::vector<GLint> gl_texture_unit_indices;
};

/**
 * 2-dimensional texture shader variable implementation using OpenGL.
 */
class gl_shader_texture_2d: public shader_variable
{
public:
	gl_shader_texture_2d(std::size_t size, GLint gl_uniform_location, GLint gl_first_texture_unit_index);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::texture_2d;
	}
	
	void update(const texture_2d& value) const noexcept override;
	void update(const texture_2d& value, std::size_t index) const override;
	void update(std::span<const texture_2d* const> values, std::size_t index = 0) const override;
	void update(std::span<const std::shared_ptr<texture_2d>> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
	std::vector<GLint> gl_texture_unit_indices;
};

/**
 * 3-dimensional texture shader variable implementation using OpenGL.
 */
class gl_shader_texture_3d: public shader_variable
{
public:
	gl_shader_texture_3d(std::size_t size, GLint gl_uniform_location, GLint gl_first_texture_index);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::texture_3d;
	}
	
	void update(const texture_3d& value) const noexcept override;
	void update(const texture_3d& value, std::size_t index) const override;
	void update(std::span<const texture_3d* const> values, std::size_t index = 0) const override;
	void update(std::span<const std::shared_ptr<texture_3d>> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
	std::vector<GLint> gl_texture_unit_indices;
};

/**
 * Cube texture shader variable implementation using OpenGL.
 */
class gl_shader_texture_cube: public shader_variable
{
public:
	gl_shader_texture_cube(std::size_t size, GLint gl_uniform_location, GLint gl_first_texture_index);
	
	[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
	{
		return shader_variable_type::texture_cube;
	}
	
	void update(const texture_cube& value) const noexcept override;
	void update(const texture_cube& value, std::size_t index) const override;
	void update(std::span<const texture_cube* const> values, std::size_t index = 0) const override;
	void update(std::span<const std::shared_ptr<texture_cube>> values, std::size_t index = 0) const override;
	
private:
	GLint gl_uniform_location;
	std::vector<GLint> gl_texture_unit_indices;
};

} // namespace gl

#endif // ANTKEEPER_GL_GL_SHADER_VARIABLES_HPP
