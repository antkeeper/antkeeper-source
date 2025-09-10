// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <glad/gl.h>
#include <engine/gl/shader-variable.hpp>
#include <engine/utility/sized-types.hpp>
#include <vector>

namespace engine::gl::opengl
{
	/// Boolean shader variable implementation using OpenGL.
	class shader_bool: public shader_variable
	{
	public:
		shader_bool(usize size, GLint gl_uniform_location);
		~shader_bool() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::bvec1;
		}

		void update(bool value) const noexcept override;
		void update(bool value, usize index) const override;
		void update(std::span<const bool> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
		mutable std::vector<GLint> m_ivalues;
	};

	/// 2-dimensional boolean vector shader variable implementation using OpenGL.
	class shader_bvec2: public shader_variable
	{
	public:
		shader_bvec2(usize size, GLint gl_uniform_location);
		~shader_bvec2() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::bvec2;
		}

		void update(const math::bvec2& value) const noexcept override;
		void update(const math::bvec2& value, usize index) const override;
		void update(std::span<const math::bvec2> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
		mutable std::vector<GLint> m_ivalues;
	};

	/// 3-dimensional boolean vector shader variable implementation using OpenGL.
	class shader_bvec3: public shader_variable
	{
	public:
		shader_bvec3(usize size, GLint gl_uniform_location);
		~shader_bvec3() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::bvec3;
		}

		void update(const math::bvec3& value) const noexcept override;
		void update(const math::bvec3& value, usize index) const override;
		void update(std::span<const math::bvec3> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
		mutable std::vector<GLint> m_ivalues;
	};

	/// 4-dimensional boolean vector shader variable implementation using OpenGL.
	class shader_bvec4: public shader_variable
	{
	public:
		shader_bvec4(usize size, GLint gl_uniform_location);
		~shader_bvec4() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::bvec4;
		}

		void update(const math::bvec4& value) const noexcept override;
		void update(const math::bvec4& value, usize index) const override;
		void update(std::span<const math::bvec4> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
		mutable std::vector<GLint> m_ivalues;
	};

	/// Signed integer shader variable implementation using OpenGL.
	class shader_int: public shader_variable
	{
	public:
		shader_int(usize size, GLint gl_uniform_location);
		~shader_int() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::ivec1;
		}

		void update(int value) const noexcept override;
		void update(int value, usize index) const override;
		void update(std::span<const int> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
	};

	/// 2-dimensional signed integer vector shader variable implementation using OpenGL.
	class shader_ivec2: public shader_variable
	{
	public:
		shader_ivec2(usize size, GLint gl_uniform_location);
		~shader_ivec2() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::ivec2;
		}

		void update(const math::ivec2& value) const noexcept override;
		void update(const math::ivec2& value, usize index) const override;
		void update(std::span<const math::ivec2> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
	};

	/// 3-dimensional signed integer vector shader variable implementation using OpenGL.
	class shader_ivec3: public shader_variable
	{
	public:
		shader_ivec3(usize size, GLint gl_uniform_location);
		~shader_ivec3() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::ivec3;
		}

		void update(const math::ivec3& value) const noexcept override;
		void update(const math::ivec3& value, usize index) const override;
		void update(std::span<const math::ivec3> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
	};

	/// 4-dimensional signed integer vector shader variable implementation using OpenGL.
	class shader_ivec4: public shader_variable
	{
	public:
		shader_ivec4(usize size, GLint gl_uniform_location);
		~shader_ivec4() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::ivec4;
		}

		void update(const math::ivec4& value) const noexcept override;
		void update(const math::ivec4& value, usize index) const override;
		void update(std::span<const math::ivec4> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
	};

	/// Unsigned integer shader variable implementation using OpenGL.
	class shader_uint: public shader_variable
	{
	public:
		shader_uint(usize size, GLint gl_uniform_location);
		~shader_uint() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::uvec1;
		}

		void update(unsigned int value) const noexcept override;
		void update(unsigned int value, usize index) const override;
		void update(std::span<const unsigned int> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
	};

	/// 2-dimensional unsigned integer vector shader variable implementation using OpenGL.
	class shader_uvec2: public shader_variable
	{
	public:
		shader_uvec2(usize size, GLint gl_uniform_location);
		~shader_uvec2() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::uvec2;
		}

		void update(const math::uvec2& value) const noexcept override;
		void update(const math::uvec2& value, usize index) const override;
		void update(std::span<const math::uvec2> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
	};

	/// 3-dimensional unsigned integer vector shader variable implementation using OpenGL.
	class shader_uvec3: public shader_variable
	{
	public:
		shader_uvec3(usize size, GLint gl_uniform_location);
		~shader_uvec3() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::uvec3;
		}

		void update(const math::uvec3& value) const noexcept override;
		void update(const math::uvec3& value, usize index) const override;
		void update(std::span<const math::uvec3> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
	};

	/// 4-dimensional unsigned integer vector shader variable implementation using OpenGL.
	class shader_uvec4: public shader_variable
	{
	public:
		shader_uvec4(usize size, GLint gl_uniform_location);
		~shader_uvec4() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::uvec4;
		}

		void update(const math::uvec4& value) const noexcept override;
		void update(const math::uvec4& value, usize index) const override;
		void update(std::span<const math::uvec4> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
	};

