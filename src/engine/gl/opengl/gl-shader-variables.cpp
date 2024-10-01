// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/gl/opengl/gl-shader-variables.hpp>
#include <engine/gl/texture.hpp>
#include <numeric>

namespace gl {

gl_shader_bool::gl_shader_bool(std::size_t size, GLint gl_uniform_location):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location},
	m_ivalues(size)
{}

void gl_shader_bool::update(bool value) const noexcept
{
	glUniform1i(m_gl_uniform_location, static_cast<GLint>(value));
}

void gl_shader_bool::update(bool value, std::size_t index) const
{
	glUniform1i(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLint>(value));
}

void gl_shader_bool::update(std::span<const bool> values, std::size_t index) const
{
	for (std::size_t i = 0; i < values.size(); ++i)
	{
		m_ivalues[i] = static_cast<GLint>(values[i]);
	}
	
	glUniform1iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), m_ivalues.data());
}

gl_shader_bvec2::gl_shader_bvec2(std::size_t size, GLint gl_uniform_location):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location},
	m_ivalues(size * 2)
{}

void gl_shader_bvec2::update(const math::bvec2& value) const noexcept
{
	glUniform2i(m_gl_uniform_location, static_cast<GLint>(value[0]), static_cast<GLint>(value[1]));
}

void gl_shader_bvec2::update(const math::bvec2& value, std::size_t index) const
{
	glUniform2i(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLint>(value[0]), static_cast<GLint>(value[1]));
}

void gl_shader_bvec2::update(std::span<const math::bvec2> values, std::size_t index) const
{
	GLint* ivalue = m_ivalues.data();
	for (const auto& value: values)
	{
		*(++ivalue) = static_cast<GLint>(value[0]);
		*(++ivalue) = static_cast<GLint>(value[1]);
	}
	
	glUniform2iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), m_ivalues.data());
}

gl_shader_bvec3::gl_shader_bvec3(std::size_t size, GLint gl_uniform_location):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location},
	m_ivalues(size * 3)
{}

void gl_shader_bvec3::update(const math::bvec3& value) const noexcept
{
	glUniform3i(m_gl_uniform_location, static_cast<GLint>(value[0]), static_cast<GLint>(value[1]), static_cast<GLint>(value[2]));
}

void gl_shader_bvec3::update(const math::bvec3& value, std::size_t index) const
{
	glUniform3i(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLint>(value[0]), static_cast<GLint>(value[1]), static_cast<GLint>(value[2]));
}

void gl_shader_bvec3::update(std::span<const math::bvec3> values, std::size_t index) const
{
	GLint* ivalue = m_ivalues.data();
	for (const auto& value: values)
	{
		*(++ivalue) = static_cast<GLint>(value[0]);
		*(++ivalue) = static_cast<GLint>(value[1]);
		*(++ivalue) = static_cast<GLint>(value[2]);
	}
	
	glUniform3iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), m_ivalues.data());
}

gl_shader_bvec4::gl_shader_bvec4(std::size_t size, GLint gl_uniform_location):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location},
	m_ivalues(size * 4)
{}

void gl_shader_bvec4::update(const math::bvec4& value) const noexcept
{
	glUniform4i(m_gl_uniform_location, static_cast<GLint>(value[0]), static_cast<GLint>(value[1]), static_cast<GLint>(value[2]), static_cast<GLint>(value[3]));
}

void gl_shader_bvec4::update(const math::bvec4& value, std::size_t index) const
{
	glUniform4i(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLint>(value[0]), static_cast<GLint>(value[1]), static_cast<GLint>(value[2]), static_cast<GLint>(value[3]));
}

void gl_shader_bvec4::update(std::span<const math::bvec4> values, std::size_t index) const
{
	GLint* ivalue = m_ivalues.data();
	for (const auto& value: values)
	{
		*(++ivalue) = static_cast<GLint>(value[0]);
		*(++ivalue) = static_cast<GLint>(value[1]);
		*(++ivalue) = static_cast<GLint>(value[2]);
		*(++ivalue) = static_cast<GLint>(value[3]);
	}
	
	glUniform4iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), m_ivalues.data());
}

