// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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

void shader_variable::update(const math::bvec2& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::bvec3& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::bvec4& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(int value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::ivec2& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::ivec3& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::ivec4& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(unsigned int value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::uvec2& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::uvec3& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::uvec4& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(float value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::fvec2& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::fvec3& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::fvec4& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::fmat2& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::fmat3& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::fmat4& value) const
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

void shader_variable::update(const math::bvec2& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::bvec3& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::bvec4& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(int value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::ivec2& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::ivec3& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::ivec4& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(unsigned int value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::uvec2& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::uvec3& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::uvec4& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(float value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::fvec2& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::fvec3& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::fvec4& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::fmat2& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::fmat3& value, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(const math::fmat4& value, std::size_t index) const
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

void shader_variable::update(std::span<const math::bvec2> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const math::bvec3> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const math::bvec4> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const int> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const math::ivec2> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const math::ivec3> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const math::ivec4> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const unsigned int> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const math::uvec2> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const math::uvec3> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const math::uvec4> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const float> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const math::fvec2> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const math::fvec3> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const math::fvec4> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const math::fmat2> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const math::fmat3> values, std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update(std::span<const math::fmat4> values, std::size_t index) const
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
