// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <glad/gl.h>
import engine.gl.shader_program;
import engine.gl.shader_object;
import engine.gl.opengl.shader_variables;
import engine.debug.log;
import engine.utility.sized_types;
import <string_view>;
import <stdexcept>;
import <unordered_set>;

namespace engine::gl
{
	shader_program::shader_program()
	{
		// Create an OpenGL shader program
		m_gl_program_id = glCreateProgram();

		// Handle OpenGL errors
		if (!m_gl_program_id)
		{
			throw std::runtime_error("Failed to create OpenGL shader program");
		}
	}

	shader_program::~shader_program()
	{
		// Detach all shader objects
		detach_all();

		// Delete the OpenGL shader program
		glDeleteProgram(m_gl_program_id);
	}

	void shader_program::attach(const shader_object& object)
	{
		if (m_attached_objects.find(&object) != m_attached_objects.end())
		{
			throw std::runtime_error("OpenGL shader object already attached to the shader program");
		}
		else
		{
			// Check that both the OpenGL shader program and OpenGL shader object are valid
			if (glIsProgram(m_gl_program_id) != GL_TRUE)
			{
				throw std::runtime_error("Invalid OpenGL shader program");
			}
			if (glIsShader(object.m_gl_shader_id) != GL_TRUE)
			{
				throw std::runtime_error("Invalid OpenGL shader object");
			}

			// Attach the OpenGL shader object to the OpenGL shader program
			glAttachShader(m_gl_program_id, object.m_gl_shader_id);

			// Add shader object to set of attached objects
			m_attached_objects.insert(&object);
		}
	}

	void shader_program::detach(const shader_object& object)
	{
		if (m_attached_objects.find(&object) == m_attached_objects.end())
		{
			throw std::runtime_error("Shader object is not attached to the shader program.");
		}
		else
		{
			// Check that both the OpenGL shader program and OpenGL shader object are valid
			if (glIsProgram(m_gl_program_id) != GL_TRUE)
			{
				throw std::runtime_error("Invalid OpenGL shader program");
			}
			if (glIsShader(object.m_gl_shader_id) != GL_TRUE)
			{
				throw std::runtime_error("Invalid OpenGL shader object");
			}

			// Detach the OpenGL shader object from the OpenGL shader program
			glDetachShader(m_gl_program_id, object.m_gl_shader_id);

			// Remove shader object from set of attached objects
			m_attached_objects.erase(&object);
		}
	}

	void shader_program::detach_all()
	{
		while (!m_attached_objects.empty())
		{
			detach(**m_attached_objects.begin());
		}
	}

	bool shader_program::link()
	{
		m_linked = false;
		m_info_log.clear();
		m_variable_map.clear();

		// Check that the OpenGL shader program is valid
		if (glIsProgram(m_gl_program_id) != GL_TRUE)
		{
			throw std::runtime_error("Invalid OpenGL shader program");
		}

		// Link OpenGL shader program
		glLinkProgram(m_gl_program_id);

		// Get OpenGL shader program linking status
		GLint gl_link_status;
		glGetProgramiv(m_gl_program_id, GL_LINK_STATUS, &gl_link_status);
		m_linked = (gl_link_status == GL_TRUE);

		// Get OpenGL shader program info log length
		GLint gl_info_log_length;
		glGetProgramiv(m_gl_program_id, GL_INFO_LOG_LENGTH, &gl_info_log_length);

		if (gl_info_log_length > 0)
		{
			// Resize string to accommodate OpenGL shader program info log
			m_info_log.resize(gl_info_log_length);

			// Read OpenGL shader program info log into string
			glGetProgramInfoLog(m_gl_program_id, gl_info_log_length, &gl_info_log_length, m_info_log.data());

			// Remove redundant null terminator from string
			m_info_log.pop_back();
		}

		if (m_linked)
		{
			if (!m_info_log.empty())
			{
				log_warning("Linked shader program {} with warnings: {}", m_gl_program_id, m_info_log);
			}

			// Load shader variables
			load_variables();
		}
		else
		{
			log_error("Failed to link shader program {}: {}", m_gl_program_id, m_info_log.empty() ? "Unknown error" : m_info_log);
		}

		return m_linked;
	}

