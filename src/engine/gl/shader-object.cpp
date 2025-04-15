// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <glad/gl.h>
import engine.gl.shader_object;
import engine.debug.log;
import engine.utility.sized_types;
import <stdexcept>;

namespace engine::gl::opengl
{
	constexpr GLenum shader_type_lut[] =
	{
		GL_VERTEX_SHADER,
		GL_FRAGMENT_SHADER,
		GL_GEOMETRY_SHADER
	};
}

namespace engine::gl
{
	shader_object::shader_object(shader_stage stage):
		m_stage{stage}
	{
		// Look up OpenGL shader type enumeration that corresponds to the given stage 
		const GLenum gl_shader_type = opengl::shader_type_lut[static_cast<usize>(m_stage)];

		// Create an OpenGL shader object
		m_gl_shader_id = glCreateShader(gl_shader_type);
		if (!m_gl_shader_id)
		{
			throw std::runtime_error("Failed to create OpenGL shader object");
		}
	}

	shader_object::~shader_object()
	{
		glDeleteShader(m_gl_shader_id);
	}

	void shader_object::source(std::string_view source_code)
	{
		// Replace OpenGL shader object source code
		const GLint gl_length = static_cast<GLint>(source_code.length());
		const GLchar* gl_string = source_code.data();
		glShaderSource(m_gl_shader_id, 1, &gl_string, &gl_length);
	}

	bool shader_object::compile()
	{
		m_compiled = false;
		m_info_log.clear();

		// Compile OpenGL shader object
		glCompileShader(m_gl_shader_id);

		// Get OpenGL shader object compilation status
		GLint gl_compile_status;
		glGetShaderiv(m_gl_shader_id, GL_COMPILE_STATUS, &gl_compile_status);
		m_compiled = (gl_compile_status == GL_TRUE);

		// Get OpenGL shader object info log length
		GLint gl_info_log_length;
		glGetShaderiv(m_gl_shader_id, GL_INFO_LOG_LENGTH, &gl_info_log_length);

		if (gl_info_log_length > 0)
		{
			// Resize string to accommodate OpenGL shader object info log
			m_info_log.resize(gl_info_log_length);

			// Read OpenGL shader object info log into string
			glGetShaderInfoLog(m_gl_shader_id, gl_info_log_length, &gl_info_log_length, m_info_log.data());

			// Remove redundant null terminator from string
			m_info_log.pop_back();
		}

		if (m_compiled)
		{
			if (!m_info_log.empty())
			{
				log_warning("Compiled shader object {} with warnings: {}", m_gl_shader_id, m_info_log);
			}
		}
		else
		{
			log_error("Failed to compile shader object {}: {}", m_gl_shader_id, m_info_log.empty() ? "Unknown error" : m_info_log);
		}

		// Return compilation status
		return m_compiled;
	}
}
