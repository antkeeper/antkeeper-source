/*
 * Copyright (C) 2020  Christopher J. Howard
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

#ifndef ANTKEEPER_VERTEX_ARRAY_HPP
#define ANTKEEPER_VERTEX_ARRAY_HPP

#include <cstdlib>

class rasterizer;
class vertex_buffer;
enum class vertex_attribute_type;

class vertex_array
{
public:
	explicit vertex_array();
	~vertex_array();

	vertex_array(const vertex_array&) = delete;
	vertex_array& operator=(const vertex_array&) = delete;

	void bind_attribute(unsigned int index, const vertex_buffer& buffer, int size, vertex_attribute_type type, int stride, std::size_t offset);
	void bind_elements(const vertex_buffer& buffer);

private:
	friend class rasterizer;

	unsigned int gl_array_id;
};

#endif // ANTKEEPER_VERTEX_ARRAY_HPP

