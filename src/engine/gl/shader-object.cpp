// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/gl/shader-object.hpp>
#include <glad/gl.h>
#include <stdexcept>

namespace gl {

static constexpr GLenum gl_shader_type_lut[] =
{
	GL_VERTEX_SHADER,
	GL_FRAGMENT_SHADER,
	GL_GEOMETRY_SHADER
};

shader_object::shader_object(shader_stage stage):
	m_stage{stage}
{
	// Look up OpenGL shader type enumeration that corresponds to the given stage 
	const GLenum gl_shader_type = gl_shader_type_lut[static_cast<std::size_t>(m_stage)];
	
	// Create an OpenGL shader object
	gl_shader_id = glCreateShader(gl_shader_type);
	if (!gl_shader_id)
	{
		throw std::runtime_error("Unable to create OpenGL shader object");
	}
}

shader_object::~shader_object()
{
	glDeleteShader(gl_shader_id);
}

void shader_object::source(std::string_view source_code)
{
	// Replace OpenGL shader object source code
	const GLint gl_length = static_cast<GLint>(source_code.length());
	const GLchar* gl_string = source_code.data();
	glShaderSource(gl_shader_id, 1, &gl_string, &gl_length);
}

bool shader_object::compile()
{
	m_compiled = false;
	info_log.clear();
	
	// Compile OpenGL shader object
	glCompileShader(gl_shader_id);
	
	// Get OpenGL shader object compilation status
	GLint gl_compile_status;
	glGetShaderiv(gl_shader_id, GL_COMPILE_STATUS, &gl_compile_status);
	m_compiled = (gl_compile_status == GL_TRUE);
	
	// Get OpenGL shader object info log length
	GLint gl_info_log_length;
	glGetShaderiv(gl_shader_id, GL_INFO_LOG_LENGTH, &gl_info_log_length);
	
	if (gl_info_log_length > 0)
	{
		// Resize string to accommodate OpenGL shader object info log
		info_log.resize(gl_info_log_length);
		
		// Read OpenGL shader object info log into string
		glGetShaderInfoLog(gl_shader_id, gl_info_log_length, &gl_info_log_length, info_log.data());
		
		// Remove redundant null terminator from string
		info_log.pop_back();
	}
	
	// Return compilation status
	return m_compiled;
}

} // namespace gl
