// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <glad/gl.h>
#include <numeric>
import engine.gl.opengl.shader_variables;
import engine.gl.texture;
import engine.utility.sized_types;

namespace engine::gl::opengl
{
	shader_bool::shader_bool(usize size, GLint gl_uniform_location):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location},
		m_ivalues(size)
	{
	}

	void shader_bool::update(bool value) const noexcept
	{
		glUniform1i(m_gl_uniform_location, static_cast<GLint>(value));
	}

	void shader_bool::update(bool value, usize index) const
	{
		glUniform1i(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLint>(value));
	}

	void shader_bool::update(std::span<const bool> values, usize index) const
	{
		for (usize i = 0; i < values.size(); ++i)
		{
			m_ivalues[i] = static_cast<GLint>(values[i]);
		}

		glUniform1iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), m_ivalues.data());
	}

	shader_bvec2::shader_bvec2(usize size, GLint gl_uniform_location):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location},
		m_ivalues(size * 2)
	{
	}

	void shader_bvec2::update(const math::bvec2& value) const noexcept
	{
		glUniform2i(m_gl_uniform_location, static_cast<GLint>(value[0]), static_cast<GLint>(value[1]));
	}

	void shader_bvec2::update(const math::bvec2& value, usize index) const
	{
		glUniform2i(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLint>(value[0]), static_cast<GLint>(value[1]));
	}

	void shader_bvec2::update(std::span<const math::bvec2> values, usize index) const
	{
		GLint* ivalue = m_ivalues.data();
		for (const auto& value : values)
		{
			*(++ivalue) = static_cast<GLint>(value[0]);
			*(++ivalue) = static_cast<GLint>(value[1]);
		}

		glUniform2iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), m_ivalues.data());
	}

	shader_bvec3::shader_bvec3(usize size, GLint gl_uniform_location):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location},
		m_ivalues(size * 3)
	{
	}

	void shader_bvec3::update(const math::bvec3& value) const noexcept
	{
		glUniform3i(m_gl_uniform_location, static_cast<GLint>(value[0]), static_cast<GLint>(value[1]), static_cast<GLint>(value[2]));
	}

	void shader_bvec3::update(const math::bvec3& value, usize index) const
	{
		glUniform3i(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLint>(value[0]), static_cast<GLint>(value[1]), static_cast<GLint>(value[2]));
	}

	void shader_bvec3::update(std::span<const math::bvec3> values, usize index) const
	{
		GLint* ivalue = m_ivalues.data();
		for (const auto& value : values)
		{
			*(++ivalue) = static_cast<GLint>(value[0]);
			*(++ivalue) = static_cast<GLint>(value[1]);
			*(++ivalue) = static_cast<GLint>(value[2]);
		}

		glUniform3iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), m_ivalues.data());
	}

	shader_bvec4::shader_bvec4(usize size, GLint gl_uniform_location):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location},
		m_ivalues(size * 4)
	{
	}

	void shader_bvec4::update(const math::bvec4& value) const noexcept
	{
		glUniform4i(m_gl_uniform_location, static_cast<GLint>(value[0]), static_cast<GLint>(value[1]), static_cast<GLint>(value[2]), static_cast<GLint>(value[3]));
	}

	void shader_bvec4::update(const math::bvec4& value, usize index) const
	{
		glUniform4i(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLint>(value[0]), static_cast<GLint>(value[1]), static_cast<GLint>(value[2]), static_cast<GLint>(value[3]));
	}

	void shader_bvec4::update(std::span<const math::bvec4> values, usize index) const
	{
		GLint* ivalue = m_ivalues.data();
		for (const auto& value : values)
		{
			*(++ivalue) = static_cast<GLint>(value[0]);
			*(++ivalue) = static_cast<GLint>(value[1]);
			*(++ivalue) = static_cast<GLint>(value[2]);
			*(++ivalue) = static_cast<GLint>(value[3]);
		}

		glUniform4iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), m_ivalues.data());
	}

	shader_int::shader_int(usize size, GLint gl_uniform_location):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location}
	{
	}

	void shader_int::update(int value) const noexcept
	{
		glUniform1i(m_gl_uniform_location, value);
	}

	void shader_int::update(int value, usize index) const
	{
		glUniform1i(m_gl_uniform_location + static_cast<GLint>(index), value);
	}

	void shader_int::update(std::span<const int> values, usize index) const
	{
		glUniform1iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data());
	}

	shader_ivec2::shader_ivec2(usize size, GLint gl_uniform_location):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location}
	{
	}

	void shader_ivec2::update(const math::ivec2& value) const noexcept
	{
		glUniform2iv(m_gl_uniform_location, 1, value.data());
	}

	void shader_ivec2::update(const math::ivec2& value, usize index) const
	{
		glUniform2iv(m_gl_uniform_location + static_cast<GLint>(index), 1, value.data());
	}

	void shader_ivec2::update(std::span<const math::ivec2> values, usize index) const
	{
		glUniform2iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data()->data());
	}

	shader_ivec3::shader_ivec3(usize size, GLint gl_uniform_location):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location}
	{
	}

	void shader_ivec3::update(const math::ivec3& value) const noexcept
	{
		glUniform3iv(m_gl_uniform_location, 1, value.data());
	}

	void shader_ivec3::update(const math::ivec3& value, usize index) const
	{
		glUniform3iv(m_gl_uniform_location + static_cast<GLint>(index), 1, value.data());
	}

	void shader_ivec3::update(std::span<const math::ivec3> values, usize index) const
	{
		glUniform3iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data()->data());
	}

	shader_ivec4::shader_ivec4(usize size, GLint gl_uniform_location):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location}
	{
	}

	void shader_ivec4::update(const math::ivec4& value) const noexcept
	{
		glUniform4iv(m_gl_uniform_location, 1, value.data());
	}

	void shader_ivec4::update(const math::ivec4& value, usize index) const
	{
		glUniform4iv(m_gl_uniform_location + static_cast<GLint>(index), 1, value.data());
	}

	void shader_ivec4::update(std::span<const math::ivec4> values, usize index) const
	{
		glUniform4iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data()->data());
	}

	shader_uint::shader_uint(usize size, GLint gl_uniform_location):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location}
	{
	}

	void shader_uint::update(unsigned int value) const noexcept
	{
		glUniform1ui(m_gl_uniform_location, value);
	}

	void shader_uint::update(unsigned int value, usize index) const
	{
		glUniform1ui(m_gl_uniform_location + static_cast<GLint>(index), value);
	}

	void shader_uint::update(std::span<const unsigned int> values, usize index) const
	{
		glUniform1uiv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data());
	}

	shader_uvec2::shader_uvec2(usize size, GLint gl_uniform_location):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location}
	{
	}

	void shader_uvec2::update(const math::uvec2& value) const noexcept
	{
		glUniform2uiv(m_gl_uniform_location, 1, value.data());
	}

	void shader_uvec2::update(const math::uvec2& value, usize index) const
	{
		glUniform2uiv(m_gl_uniform_location + static_cast<GLint>(index), 1, value.data());
	}

	void shader_uvec2::update(std::span<const math::uvec2> values, usize index) const
	{
		glUniform2uiv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data()->data());
	}

	shader_uvec3::shader_uvec3(usize size, GLint gl_uniform_location):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location}
	{
	}

	void shader_uvec3::update(const math::uvec3& value) const noexcept
	{
		glUniform3uiv(m_gl_uniform_location, 1, value.data());
	}

	void shader_uvec3::update(const math::uvec3& value, usize index) const
	{
		glUniform3uiv(m_gl_uniform_location + static_cast<GLint>(index), 1, value.data());
	}

	void shader_uvec3::update(std::span<const math::uvec3> values, usize index) const
	{
		glUniform3uiv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data()->data());
	}

	shader_uvec4::shader_uvec4(usize size, GLint gl_uniform_location):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location}
	{
	}

	void shader_uvec4::update(const math::uvec4& value) const noexcept
	{
		glUniform4uiv(m_gl_uniform_location, 1, value.data());
	}

	void shader_uvec4::update(const math::uvec4& value, usize index) const
	{
		glUniform4uiv(m_gl_uniform_location + static_cast<GLint>(index), 1, value.data());
	}

	void shader_uvec4::update(std::span<const math::uvec4> values, usize index) const
	{
		glUniform4uiv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data()->data());
	}

	shader_float::shader_float(usize size, GLint gl_uniform_location):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location}
	{
	}

	void shader_float::update(float value) const noexcept
	{
		glUniform1f(m_gl_uniform_location, value);
	}

	void shader_float::update(float value, usize index) const
	{
		glUniform1f(m_gl_uniform_location + static_cast<GLint>(index), value);
	}

	void shader_float::update(std::span<const float> values, usize index) const
	{
		glUniform1fv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data());
	}

	shader_fvec2::shader_fvec2(usize size, GLint gl_uniform_location):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location}
	{
	}

	void shader_fvec2::update(const math::fvec2& value) const noexcept
	{
		glUniform2fv(m_gl_uniform_location, 1, value.data());
	}

	void shader_fvec2::update(const math::fvec2& value, usize index) const
	{
		glUniform2fv(m_gl_uniform_location + static_cast<GLint>(index), 1, value.data());
	}

	void shader_fvec2::update(std::span<const math::fvec2> values, usize index) const
	{
		glUniform2fv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data()->data());
	}

	shader_fvec3::shader_fvec3(usize size, GLint gl_uniform_location):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location}
	{
	}

	void shader_fvec3::update(const math::fvec3& value) const noexcept
	{
		glUniform3fv(m_gl_uniform_location, 1, value.data());
	}

	void shader_fvec3::update(const math::fvec3& value, usize index) const
	{
		glUniform3fv(m_gl_uniform_location + static_cast<GLint>(index), 1, value.data());
	}

	void shader_fvec3::update(std::span<const math::fvec3> values, usize index) const
	{
		glUniform3fv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data()->data());
	}

	shader_fvec4::shader_fvec4(usize size, GLint gl_uniform_location):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location}
	{
	}

	void shader_fvec4::update(const math::fvec4& value) const noexcept
	{
		glUniform4fv(m_gl_uniform_location, 1, value.data());
	}

	void shader_fvec4::update(const math::fvec4& value, usize index) const
	{
		glUniform4fv(m_gl_uniform_location + static_cast<GLint>(index), 1, value.data());
	}

	void shader_fvec4::update(std::span<const math::fvec4> values, usize index) const
	{
		glUniform4fv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data()->data());
	}

	shader_fmat2::shader_fmat2(usize size, GLint gl_uniform_location):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location}
	{
	}

	void shader_fmat2::update(const math::fmat2& value) const noexcept
	{
		glUniformMatrix2fv(m_gl_uniform_location, 1, GL_FALSE, value.data()->data());
	}

	void shader_fmat2::update(const math::fmat2& value, usize index) const
	{
		glUniformMatrix2fv(m_gl_uniform_location + static_cast<GLint>(index) * 2, 1, GL_FALSE, value.data()->data());
	}

	void shader_fmat2::update(std::span<const math::fmat2> values, usize index) const
	{
		glUniformMatrix2fv(m_gl_uniform_location + static_cast<GLint>(index) * 2, static_cast<GLsizei>(values.size()), GL_FALSE, values.data()->data()->data());
	}

	shader_fmat3::shader_fmat3(usize size, GLint gl_uniform_location):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location}
	{
	}

	void shader_fmat3::update(const math::fmat3& value) const noexcept
	{
		glUniformMatrix3fv(m_gl_uniform_location, 1, GL_FALSE, value.data()->data());
	}

	void shader_fmat3::update(const math::fmat3& value, usize index) const
	{
		glUniformMatrix3fv(m_gl_uniform_location + static_cast<GLint>(index) * 3, 1, GL_FALSE, value.data()->data());
	}

	void shader_fmat3::update(std::span<const math::fmat3> values, usize index) const
	{
		glUniformMatrix3fv(m_gl_uniform_location + static_cast<GLint>(index) * 3, static_cast<GLsizei>(values.size()), GL_FALSE, values.data()->data()->data());
	}

	shader_fmat4::shader_fmat4(usize size, GLint gl_uniform_location):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location}
	{
	}

	void shader_fmat4::update(const math::fmat4& value) const noexcept
	{
		glUniformMatrix4fv(m_gl_uniform_location, 1, GL_FALSE, value.data()->data());
	}

	void shader_fmat4::update(const math::fmat4& value, usize index) const
	{
		glUniformMatrix4fv(m_gl_uniform_location + static_cast<GLint>(index) * 4, 1, GL_FALSE, value.data()->data());
	}

	void shader_fmat4::update(std::span<const math::fmat4> values, usize index) const
	{
		glUniformMatrix4fv(m_gl_uniform_location + static_cast<GLint>(index) * 4, static_cast<GLsizei>(values.size()), GL_FALSE, values.data()->data()->data());
	}

	shader_texture_1d::shader_texture_1d(usize size, GLint gl_uniform_location, GLint gl_first_texture_unit_index):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location},
		m_gl_texture_unit_indices(size)
	{
		std::iota(m_gl_texture_unit_indices.begin(), m_gl_texture_unit_indices.end(), gl_first_texture_unit_index);
	}

	void shader_texture_1d::update(const texture_1d& value) const noexcept
	{
		// Bind texture to texture unit
		glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices.front()), value.get_image_view() ? value.get_image_view()->m_gl_texture_name : 0);
		glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices.front()), value.get_sampler() ? value.get_sampler()->m_gl_named_sampler : 0);

		// Pass texture unit index to shader
		glUniform1i(m_gl_uniform_location, m_gl_texture_unit_indices.front());
	}

	void shader_texture_1d::update(const texture_1d& value, usize index) const
	{
		const GLint gl_texture_index = m_gl_texture_unit_indices[index];

		// Bind texture to texture unit
		glBindTextureUnit(static_cast<GLuint>(gl_texture_index), value.get_image_view() ? value.get_image_view()->m_gl_texture_name : 0);
		glBindSampler(static_cast<GLuint>(gl_texture_index), value.get_sampler() ? value.get_sampler()->m_gl_named_sampler : 0);

		// Pass texture unit index to shader
		glUniform1i(m_gl_uniform_location + static_cast<GLint>(index), gl_texture_index);
	}

	void shader_texture_1d::update(std::span<const texture_1d* const> values, usize index) const
	{
		// Bind textures
		for (usize i = 0; i < values.size(); ++i)
		{
			glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_image_view() ? values[i]->get_image_view()->m_gl_texture_name : 0);
			glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_sampler() ? values[i]->get_sampler()->m_gl_named_sampler : 0);
		}

		// Pass texture unit indices to shader
		glUniform1iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), &m_gl_texture_unit_indices[index]);
	}

	void shader_texture_1d::update(std::span<const std::shared_ptr<texture_1d>> values, usize index) const
	{
		// Bind textures
		for (usize i = 0; i < values.size(); ++i)
		{
			glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_image_view() ? values[i]->get_image_view()->m_gl_texture_name : 0);
			glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_sampler() ? values[i]->get_sampler()->m_gl_named_sampler : 0);
		}

		// Pass texture unit indices to shader
		glUniform1iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), &m_gl_texture_unit_indices[index]);
	}

	shader_texture_2d::shader_texture_2d(usize size, GLint gl_uniform_location, GLint gl_first_texture_unit_index):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location},
		m_gl_texture_unit_indices(size)
	{
		std::iota(m_gl_texture_unit_indices.begin(), m_gl_texture_unit_indices.end(), gl_first_texture_unit_index);
	}

	void shader_texture_2d::update(const texture_2d& value) const noexcept
	{
		// Bind texture to texture unit
		glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices.front()), value.get_image_view() ? value.get_image_view()->m_gl_texture_name : 0);
		glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices.front()), value.get_sampler() ? value.get_sampler()->m_gl_named_sampler : 0);

		// Pass texture unit index to shader
		glUniform1i(m_gl_uniform_location, m_gl_texture_unit_indices.front());
	}

	void shader_texture_2d::update(const texture_2d& value, usize index) const
	{
		const GLint gl_texture_index = m_gl_texture_unit_indices[index];

		// Bind texture to texture unit
		glBindTextureUnit(static_cast<GLuint>(gl_texture_index), value.get_image_view() ? value.get_image_view()->m_gl_texture_name : 0);
		glBindSampler(static_cast<GLuint>(gl_texture_index), value.get_sampler() ? value.get_sampler()->m_gl_named_sampler : 0);

		// Pass texture unit index to shader
		glUniform1i(m_gl_uniform_location + static_cast<GLint>(index), gl_texture_index);
	}

	void shader_texture_2d::update(std::span<const texture_2d* const> values, usize index) const
	{
		// Bind textures
		for (usize i = 0; i < values.size(); ++i)
		{
			glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_image_view() ? values[i]->get_image_view()->m_gl_texture_name : 0);
			glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_sampler() ? values[i]->get_sampler()->m_gl_named_sampler : 0);
		}

		// Pass texture unit indices to shader
		glUniform1iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), &m_gl_texture_unit_indices[index]);
	}

	void shader_texture_2d::update(std::span<const std::shared_ptr<texture_2d>> values, usize index) const
	{
		// Bind textures
		for (usize i = 0; i < values.size(); ++i)
		{
			glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_image_view() ? values[i]->get_image_view()->m_gl_texture_name : 0);
			glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_sampler() ? values[i]->get_sampler()->m_gl_named_sampler : 0);
		}

		// Pass texture unit indices to shader
		glUniform1iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), &m_gl_texture_unit_indices[index]);
	}

	shader_texture_3d::shader_texture_3d(usize size, GLint gl_uniform_location, GLint gl_first_texture_unit_index):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location},
		m_gl_texture_unit_indices(size)
	{
		std::iota(m_gl_texture_unit_indices.begin(), m_gl_texture_unit_indices.end(), gl_first_texture_unit_index);
	}

	void shader_texture_3d::update(const texture_3d& value) const noexcept
	{
		// Bind texture to texture unit
		glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices.front()), value.get_image_view() ? value.get_image_view()->m_gl_texture_name : 0);
		glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices.front()), value.get_sampler() ? value.get_sampler()->m_gl_named_sampler : 0);

		// Pass texture unit index to shader
		glUniform1i(m_gl_uniform_location, m_gl_texture_unit_indices.front());
	}

	void shader_texture_3d::update(const texture_3d& value, usize index) const
	{
		const GLint gl_texture_index = m_gl_texture_unit_indices[index];

		// Bind texture to texture unit
		glBindTextureUnit(static_cast<GLuint>(gl_texture_index), value.get_image_view() ? value.get_image_view()->m_gl_texture_name : 0);
		glBindSampler(static_cast<GLuint>(gl_texture_index), value.get_sampler() ? value.get_sampler()->m_gl_named_sampler : 0);

		// Pass texture unit index to shader
		glUniform1i(m_gl_uniform_location + static_cast<GLint>(index), gl_texture_index);
	}

	void shader_texture_3d::update(std::span<const texture_3d* const> values, usize index) const
	{
		// Bind textures
		for (usize i = 0; i < values.size(); ++i)
		{
			glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_image_view() ? values[i]->get_image_view()->m_gl_texture_name : 0);
			glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_sampler() ? values[i]->get_sampler()->m_gl_named_sampler : 0);
		}

		// Pass texture unit indices to shader
		glUniform1iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), &m_gl_texture_unit_indices[index]);
	}

	void shader_texture_3d::update(std::span<const std::shared_ptr<texture_3d>> values, usize index) const
	{
		// Bind textures
		for (usize i = 0; i < values.size(); ++i)
		{
			glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_image_view() ? values[i]->get_image_view()->m_gl_texture_name : 0);
			glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_sampler() ? values[i]->get_sampler()->m_gl_named_sampler : 0);
		}

		// Pass texture unit indices to shader
		glUniform1iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), &m_gl_texture_unit_indices[index]);
	}

	shader_texture_cube::shader_texture_cube(usize size, GLint gl_uniform_location, GLint gl_first_texture_unit_index):
		shader_variable(size),
		m_gl_uniform_location{gl_uniform_location},
		m_gl_texture_unit_indices(size)
	{
		std::iota(m_gl_texture_unit_indices.begin(), m_gl_texture_unit_indices.end(), gl_first_texture_unit_index);
	}

	void shader_texture_cube::update(const texture_cube& value) const noexcept
	{
		// Bind texture to texture unit
		glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices.front()), value.get_image_view() ? value.get_image_view()->m_gl_texture_name : 0);
		glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices.front()), value.get_sampler() ? value.get_sampler()->m_gl_named_sampler : 0);

		// Pass texture unit index to shader
		glUniform1i(m_gl_uniform_location, m_gl_texture_unit_indices.front());
	}

	void shader_texture_cube::update(const texture_cube& value, usize index) const
	{
		const GLint gl_texture_index = m_gl_texture_unit_indices[index];

		// Bind texture to texture unit
		glBindTextureUnit(static_cast<GLuint>(gl_texture_index), value.get_image_view() ? value.get_image_view()->m_gl_texture_name : 0);
		glBindSampler(static_cast<GLuint>(gl_texture_index), value.get_sampler() ? value.get_sampler()->m_gl_named_sampler : 0);

		// Pass texture unit index to shader
		glUniform1i(m_gl_uniform_location + static_cast<GLint>(index), gl_texture_index);
	}

	void shader_texture_cube::update(std::span<const texture_cube* const> values, usize index) const
	{
		// Bind textures
		for (usize i = 0; i < values.size(); ++i)
		{
			glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_image_view() ? values[i]->get_image_view()->m_gl_texture_name : 0);
			glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_sampler() ? values[i]->get_sampler()->m_gl_named_sampler : 0);
		}

		// Pass texture unit indices to shader
		glUniform1iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), &m_gl_texture_unit_indices[index]);
	}

	void shader_texture_cube::update(std::span<const std::shared_ptr<texture_cube>> values, usize index) const
	{
		// Bind textures
		for (usize i = 0; i < values.size(); ++i)
		{
			glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_image_view() ? values[i]->get_image_view()->m_gl_texture_name : 0);
			glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_sampler() ? values[i]->get_sampler()->m_gl_named_sampler : 0);
		}

		// Pass texture unit indices to shader
		glUniform1iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), &m_gl_texture_unit_indices[index]);
	}
}