gl_shader_int::gl_shader_int(std::size_t size, GLint gl_uniform_location):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location}
{}

void gl_shader_int::update(int value) const noexcept
{
	glUniform1i(m_gl_uniform_location, value);
}

void gl_shader_int::update(int value, std::size_t index) const
{
	glUniform1i(m_gl_uniform_location + static_cast<GLint>(index), value);
}

void gl_shader_int::update(std::span<const int> values, std::size_t index) const
{
	glUniform1iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data());
}

gl_shader_ivec2::gl_shader_ivec2(std::size_t size, GLint gl_uniform_location):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location}
{}

void gl_shader_ivec2::update(const math::ivec2& value) const noexcept
{
	glUniform2iv(m_gl_uniform_location, 1, value.data());
}

void gl_shader_ivec2::update(const math::ivec2& value, std::size_t index) const
{
	glUniform2iv(m_gl_uniform_location + static_cast<GLint>(index), 1, value.data());
}

void gl_shader_ivec2::update(std::span<const math::ivec2> values, std::size_t index) const
{
	glUniform2iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data()->data());
}

gl_shader_ivec3::gl_shader_ivec3(std::size_t size, GLint gl_uniform_location):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location}
{}

void gl_shader_ivec3::update(const math::ivec3& value) const noexcept
{
	glUniform3iv(m_gl_uniform_location, 1, value.data());
}

void gl_shader_ivec3::update(const math::ivec3& value, std::size_t index) const
{
	glUniform3iv(m_gl_uniform_location + static_cast<GLint>(index), 1, value.data());
}

void gl_shader_ivec3::update(std::span<const math::ivec3> values, std::size_t index) const
{
	glUniform3iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data()->data());
}

gl_shader_ivec4::gl_shader_ivec4(std::size_t size, GLint gl_uniform_location):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location}
{}

void gl_shader_ivec4::update(const math::ivec4& value) const noexcept
{
	glUniform4iv(m_gl_uniform_location, 1, value.data());
}

void gl_shader_ivec4::update(const math::ivec4& value, std::size_t index) const
{
	glUniform4iv(m_gl_uniform_location + static_cast<GLint>(index), 1, value.data());
}

void gl_shader_ivec4::update(std::span<const math::ivec4> values, std::size_t index) const
{
	glUniform4iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data()->data());
}

gl_shader_uint::gl_shader_uint(std::size_t size, GLint gl_uniform_location):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location}
{}

void gl_shader_uint::update(unsigned int value) const noexcept
{
	glUniform1ui(m_gl_uniform_location, value);
}

void gl_shader_uint::update(unsigned int value, std::size_t index) const
{
	glUniform1ui(m_gl_uniform_location + static_cast<GLint>(index), value);
}

void gl_shader_uint::update(std::span<const unsigned int> values, std::size_t index) const
{
	glUniform1uiv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data());
}

gl_shader_uvec2::gl_shader_uvec2(std::size_t size, GLint gl_uniform_location):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location}
{}

void gl_shader_uvec2::update(const math::uvec2& value) const noexcept
{
	glUniform2uiv(m_gl_uniform_location, 1, value.data());
}

void gl_shader_uvec2::update(const math::uvec2& value, std::size_t index) const
{
	glUniform2uiv(m_gl_uniform_location + static_cast<GLint>(index), 1, value.data());
}

void gl_shader_uvec2::update(std::span<const math::uvec2> values, std::size_t index) const
{
	glUniform2uiv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data()->data());
}

gl_shader_uvec3::gl_shader_uvec3(std::size_t size, GLint gl_uniform_location):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location}
{}

void gl_shader_uvec3::update(const math::uvec3& value) const noexcept
{
	glUniform3uiv(m_gl_uniform_location, 1, value.data());
}

void gl_shader_uvec3::update(const math::uvec3& value, std::size_t index) const
{
	glUniform3uiv(m_gl_uniform_location + static_cast<GLint>(index), 1, value.data());
}

