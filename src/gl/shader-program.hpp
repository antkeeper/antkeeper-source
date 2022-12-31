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

#ifndef ANTKEEPER_GL_SHADER_PROGRAM_HPP
#define ANTKEEPER_GL_SHADER_PROGRAM_HPP

#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace gl {

class shader_object;
class rasterizer;
class shader_input;

/**
 * Shader program which can be linked to shader objects and executed.
 *
 * @see gl::shader_object
 */
class shader_program
{
public:
	/**
	 * Creates an empty shader program.
	 *
	 * @exception std::runtime_error An error occurred while creating an OpenGL shader program.
	 */
	shader_program();
	
	/**
	 * Destroys a shader program.
	 */
	~shader_program();
	
	/**
	 * Attaches a shader object to the shader program. Attaching a shader object has no effect on a shader program until shader_program::link() is called.
	 *
	 * @param object Shader object to attach.
	 *
	 * @exception std::runtime_error Shader object is already attached to the shader program.
	 * @exception std::runtime_error OpenGL shader program is not a valid program object.
	 * @exception std::runtime_error OpenGL shader object is not a valid shader object.
	 * @exception std::runtime_error OpenGL shader object is already attached to the shader program.
	 *
	 * @see shader_program::link()
	 */
	void attach(const shader_object* object);
	
	/**
	 * Detaches a shader object from the shader program. Detaching a shader object has no effect on a shader program until shader_program::link() is called.
	 *
	 * @param object Shader object to detach.
	 *
	 * @exception std::runtime_error Shader object is not attached to the shader program.
	 * @exception std::runtime_error OpenGL shader program is not a valid program object.
	 * @exception std::runtime_error OpenGL shader object is not a valid shader object.
	 * @exception std::runtime_error OpenGL shader object is not attached to the shader program.
	 *
	 * @see shader_program::link()
	 */
	void detach(const shader_object* object);
	
	/**
	 * Detaches all shader objects from the shader program.
	 *
	 * @exception std::runtime_error Shader object is not attached to the shader program.
	 * @exception std::runtime_error OpenGL shader program is not a valid program object.
	 * @exception std::runtime_error OpenGL shader object is not a valid shader object.
	 * @exception std::runtime_error OpenGL shader object is not attached to the shader program.
	 *
	 * @see shader_program::detach(const shader_object*)
	 */
	void detach_all();
	
	/**
	 * Links all attached shader objects to create an executable shader program.
	 *
	 * @warning All existing pointers to a shader program's shader inputs will be invalidated if the program is re-linked.
	 *
	 * @return `true` if the attached shader objects were successfully linked into the shader program, `false` otherwise. If linking fails, check the info log via shader_program::get_info_log() for more information.
	 */
	bool link();
	
	/// Returns the shader program info log, which is updated when the shader program is linked.
	const std::string& get_info_log() const;
	
	/// Returns `true` if the shader program has been successfully linked, `false` otherwise.
	bool was_linked() const;

	shader_program(const shader_program&) = delete;
	shader_program& operator=(const shader_program&) = delete;

	const std::list<shader_input*>* get_inputs() const;
	const shader_input* get_input(const std::string& name) const;

private:
	friend class rasterizer;
	
	unsigned int gl_program_id;
	std::string info_log;
	bool linked;
	std::unordered_set<const shader_object*> attached_objects;

	void find_inputs();
	void free_inputs();
	
	std::list<shader_input*> inputs;
	std::unordered_map<std::string, shader_input*> input_map;
	
};

inline const std::string& shader_program::get_info_log() const
{
	return info_log;
}

inline bool shader_program::was_linked() const
{
	return linked;
}

inline const std::list<shader_input*>* shader_program::get_inputs() const
{
	return &inputs;
}

inline const shader_input* shader_program::get_input(const std::string& name) const
{
	auto it = input_map.find(name);
	if (it == input_map.end())
	{
		return nullptr;
	}

	return it->second;
}

} // namespace gl

#endif // ANTKEEPER_GL_SHADER_PROGRAM_HPP
