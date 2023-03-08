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

#include <engine/gl/shader-variable.hpp>
#include <stdexcept>

namespace gl {

static const char* type_mismatch_error = "Shader variable type mismatch";

shader_variable::shader_variable(std::size_t size) noexcept:
	m_size{size}
{}

void shader_variable::update(bool value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const bool2& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const bool3& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const bool4& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(int value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const int2& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const int3& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const int4& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(unsigned int value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const uint2& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const uint3& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const uint4& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(float value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const float2& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const float3& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const float4& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const float2x2& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const float3x3& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const float4x4& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const texture_1d& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const texture_2d& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const texture_3d& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const texture_cube& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(bool value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const bool2& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const bool3& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const bool4& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(int value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const int2& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const int3& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const int4& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(unsigned int value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const uint2& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const uint3& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const uint4& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(float value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const float2& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const float3& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const float4& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const float2x2& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const float3x3& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const float4x4& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const texture_1d& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const texture_2d& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const texture_3d& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const texture_cube& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const bool> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const bool2> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const bool3> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const bool4> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const int> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const int2> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const int3> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const int4> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const unsigned int> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const uint2> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const uint3> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const uint4> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const float> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const float2> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const float3> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const float4> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const float2x2> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const float3x3> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const float4x4> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const texture_1d* const> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const texture_2d* const> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const texture_3d* const> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const texture_cube* const> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const std::shared_ptr<texture_1d>> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const std::shared_ptr<texture_2d>> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const std::shared_ptr<texture_3d>> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const std::shared_ptr<texture_cube>> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

} // namespace gl
