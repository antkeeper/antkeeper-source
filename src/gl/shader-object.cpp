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

#include "gl/shader-object.hpp"
#include <glad/glad.h>
#include <stdexcept>

namespace gl {

static constexpr GLenum gl_shader_type_lut[] =
{
	GL_VERTEX_SHADER,
	GL_FRAGMENT_SHADER,
	GL_GEOMETRY_SHADER
};

shader_object::shader_object(shader_stage stage):
	gl_shader_id(0),
	stage(stage),
	compiled(false)
{
	// Look up OpenGL shader type enumeration that corresponds to the given stage 
	GLenum gl_shader_type = gl_shader_type_lut[static_cast<std::size_t>(stage)];
	
	// Create an OpenGL shader object
	gl_shader_id = glCreateShader(gl_shader_type);
	
	// Handle OpenGL errors
	if (!gl_shader_id)
	{
		throw std::runtime_error("An error occurred while creating an OpenGL shader object.");
	}
}

shader_object::~shader_object()
{
	// Flag the OpenGL shader object for deletion
	glDeleteShader(gl_shader_id);
}

void shader_object::source(const char* buffer, std::size_t size)
{
	// Replace OpenGL shader object source code
	GLint gl_length = size;
	glShaderSource(gl_shader_id, 1, &buffer, &gl_length);
	
	// Handle OpenGL errors
	switch (glGetError())
	{
		case GL_INVALID_VALUE:
			throw std::runtime_error("OpenGL shader object handle is not a value generated by OpenGL.");
			break;
		
		case GL_INVALID_OPERATION:
			throw std::runtime_error("OpenGL shader object handle is not a shader object.");
			break;
	}
}

bool shader_object::compile()
{
	// Compile OpenGL shader object
	glCompileShader(gl_shader_id);
	
	// Handle OpenGL errors
	switch (glGetError())
	{
		case GL_INVALID_VALUE:
			throw std::runtime_error("OpenGL shader object handle is not a value generated by OpenGL.");
			break;
		
		case GL_INVALID_OPERATION:
			throw std::runtime_error("OpenGL shader object handle is not a shader object.");
			break;
	}
	
	// Get OpenGL shader object compilation status
	GLint gl_compile_status;
	glGetShaderiv(gl_shader_id, GL_COMPILE_STATUS, &gl_compile_status);
	compiled = (gl_compile_status == GL_TRUE);
	
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
	else
	{
		// Empty info log
		info_log.clear();
	}
	
	// Return compilation status
	return compiled;
}

} // namespace gl
