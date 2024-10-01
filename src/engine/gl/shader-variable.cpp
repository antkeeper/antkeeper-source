// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/gl/shader-variable.hpp>
#include <stdexcept>

namespace gl {

static const char* type_mismatch_error = "Shader variable type mismatch";

shader_variable::shader_variable(std::size_t size) noexcept:
	m_size{size}
{}

void shader_variable::update([[maybe_unused]] bool value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::bvec2& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::bvec3& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::bvec4& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] int value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::ivec2& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::ivec3& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::ivec4& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] unsigned int value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::uvec2& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::uvec3& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::uvec4& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] float value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::fvec2& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::fvec3& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::fvec4& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::fmat2& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::fmat3& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::fmat4& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const texture_1d& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const texture_2d& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const texture_3d& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const texture_cube& value) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] bool value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::bvec2& value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::bvec3& value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::bvec4& value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] int value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::ivec2& value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::ivec3& value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::ivec4& value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] unsigned int value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::uvec2& value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::uvec3& value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::uvec4& value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] float value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::fvec2& value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::fvec3& value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::fvec4& value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::fmat2& value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::fmat3& value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const math::fmat4& value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const texture_1d& value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const texture_2d& value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const texture_3d& value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] const texture_cube& value, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const bool> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const math::bvec2> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const math::bvec3> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const math::bvec4> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const int> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const math::ivec2> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const math::ivec3> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const math::ivec4> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const unsigned int> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const math::uvec2> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const math::uvec3> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const math::uvec4> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const float> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const math::fvec2> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const math::fvec3> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const math::fvec4> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const math::fmat2> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const math::fmat3> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const math::fmat4> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const texture_1d* const> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const texture_2d* const> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const texture_3d* const> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const texture_cube* const> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const std::shared_ptr<texture_1d>> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const std::shared_ptr<texture_2d>> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const std::shared_ptr<texture_3d>> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

void shader_variable::update([[maybe_unused]] std::span<const std::shared_ptr<texture_cube>> values, [[maybe_unused]] std::size_t index) const
{
	throw std::invalid_argument(type_mismatch_error);
}

} // namespace gl
