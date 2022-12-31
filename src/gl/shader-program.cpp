/*
 * Copyright (C) 2023  Christopher J. Howard
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

#include "gl/shader-program.hpp"
#include "gl/shader-object.hpp"
#include "gl/shader-variable-type.hpp"
#include "gl/shader-input.hpp"
#include <glad/glad.h>
#include <stdexcept>

namespace gl {

shader_program::shader_program():
	gl_program_id(0),
	linked(false)
{
	// Create an OpenGL shader program
	gl_program_id = glCreateProgram();
	
	// Handle OpenGL errors
	if (!gl_program_id)
	{
		throw std::runtime_error("An error occurred while creating an OpenGL shader program.");
	}
}

shader_program::~shader_program()
{
	// Delete shader inputs
	free_inputs();
	
	// Detach all shader objects
	detach_all();
	
	// Delete the OpenGL shader program
	glDeleteProgram(gl_program_id);
}

void shader_program::attach(const shader_object* object)
{
	if (attached_objects.find(object) != attached_objects.end())
	{
		throw std::runtime_error("Shader object is already attached to the shader program.");
	}
	else
	{
		// Check that both the OpenGL shader program and OpenGL shader object are valid
		if (glIsProgram(gl_program_id) != GL_TRUE)
			throw std::runtime_error("OpenGL shader program is not a valid program object.");
		if (glIsShader(object->gl_shader_id) != GL_TRUE)
			throw std::runtime_error("OpenGL shader object is not a valid shader object.");
		
		// Attach the OpenGL shader object to the OpenGL shader program
		glAttachShader(gl_program_id, object->gl_shader_id);
		
		// Handle OpenGL errors
		switch (glGetError())
		{
			case GL_INVALID_OPERATION:
				throw std::runtime_error("OpenGL shader object is already attached to the shader program.");
				break;
		}
		
		// Add shader object to set of attached objects
		attached_objects.insert(object);
	}
}

void shader_program::detach(const shader_object* object)
{
	if (attached_objects.find(object) == attached_objects.end())
	{
		throw std::runtime_error("Shader object is not attached to the shader program.");
	}
	else
	{
		// Check that both the OpenGL shader program and OpenGL shader object are valid
		if (glIsProgram(gl_program_id) != GL_TRUE)
			throw std::runtime_error("OpenGL shader program is not a valid program object.");
		if (glIsShader(object->gl_shader_id) != GL_TRUE)
			throw std::runtime_error("OpenGL shader object is not a valid shader object.");
		
		// Detach the OpenGL shader object from the OpenGL shader program
		glDetachShader(gl_program_id, object->gl_shader_id);
		
		// Handle OpenGL errors
		switch (glGetError())
		{
			case GL_INVALID_OPERATION:
				throw std::runtime_error("OpenGL shader object is not attached to the shader program.");
				break;
		}
		
		// Remove shader object from set of attached objects
		attached_objects.erase(object);
	}
}

void shader_program::detach_all()
{
	while (!attached_objects.empty())
		detach(*attached_objects.begin());
}

bool shader_program::link()
{
	// Check that the OpenGL shader program is valid
	if (glIsProgram(gl_program_id) != GL_TRUE)
		throw std::runtime_error("OpenGL shader program is not a valid program object.");
	
	// Link OpenGL shader program
	glLinkProgram(gl_program_id);
	
	// Handle OpenGL errors
	switch (glGetError())
	{
		case GL_INVALID_OPERATION:
			throw std::runtime_error("OpenGL shader program is the currently active program object and transform feedback mode is active.");
			break;
	}
	
	// Get OpenGL shader program linking status
	GLint gl_link_status;
	glGetProgramiv(gl_program_id, GL_LINK_STATUS, &gl_link_status);
	linked = (gl_link_status == GL_TRUE);
	
	// Get OpenGL shader program info log length
	GLint gl_info_log_length;
	glGetProgramiv(gl_program_id, GL_INFO_LOG_LENGTH, &gl_info_log_length);
	
	if (gl_info_log_length > 0)
	{
		// Resize string to accommodate OpenGL shader program info log
		info_log.resize(gl_info_log_length);
		
		// Read OpenGL shader program info log into string
		glGetProgramInfoLog(gl_program_id, gl_info_log_length, &gl_info_log_length, info_log.data());
		
		// Remove redundant null terminator from string
		info_log.pop_back();
	}
	else
	{
		// Empty info log
		info_log.clear();
	}
	
	// Find shader inputs
	find_inputs();
	
	return linked;
}

void shader_program::find_inputs()
{
	// Get maximum uniform name length
	GLint max_uniform_name_length = 0;
	glGetProgramiv(gl_program_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_uniform_name_length);
	
	// Allocate uniform name buffer
	GLchar* uniform_name = new GLchar[max_uniform_name_length];
	
	// Get number of active uniforms in the shader
	GLint active_uniform_count = 0;
	glGetProgramiv(gl_program_id, GL_ACTIVE_UNIFORMS, &active_uniform_count);
	
	// Set first available texture unit
	int available_texture_unit = 0;
	
	// For each active uniform
	for (GLuint uniform_index = 0; uniform_index < static_cast<GLuint>(active_uniform_count); ++uniform_index)
	{
		// Get information about uniform
		GLsizei uniform_name_length;
		GLint uniform_size;
		GLenum uniform_type;
		glGetActiveUniform(gl_program_id, uniform_index, static_cast<GLsizei>(max_uniform_name_length), &uniform_name_length, &uniform_size, &uniform_type, &uniform_name[0]);
		
		// Get name without array symbols
		std::string input_name = uniform_name;
		std::size_t bracketPos = input_name.find_first_of("[");
		if (bracketPos != std::string::npos)
		{
			input_name = input_name.substr(0, bracketPos);
		}
		
		// Determine corresponding shader variable data type
		shader_variable_type variable_type;
		int texture_unit = -1;
		bool unsupported = false;
		switch (uniform_type)
		{
			case GL_BOOL:
				variable_type = shader_variable_type::bool1;
				break;
			case GL_BOOL_VEC2:
				variable_type = shader_variable_type::bool2;
				break;
			case GL_BOOL_VEC3:
				variable_type = shader_variable_type::bool3;
				break;
			case GL_BOOL_VEC4:
				variable_type = shader_variable_type::bool4;
				break;

			case GL_INT:
				variable_type = shader_variable_type::int1;
				break;
			case GL_INT_VEC2:
				variable_type = shader_variable_type::int2;
				break;
			case GL_INT_VEC3:
				variable_type = shader_variable_type::int3;
				break;
			case GL_INT_VEC4:
				variable_type = shader_variable_type::int4;
				break;

			case GL_UNSIGNED_INT:
				variable_type = shader_variable_type::uint1;
				break;
			case GL_UNSIGNED_INT_VEC2:
				variable_type = shader_variable_type::uint2;
				break;
			case GL_UNSIGNED_INT_VEC3:
				variable_type = shader_variable_type::uint3;
				break;
			case GL_UNSIGNED_INT_VEC4:
				variable_type = shader_variable_type::uint4;
				break;
				
			case GL_FLOAT:
				variable_type = shader_variable_type::float1;
				break;
			case GL_FLOAT_VEC2:
				variable_type = shader_variable_type::float2;
				break;
			case GL_FLOAT_VEC3:
				variable_type = shader_variable_type::float3;
				break;
			case GL_FLOAT_VEC4:
				variable_type = shader_variable_type::float4;
				break;

			case GL_FLOAT_MAT2:
				variable_type = shader_variable_type::float2x2;
				break;
			case GL_FLOAT_MAT3:
				variable_type = shader_variable_type::float3x3;
				break;
			case GL_FLOAT_MAT4:
				variable_type = shader_variable_type::float4x4;
				break;
			
			case GL_SAMPLER_1D:
			case GL_SAMPLER_1D_SHADOW:
				variable_type = shader_variable_type::texture_1d;
				texture_unit = available_texture_unit;
				available_texture_unit += uniform_size;
				break;
						
			case GL_SAMPLER_2D:
			case GL_SAMPLER_2D_SHADOW:
				variable_type = shader_variable_type::texture_2d;
				texture_unit = available_texture_unit;
				available_texture_unit += uniform_size;
				break;
			
			case GL_SAMPLER_3D:
				variable_type = shader_variable_type::texture_3d;
				texture_unit = available_texture_unit;
				available_texture_unit += uniform_size;
				break;
			
			case GL_SAMPLER_CUBE:
				variable_type = shader_variable_type::texture_cube;
				texture_unit = available_texture_unit;
				available_texture_unit += uniform_size;
				break;
			
			default:
				unsupported = true;
				break;
		}
		
		// Check if data type is supported
		if (unsupported)
		{
			std::string message = std::string("Shader uniform \"") + std::string(uniform_name) + std::string("\" has unsupported data type.");
			throw std::runtime_error(message.c_str());
		}
		
		// Get uniform location
		GLint uniform_location = glGetUniformLocation(gl_program_id, uniform_name);
		if (uniform_location == -1)
		{
			//std::string message = std::string("Unable to get location for uniform \"") + std::string(uniform_name) + std::string("\"");
			//throw std::runtime_error(message.c_str());
		}
		else
		{
			// Create new shader input
			shader_input* input = new shader_input(this, inputs.size(), uniform_location, input_name, variable_type, uniform_size, texture_unit);
			input_map[input_name] = input;
			inputs.push_back(input);
		}
	}
	
	// Free uniform name buffer
	delete[] uniform_name;
}

void shader_program::free_inputs()
{
	for (shader_input* input: inputs)
	{
		delete input;
	}
	
	inputs.clear();
}

} // namespace gl