void gl_shader_uvec3::update(std::span<const math::uvec3> values, std::size_t index) const
{
	glUniform3uiv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data()->data());
}

gl_shader_uvec4::gl_shader_uvec4(std::size_t size, GLint gl_uniform_location):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location}
{}

void gl_shader_uvec4::update(const math::uvec4& value) const noexcept
{
	glUniform4uiv(m_gl_uniform_location, 1, value.data());
}

void gl_shader_uvec4::update(const math::uvec4& value, std::size_t index) const
{
	glUniform4uiv(m_gl_uniform_location + static_cast<GLint>(index), 1, value.data());
}

void gl_shader_uvec4::update(std::span<const math::uvec4> values, std::size_t index) const
{
	glUniform4uiv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data()->data());
}

gl_shader_float::gl_shader_float(std::size_t size, GLint gl_uniform_location):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location}
{}

void gl_shader_float::update(float value) const noexcept
{
	glUniform1f(m_gl_uniform_location, value);
}

void gl_shader_float::update(float value, std::size_t index) const
{
	glUniform1f(m_gl_uniform_location + static_cast<GLint>(index), value);	
}

void gl_shader_float::update(std::span<const float> values, std::size_t index) const
{
	glUniform1fv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data());
}

gl_shader_fvec2::gl_shader_fvec2(std::size_t size, GLint gl_uniform_location):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location}
{}

void gl_shader_fvec2::update(const math::fvec2& value) const noexcept
{
	glUniform2fv(m_gl_uniform_location, 1, value.data());
}

void gl_shader_fvec2::update(const math::fvec2& value, std::size_t index) const
{
	glUniform2fv(m_gl_uniform_location + static_cast<GLint>(index), 1, value.data());
}

void gl_shader_fvec2::update(std::span<const math::fvec2> values, std::size_t index) const
{
	glUniform2fv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data()->data());
}

gl_shader_fvec3::gl_shader_fvec3(std::size_t size, GLint gl_uniform_location):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location}
{}

void gl_shader_fvec3::update(const math::fvec3& value) const noexcept
{
	glUniform3fv(m_gl_uniform_location, 1, value.data());
}

void gl_shader_fvec3::update(const math::fvec3& value, std::size_t index) const
{
	glUniform3fv(m_gl_uniform_location + static_cast<GLint>(index), 1, value.data());
}

void gl_shader_fvec3::update(std::span<const math::fvec3> values, std::size_t index) const
{
	glUniform3fv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data()->data());
}

gl_shader_fvec4::gl_shader_fvec4(std::size_t size, GLint gl_uniform_location):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location}
{}

void gl_shader_fvec4::update(const math::fvec4& value) const noexcept
{
	glUniform4fv(m_gl_uniform_location, 1, value.data());
}

void gl_shader_fvec4::update(const math::fvec4& value, std::size_t index) const
{
	glUniform4fv(m_gl_uniform_location + static_cast<GLint>(index), 1, value.data());
}

void gl_shader_fvec4::update(std::span<const math::fvec4> values, std::size_t index) const
{
	glUniform4fv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), values.data()->data());
}

gl_shader_fmat2::gl_shader_fmat2(std::size_t size, GLint gl_uniform_location):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location}
{}

void gl_shader_fmat2::update(const math::fmat2& value) const noexcept
{
	glUniformMatrix2fv(m_gl_uniform_location, 1, GL_FALSE, value.data()->data());
}

void gl_shader_fmat2::update(const math::fmat2& value, std::size_t index) const
{
	glUniformMatrix2fv(m_gl_uniform_location + static_cast<GLint>(index) * 2, 1, GL_FALSE, value.data()->data());
}

void gl_shader_fmat2::update(std::span<const math::fmat2> values, std::size_t index) const
{
	glUniformMatrix2fv(m_gl_uniform_location + static_cast<GLint>(index) * 2, static_cast<GLsizei>(values.size()), GL_FALSE, values.data()->data()->data());
}

