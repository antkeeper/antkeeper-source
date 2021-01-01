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

#ifndef ANTKEEPER_SHADER_HPP
#define ANTKEEPER_SHADER_HPP

#include <cstdlib>
#include <string>

enum class shader_type;
class shader_program;

class shader
{
public:
	shader(shader_type type, const std::string& source);
	~shader();

	shader(const shader&) = delete;
	shader& operator=(const shader&) = delete;

	shader_type get_type() const;

private:
	friend class shader_program;

	std::string get_info_log() const;

	unsigned int gl_shader_id;
	shader_type type;
};

inline shader_type shader::get_type() const
{
	return type;
}

#endif // ANTKEEPER_SHADER_HPP