	/// Floating-point shader variable implementation using OpenGL.
	class shader_float: public shader_variable
	{
	public:
		shader_float(usize size, GLint gl_uniform_location);
		~shader_float() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::fvec1;
		}

		void update(float value) const noexcept override;
		void update(float value, usize index) const override;
		void update(std::span<const float> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
	};

	/// 2-dimensional floating-point vector shader variable implementation using OpenGL.
	class shader_fvec2: public shader_variable
	{
	public:
		shader_fvec2(usize size, GLint gl_uniform_location);
		~shader_fvec2() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::fvec2;
		}

		void update(const math::fvec2& value) const noexcept override;
		void update(const math::fvec2& value, usize index) const override;
		void update(std::span<const math::fvec2> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
	};

	/// 3-dimensional floating-point vector shader variable implementation using OpenGL.
	class shader_fvec3: public shader_variable
	{
	public:
		shader_fvec3(usize size, GLint gl_uniform_location);
		~shader_fvec3() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::fvec3;
		}

		void update(const math::fvec3& value) const noexcept override;
		void update(const math::fvec3& value, usize index) const override;
		void update(std::span<const math::fvec3> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
	};

	/// 4-dimensional floating-point vector shader variable implementation using OpenGL.
	class shader_fvec4: public shader_variable
	{
	public:
		shader_fvec4(usize size, GLint gl_uniform_location);
		~shader_fvec4() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::fvec4;
		}

		void update(const math::fvec4& value) const noexcept override;
		void update(const math::fvec4& value, usize index) const override;
		void update(std::span<const math::fvec4> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
	};

	/// 2x2 floating-point matrix shader variable implementation using OpenGL.
	class shader_fmat2: public shader_variable
	{
	public:
		shader_fmat2(usize size, GLint gl_uniform_location);
		~shader_fmat2() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::fmat2;
		}

		void update(const math::fmat2& value) const noexcept override;
		void update(const math::fmat2& value, usize index) const override;
		void update(std::span<const math::fmat2> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
	};

	/// 3x3 floating-point matrix shader variable implementation using OpenGL.
	class shader_fmat3: public shader_variable
	{
	public:
		shader_fmat3(usize size, GLint gl_uniform_location);
		~shader_fmat3() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::fmat3;
		}

		void update(const math::fmat3& value) const noexcept override;
		void update(const math::fmat3& value, usize index) const override;
		void update(std::span<const math::fmat3> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
	};

	/// 4x4 floating-point matrix shader variable implementation using OpenGL.
	class shader_fmat4: public shader_variable
	{
	public:
		shader_fmat4(usize size, GLint gl_uniform_location);
		~shader_fmat4() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::fmat4;
		}

		void update(const math::fmat4& value) const noexcept override;
		void update(const math::fmat4& value, usize index) const override;
		void update(std::span<const math::fmat4> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
	};

	/// 1-dimensional texture shader variable implementation using OpenGL.
	class shader_texture_1d: public shader_variable
	{
	public:
		shader_texture_1d(usize size, GLint gl_uniform_location, GLint gl_first_texture_index);
		~shader_texture_1d() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::texture_1d;
		}

		void update(const texture_1d& value) const noexcept override;
		void update(const texture_1d& value, usize index) const override;
		void update(std::span<const texture_1d* const> values, usize index = 0) const override;
		void update(std::span<const std::shared_ptr<texture_1d>> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
		std::vector<GLint> m_gl_texture_unit_indices;
	};

	/// 2-dimensional texture shader variable implementation using OpenGL.
	class shader_texture_2d: public shader_variable
	{
	public:
		shader_texture_2d(usize size, GLint gl_uniform_location, GLint gl_first_texture_unit_index);
		~shader_texture_2d() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::texture_2d;
		}

		void update(const texture_2d& value) const noexcept override;
		void update(const texture_2d& value, usize index) const override;
		void update(std::span<const texture_2d* const> values, usize index = 0) const override;
		void update(std::span<const std::shared_ptr<texture_2d>> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
		std::vector<GLint> m_gl_texture_unit_indices;
	};

	/// 3-dimensional texture shader variable implementation using OpenGL.
	class shader_texture_3d: public shader_variable
	{
	public:
		shader_texture_3d(usize size, GLint gl_uniform_location, GLint gl_first_texture_index);
		~shader_texture_3d() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::texture_3d;
		}

		void update(const texture_3d& value) const noexcept override;
		void update(const texture_3d& value, usize index) const override;
		void update(std::span<const texture_3d* const> values, usize index = 0) const override;
		void update(std::span<const std::shared_ptr<texture_3d>> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
		std::vector<GLint> m_gl_texture_unit_indices;
	};

	/// Cube texture shader variable implementation using OpenGL.
	class shader_texture_cube: public shader_variable
	{
	public:
		shader_texture_cube(usize size, GLint gl_uniform_location, GLint gl_first_texture_index);
		~shader_texture_cube() override = default;

		[[nodiscard]] inline constexpr shader_variable_type type() const noexcept override
		{
			return shader_variable_type::texture_cube;
		}

		void update(const texture_cube& value) const noexcept override;
		void update(const texture_cube& value, usize index) const override;
		void update(std::span<const texture_cube* const> values, usize index = 0) const override;
		void update(std::span<const std::shared_ptr<texture_cube>> values, usize index = 0) const override;

	private:
		GLint m_gl_uniform_location;
		std::vector<GLint> m_gl_texture_unit_indices;
	};
}