gl_shader_fmat3::gl_shader_fmat3(std::size_t size, GLint gl_uniform_location):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location}
{}

void gl_shader_fmat3::update(const math::fmat3& value) const noexcept
{
	glUniformMatrix3fv(m_gl_uniform_location, 1, GL_FALSE, value.data()->data());
}

void gl_shader_fmat3::update(const math::fmat3& value, std::size_t index) const
{
	glUniformMatrix3fv(m_gl_uniform_location + static_cast<GLint>(index) * 3, 1, GL_FALSE, value.data()->data());
}

void gl_shader_fmat3::update(std::span<const math::fmat3> values, std::size_t index) const
{
	glUniformMatrix3fv(m_gl_uniform_location + static_cast<GLint>(index) * 3, static_cast<GLsizei>(values.size()), GL_FALSE, values.data()->data()->data());
}

gl_shader_fmat4::gl_shader_fmat4(std::size_t size, GLint gl_uniform_location):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location}
{}

void gl_shader_fmat4::update(const math::fmat4& value) const noexcept
{
	glUniformMatrix4fv(m_gl_uniform_location, 1, GL_FALSE, value.data()->data());
}

void gl_shader_fmat4::update(const math::fmat4& value, std::size_t index) const
{
	glUniformMatrix4fv(m_gl_uniform_location + static_cast<GLint>(index) * 4, 1, GL_FALSE, value.data()->data());
}

void gl_shader_fmat4::update(std::span<const math::fmat4> values, std::size_t index) const
{
	glUniformMatrix4fv(m_gl_uniform_location + static_cast<GLint>(index) * 4, static_cast<GLsizei>(values.size()), GL_FALSE, values.data()->data()->data());
}

gl_shader_texture_1d::gl_shader_texture_1d(std::size_t size, GLint gl_uniform_location, GLint gl_first_texture_unit_index):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location},
	m_gl_texture_unit_indices(size)
{
	std::iota(m_gl_texture_unit_indices.begin(), m_gl_texture_unit_indices.end(), gl_first_texture_unit_index);
}

void gl_shader_texture_1d::update(const texture_1d& value) const noexcept
{
	// Bind texture to texture unit
	glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices.front()), value.get_image_view() ? value.get_image_view()->m_gl_texture_name : 0);
	glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices.front()), value.get_sampler() ? value.get_sampler()->m_gl_named_sampler : 0);
	
	// Pass texture unit index to shader
	glUniform1i(m_gl_uniform_location, m_gl_texture_unit_indices.front());
}

void gl_shader_texture_1d::update(const texture_1d& value, std::size_t index) const
{
	const GLint gl_texture_index = m_gl_texture_unit_indices[index];
	
	// Bind texture to texture unit
	glBindTextureUnit(static_cast<GLuint>(gl_texture_index), value.get_image_view() ? value.get_image_view()->m_gl_texture_name : 0);
	glBindSampler(static_cast<GLuint>(gl_texture_index), value.get_sampler() ? value.get_sampler()->m_gl_named_sampler : 0);
	
	// Pass texture unit index to shader
	glUniform1i(m_gl_uniform_location + static_cast<GLint>(index), gl_texture_index);
}

void gl_shader_texture_1d::update(std::span<const texture_1d* const> values, std::size_t index) const
{
	// Bind textures
	for (std::size_t i = 0; i < values.size(); ++i)
	{
		glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_image_view() ? values[i]->get_image_view()->m_gl_texture_name : 0);
		glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_sampler() ? values[i]->get_sampler()->m_gl_named_sampler : 0);
	}
	
	// Pass texture unit indices to shader
	glUniform1iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), &m_gl_texture_unit_indices[index]);
}

void gl_shader_texture_1d::update(std::span<const std::shared_ptr<texture_1d>> values, std::size_t index) const
{
	// Bind textures
	for (std::size_t i = 0; i < values.size(); ++i)
	{
		glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_image_view() ? values[i]->get_image_view()->m_gl_texture_name : 0);
		glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_sampler() ? values[i]->get_sampler()->m_gl_named_sampler : 0);
	}
	
	// Pass texture unit indices to shader
	glUniform1iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), &m_gl_texture_unit_indices[index]);
}

