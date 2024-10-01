// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_SHADER_PROGRAM_HPP
#define ANTKEEPER_GL_SHADER_PROGRAM_HPP

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <memory>
#include <engine/hash/fnv1a.hpp>

namespace gl {

class shader_object;
class shader_variable;

/// Shader program which can be linked to shader objects and executed.
/// @see gl::shader_object
class shader_program
{
public:
	/// Creates an empty shader program.
	/// @exception std::runtime_error An error occurred while creating an OpenGL shader program.
	shader_program();
	
	/// Destroys a shader program.
	~shader_program();
	
	shader_program(const shader_program&) = delete;
	shader_program(shader_program&&) = delete;
	shader_program& operator=(const shader_program&) = delete;
	shader_program& operator=(shader_program&&) = delete;
	
	/// Attaches a shader object to the shader program. Attaching a shader object has no effect on a shader program until shader_program::link() is called.
	/// @param object Shader object to attach.
	/// @exception std::runtime_error Shader object is already attached to the shader program.
	/// @exception std::runtime_error OpenGL shader program is not a valid program object.
	/// @exception std::runtime_error OpenGL shader object is not a valid shader object.
	/// @exception std::runtime_error OpenGL shader object is already attached to the shader program.
	/// @see shader_program::link()
	void attach(const shader_object& object);
	
	/// Detaches a shader object from the shader program. Detaching a shader object has no effect on a shader program until shader_program::link() is called.
	/// @param object Shader object to detach.
	/// @exception std::runtime_error Shader object is not attached to the shader program.
	/// @exception std::runtime_error OpenGL shader program is not a valid program object.
	/// @exception std::runtime_error OpenGL shader object is not a valid shader object.
	/// @exception std::runtime_error OpenGL shader object is not attached to the shader program.
	/// @see shader_program::link()
	void detach(const shader_object& object);
	
	/// Detaches all shader objects from the shader program.
	/// @exception std::runtime_error Shader object is not attached to the shader program.
	/// @exception std::runtime_error OpenGL shader program is not a valid program object.
	/// @exception std::runtime_error OpenGL shader object is not a valid shader object.
	/// @exception std::runtime_error OpenGL shader object is not attached to the shader program.
	/// @see shader_program::detach(const shader_object*)
	void detach_all();
	
	/// Links all attached shader objects to create an executable shader program.
	/// @return `true` if the attached shader objects were successfully linked into the shader program, `false` otherwise.
	/// @warning All existing of the shader program's variables will be invalidated if the program is re-linked.
	bool link();
	
	/// Returns `true` if the shader program has been successfully linked, `false` otherwise.
	[[nodiscard]] inline bool linked() const noexcept
	{
		return m_linked;
	}
	
	/// Returns all active shader variables in the shader program.
	/// @return Map of 32-bit FNV-1a hash values of shader variable names to shader variables.
	[[nodiscard]] inline const std::unordered_map<hash::fnv1a32_t, const std::unique_ptr<const shader_variable>>& variables() const noexcept
	{
		return m_variable_map;
	}
	
	/// Returns a pointer to an active shader variable with the given name, or `nullptr` if not found.
	/// @param key 32-bit FNV-1a hash value of a shader variable name.
	/// @return Pointer to the active shader variable with the given name, or `nullptr` if not found.
	[[nodiscard]] inline const shader_variable* variable(hash::fnv1a32_t key) const
	{
		if (auto i = m_variable_map.find(key); i != m_variable_map.end())
		{
			return i->second.get();
		}
		
		return nullptr;
	}
	
	/// Returns the info log that contains debug information when linking fails.
	[[nodiscard]] inline const std::string& info() const noexcept
	{
		return m_info_log;
	}

private:
	friend class pipeline;
	
	void load_variables();
	
	unsigned int m_gl_program_id{0};
	bool m_linked{false};
	std::unordered_set<const shader_object*> m_attached_objects;
	std::unordered_map<hash::fnv1a32_t, const std::unique_ptr<const shader_variable>> m_variable_map;
	std::string m_info_log;
};

} // namespace gl

#endif // ANTKEEPER_GL_SHADER_PROGRAM_HPP