	void shader_program::load_variables()
	{
		m_variable_map.clear();

		// Get maximum uniform name length
		GLint max_uniform_name_length = 0;
		glGetProgramiv(m_gl_program_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_uniform_name_length);

		// Allocate uniform name buffer
		std::string uniform_name(static_cast<usize>(max_uniform_name_length), '\0');

		// Get number of active uniforms in the shader
		GLint active_uniform_count = 0;
		glGetProgramiv(m_gl_program_id, GL_ACTIVE_UNIFORMS, &active_uniform_count);

		// Init texture unit index
		GLint texture_index = 0;

		// For each active uniform
		for (GLint uniform_index = 0; uniform_index < active_uniform_count; ++uniform_index)
		{
			// Get uniform info
			GLsizei uniform_name_length;
			GLint uniform_size;
			GLenum uniform_type;
			glGetActiveUniform(m_gl_program_id, static_cast<GLuint>(uniform_index), static_cast<GLsizei>(max_uniform_name_length), &uniform_name_length, &uniform_size, &uniform_type, uniform_name.data());

			// Get uniform location
			const GLint uniform_location = glGetUniformLocation(m_gl_program_id, uniform_name.c_str());
			if (uniform_location == -1)
			{
				throw std::runtime_error("Failed to get shader uniform location");
			}

			// Get length of variable name by stripping array notation from uniform name
			usize uniform_base_name_length = static_cast<usize>(uniform_name_length);
			if (usize i = std::string_view(uniform_name.c_str(), uniform_name_length).find_first_of("["); i != std::string::npos)
			{
				uniform_base_name_length = i;
			}

			// Hash variable name to get variable key
			const hash::fnv32_t variable_key = hash::fnv1a32<char>({uniform_name.c_str(), uniform_base_name_length});

			// Get size of variable
			const usize variable_size = static_cast<usize>(uniform_size);

			// Construct shader variable
			std::unique_ptr<const gl::shader_variable> variable;
			switch (uniform_type)
			{
				case GL_BOOL:
					variable = std::make_unique<const opengl::shader_bool>(variable_size, uniform_location);
					break;
				case GL_BOOL_VEC2:
					variable = std::make_unique<const opengl::shader_bvec2>(variable_size, uniform_location);
					break;
				case GL_BOOL_VEC3:
					variable = std::make_unique<const opengl::shader_bvec3>(variable_size, uniform_location);
					break;
				case GL_BOOL_VEC4:
					variable = std::make_unique<const opengl::shader_bvec4>(variable_size, uniform_location);
					break;

				case GL_INT:
					variable = std::make_unique<const opengl::shader_int>(variable_size, uniform_location);
					break;
				case GL_INT_VEC2:
					variable = std::make_unique<const opengl::shader_ivec2>(variable_size, uniform_location);
					break;
				case GL_INT_VEC3:
					variable = std::make_unique<const opengl::shader_ivec3>(variable_size, uniform_location);
					break;
				case GL_INT_VEC4:
					variable = std::make_unique<const opengl::shader_ivec4>(variable_size, uniform_location);
					break;

				case GL_UNSIGNED_INT:
					variable = std::make_unique<const opengl::shader_uint>(variable_size, uniform_location);
					break;
				case GL_UNSIGNED_INT_VEC2:
					variable = std::make_unique<const opengl::shader_uvec2>(variable_size, uniform_location);
					break;
				case GL_UNSIGNED_INT_VEC3:
					variable = std::make_unique<const opengl::shader_uvec3>(variable_size, uniform_location);
					break;
				case GL_UNSIGNED_INT_VEC4:
					variable = std::make_unique<const opengl::shader_uvec4>(variable_size, uniform_location);
					break;

				case GL_FLOAT:
					variable = std::make_unique<const opengl::shader_float>(variable_size, uniform_location);
					break;
				case GL_FLOAT_VEC2:
					variable = std::make_unique<const opengl::shader_fvec2>(variable_size, uniform_location);
					break;
				case GL_FLOAT_VEC3:
					variable = std::make_unique<const opengl::shader_fvec3>(variable_size, uniform_location);
					break;
				case GL_FLOAT_VEC4:
					variable = std::make_unique<const opengl::shader_fvec4>(variable_size, uniform_location);
					break;

				case GL_FLOAT_MAT2:
					variable = std::make_unique<const opengl::shader_fmat2>(variable_size, uniform_location);
					break;
				case GL_FLOAT_MAT3:
					variable = std::make_unique<const opengl::shader_fmat3>(variable_size, uniform_location);
					break;
				case GL_FLOAT_MAT4:
					variable = std::make_unique<const opengl::shader_fmat4>(variable_size, uniform_location);
					break;

				case GL_SAMPLER_1D:
				case GL_SAMPLER_1D_SHADOW:
					variable = std::make_unique<const opengl::shader_texture_1d>(variable_size, uniform_location, texture_index);
					texture_index += uniform_size;
					break;

				case GL_SAMPLER_2D:
				case GL_SAMPLER_2D_SHADOW:
					variable = std::make_unique<const opengl::shader_texture_2d>(variable_size, uniform_location, texture_index);
					texture_index += uniform_size;
					break;

				case GL_SAMPLER_3D:
					variable = std::make_unique<const opengl::shader_texture_3d>(variable_size, uniform_location, texture_index);
					texture_index += uniform_size;
					break;

				case GL_SAMPLER_CUBE:
					variable = std::make_unique<const opengl::shader_texture_cube>(variable_size, uniform_location, texture_index);
					texture_index += uniform_size;
					break;

				default:
					throw std::runtime_error("Unsupported to shader uniform type");
					break;
			}

			// Map variable to variable key
			m_variable_map.emplace(variable_key, std::move(variable));
		}
	}
}