gl_shader_texture_2d::gl_shader_texture_2d(std::size_t size, GLint gl_uniform_location, GLint gl_first_texture_unit_index):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location},
	m_gl_texture_unit_indices(size)
{
	std::iota(m_gl_texture_unit_indices.begin(), m_gl_texture_unit_indices.end(), gl_first_texture_unit_index);
}

void gl_shader_texture_2d::update(const texture_2d& value) const noexcept
{
	// Bind texture to texture unit
	glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices.front()), value.get_image_view() ? value.get_image_view()->m_gl_texture_name : 0);
	glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices.front()), value.get_sampler() ? value.get_sampler()->m_gl_named_sampler : 0);
	
	// Pass texture unit index to shader
	glUniform1i(m_gl_uniform_location, m_gl_texture_unit_indices.front());
}

void gl_shader_texture_2d::update(const texture_2d& value, std::size_t index) const
{
	const GLint gl_texture_index = m_gl_texture_unit_indices[index];
	
	// Bind texture to texture unit
	glBindTextureUnit(static_cast<GLuint>(gl_texture_index), value.get_image_view() ? value.get_image_view()->m_gl_texture_name : 0);
	glBindSampler(static_cast<GLuint>(gl_texture_index), value.get_sampler() ? value.get_sampler()->m_gl_named_sampler : 0);
	
	// Pass texture unit index to shader
	glUniform1i(m_gl_uniform_location + static_cast<GLint>(index), gl_texture_index);
}

void gl_shader_texture_2d::update(std::span<const texture_2d* const> values, std::size_t index) const
{
	// Bind textures
	for (std::size_t i = 0; i < values.size(); ++i)
	{
		glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_image_view() ? values[i]->get_image_view()->m_gl_texture_name : 0);
		glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_sampler() ? values[i]->get_sampler()->m_gl_named_sampler : 0);
	}
	
	// Pass texture unit indices to shader
	glUniform1iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), &m_gl_texture_unit_indices[index]);
}

void gl_shader_texture_2d::update(std::span<const std::shared_ptr<texture_2d>> values, std::size_t index) const
{
	// Bind textures
	for (std::size_t i = 0; i < values.size(); ++i)
	{
		glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_image_view() ? values[i]->get_image_view()->m_gl_texture_name : 0);
		glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_sampler() ? values[i]->get_sampler()->m_gl_named_sampler : 0);
	}
	
	// Pass texture unit indices to shader
	glUniform1iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), &m_gl_texture_unit_indices[index]);
}

gl_shader_texture_3d::gl_shader_texture_3d(std::size_t size, GLint gl_uniform_location, GLint gl_first_texture_unit_index):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location},
	m_gl_texture_unit_indices(size)
{
	std::iota(m_gl_texture_unit_indices.begin(), m_gl_texture_unit_indices.end(), gl_first_texture_unit_index);
}

void gl_shader_texture_3d::update(const texture_3d& value) const noexcept
{
	// Bind texture to texture unit
	glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices.front()), value.get_image_view() ? value.get_image_view()->m_gl_texture_name : 0);
	glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices.front()), value.get_sampler() ? value.get_sampler()->m_gl_named_sampler : 0);
	
	// Pass texture unit index to shader
	glUniform1i(m_gl_uniform_location, m_gl_texture_unit_indices.front());
}

void gl_shader_texture_3d::update(const texture_3d& value, std::size_t index) const
{
	const GLint gl_texture_index = m_gl_texture_unit_indices[index];
	
	// Bind texture to texture unit
	glBindTextureUnit(static_cast<GLuint>(gl_texture_index), value.get_image_view() ? value.get_image_view()->m_gl_texture_name : 0);
	glBindSampler(static_cast<GLuint>(gl_texture_index), value.get_sampler() ? value.get_sampler()->m_gl_named_sampler : 0);
	
	// Pass texture unit index to shader
	glUniform1i(m_gl_uniform_location + static_cast<GLint>(index), gl_texture_index);
}

