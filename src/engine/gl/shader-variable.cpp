// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/gl/shader-variable.hpp>
#include <engine/utility/sized-types.hpp>
#include <stdexcept>

namespace engine::gl
{
	static const char* type_mismatch_error = "Shader variable type mismatch";

	shader_variable::shader_variable(usize size) noexcept:
		m_size{size}
	{
	}

	void shader_variable::update(bool) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::bvec2&) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::bvec3&) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::bvec4&) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(int) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::ivec2&) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::ivec3&) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::ivec4&) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(unsigned int) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::uvec2&) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::uvec3&) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::uvec4&) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(float) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::fvec2&) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::fvec3&) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::fvec4&) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::fmat2&) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::fmat3&) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::fmat4&) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const texture_1d&) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const texture_2d&) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const texture_3d&) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const texture_cube&) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(bool, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::bvec2&, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::bvec3&, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::bvec4&, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(int, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::ivec2&, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::ivec3&, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::ivec4&, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(unsigned int, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::uvec2&, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::uvec3&, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::uvec4&, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(float, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::fvec2&, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::fvec3&, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::fvec4&, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::fmat2&, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::fmat3&, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const math::fmat4&, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const texture_1d&, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const texture_2d&, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const texture_3d&, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(const texture_cube&, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const bool>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const math::bvec2>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const math::bvec3>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const math::bvec4>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const int>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const math::ivec2>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const math::ivec3>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const math::ivec4>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const unsigned int>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const math::uvec2>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const math::uvec3>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const math::uvec4>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const float>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const math::fvec2>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const math::fvec3>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const math::fvec4>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const math::fmat2>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const math::fmat3>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const math::fmat4>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const texture_1d* const>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const texture_2d* const>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const texture_3d* const>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const texture_cube* const>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const std::shared_ptr<texture_1d>>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const std::shared_ptr<texture_2d>>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const std::shared_ptr<texture_3d>>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}

	void shader_variable::update(std::span<const std::shared_ptr<texture_cube>>, usize) const
	{
		throw std::invalid_argument(type_mismatch_error);
	}
}
