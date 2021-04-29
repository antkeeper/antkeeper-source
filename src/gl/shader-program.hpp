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

#ifndef ANTKEEPER_GL_SHADER_PROGRAM_HPP
#define ANTKEEPER_GL_SHADER_PROGRAM_HPP

#include <cstdlib>
#include <list>
#include <map>
#include <string>

namespace gl {

class shader_object;
class rasterizer;
class shader_input;

class shader_program
{
public:
	/**
	 * Creates a shader program.
	 *
	 * @param shaders List of shaders to be linked to the program. Note that after the shader program has been created, these shaders can be deleted if desired with no effect on the shader program.
	 */
	explicit shader_program(const std::list<shader_object*>& shaders);
	~shader_program();

	shader_program(const shader_program&) = delete;
	shader_program& operator=(const shader_program&) = delete;

	const std::list<shader_input*>* get_inputs() const;
	const shader_input* get_input(const std::string& name) const;

private:
	friend class rasterizer;

	void find_inputs();
	std::string get_info_log() const;

	unsigned int gl_program_id;
	std::list<shader_input*> inputs;
	std::map<std::string, shader_input*> input_map;
};

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