void gl_shader_texture_3d::update(std::span<const texture_3d* const> values, std::size_t index) const
{
	// Bind textures
	for (std::size_t i = 0; i < values.size(); ++i)
	{
		glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_image_view() ? values[i]->get_image_view()->m_gl_texture_name : 0);
		glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_sampler() ? values[i]->get_sampler()->m_gl_named_sampler : 0);
	}
	
	// Pass texture unit indices to shader
	glUniform1iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), &m_gl_texture_unit_indices[index]);
}

void gl_shader_texture_3d::update(std::span<const std::shared_ptr<texture_3d>> values, std::size_t index) const
{
	// Bind textures
	for (std::size_t i = 0; i < values.size(); ++i)
	{
		glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_image_view() ? values[i]->get_image_view()->m_gl_texture_name : 0);
		glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_sampler() ? values[i]->get_sampler()->m_gl_named_sampler : 0);
	}
	
	// Pass texture unit indices to shader
	glUniform1iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), &m_gl_texture_unit_indices[index]);
}

gl_shader_texture_cube::gl_shader_texture_cube(std::size_t size, GLint gl_uniform_location, GLint gl_first_texture_unit_index):
	shader_variable(size),
	m_gl_uniform_location{gl_uniform_location},
	m_gl_texture_unit_indices(size)
{
	std::iota(m_gl_texture_unit_indices.begin(), m_gl_texture_unit_indices.end(), gl_first_texture_unit_index);
}

void gl_shader_texture_cube::update(const texture_cube& value) const noexcept
{
	// Bind texture to texture unit
	glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices.front()), value.get_image_view() ? value.get_image_view()->m_gl_texture_name : 0);
	glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices.front()), value.get_sampler() ? value.get_sampler()->m_gl_named_sampler : 0);
	
	// Pass texture unit index to shader
	glUniform1i(m_gl_uniform_location, m_gl_texture_unit_indices.front());
}

void gl_shader_texture_cube::update(const texture_cube& value, std::size_t index) const
{
	const GLint gl_texture_index = m_gl_texture_unit_indices[index];
	
	// Bind texture to texture unit
	glBindTextureUnit(static_cast<GLuint>(gl_texture_index), value.get_image_view() ? value.get_image_view()->m_gl_texture_name : 0);
	glBindSampler(static_cast<GLuint>(gl_texture_index), value.get_sampler() ? value.get_sampler()->m_gl_named_sampler : 0);
	
	// Pass texture unit index to shader
	glUniform1i(m_gl_uniform_location + static_cast<GLint>(index), gl_texture_index);
}

void gl_shader_texture_cube::update(std::span<const texture_cube* const> values, std::size_t index) const
{
	// Bind textures
	for (std::size_t i = 0; i < values.size(); ++i)
	{
		glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_image_view() ? values[i]->get_image_view()->m_gl_texture_name : 0);
		glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_sampler() ? values[i]->get_sampler()->m_gl_named_sampler : 0);
	}
	
	// Pass texture unit indices to shader
	glUniform1iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), &m_gl_texture_unit_indices[index]);
}

void gl_shader_texture_cube::update(std::span<const std::shared_ptr<texture_cube>> values, std::size_t index) const
{
	// Bind textures
	for (std::size_t i = 0; i < values.size(); ++i)
	{
		glBindTextureUnit(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_image_view() ? values[i]->get_image_view()->m_gl_texture_name : 0);
		glBindSampler(static_cast<GLuint>(m_gl_texture_unit_indices[index + i]), values[i]->get_sampler() ? values[i]->get_sampler()->m_gl_named_sampler : 0);
	}
	
	// Pass texture unit indices to shader
	glUniform1iv(m_gl_uniform_location + static_cast<GLint>(index), static_cast<GLsizei>(values.size()), &m_gl_texture_unit_indices[index]);
}

} // namespace gl
