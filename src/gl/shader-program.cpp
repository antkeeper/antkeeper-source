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

#include "gl/shader-program.hpp"
#include "gl/shader.hpp"
#include "gl/shader-variable-type.hpp"
#include "gl/shader-input.hpp"
#include <glad/glad.h>
#include <stdexcept>

namespace gl {

shader_program::shader_program(const std::list<shader*>& shaders):
	gl_program_id(0)
{
	gl_program_id = glCreateProgram();

	for (shader* shader: shaders)
	{
		glAttachShader(gl_program_id, shader->gl_shader_id);
	}

	glLinkProgram(gl_program_id);
	
	GLint status;
	glGetProgramiv(gl_program_id, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		throw std::runtime_error(get_info_log().c_str());
	}

	for (shader* shader: shaders)
	{
		glDetachShader(gl_program_id, shader->gl_shader_id);
	}

	// Find shader inputs
	find_inputs();
}

shader_program::~shader_program()
{
	glDeleteProgram(gl_program_id);

	for (shader_input* input: inputs)
	{
		delete input;
	}
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
						
			case GL_SAMPLER_2D:
			case GL_SAMPLER_2D_SHADOW:
				variable_type = shader_variable_type::texture_2d;
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

std::string shader_program::get_info_log() const
{
	GLint length;
	glGetProgramiv(gl_program_id, GL_INFO_LOG_LENGTH, &length);

	if (length > 0)
	{
		std::string log(length, '\0');
		glGetProgramInfoLog(gl_program_id, length, &length, &log[0]);
		return log;
	}

	return std::string();
}

} // namespace gl
