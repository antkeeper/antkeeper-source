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

#include "gl/shader.hpp"
#include <glad/glad.h>
#include <stdexcept>

namespace gl {

static constexpr GLenum shader_type_lut[] =
{
	GL_VERTEX_SHADER,
	GL_FRAGMENT_SHADER,
	GL_GEOMETRY_SHADER
};

shader::shader(shader_type type, const std::string& source):
	gl_shader_id(0),
	type(type)
{
	GLenum gl_shader_type = shader_type_lut[static_cast<std::size_t>(type)];
	const char* source_c_str = source.c_str();

	gl_shader_id = glCreateShader(gl_shader_type);
	glShaderSource(gl_shader_id, 1, &source_c_str, nullptr);
	glCompileShader(gl_shader_id);

	GLint status;
	glGetShaderiv(gl_shader_id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		throw std::runtime_error(get_info_log().c_str());
	}
}

shader::~shader()
{
	glDeleteShader(gl_shader_id);
}

std::string shader::get_info_log() const
{
	GLint length;
	glGetShaderiv(gl_shader_id, GL_INFO_LOG_LENGTH, &length);

	if (length > 0)
	{
		std::string log(length, '\0');
		glGetShaderInfoLog(gl_shader_id, length, &length, &log[0]);
		return log;
	}

	return std::string();
}

} // namespace gl
