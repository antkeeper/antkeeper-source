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

#ifndef ANTKEEPER_VERTEX_BUFFER_HPP
#define ANTKEEPER_VERTEX_BUFFER_HPP

#include <cstdlib>
#include "rasterizer/buffer-usage.hpp"

class vertex_array;

class vertex_buffer
{
public:
	explicit vertex_buffer(std::size_t size, const void* data = nullptr, buffer_usage usage = buffer_usage::static_draw);
	vertex_buffer();
	~vertex_buffer();

	vertex_buffer(const vertex_buffer&) = delete;
	vertex_buffer& operator=(const vertex_buffer&) = delete;

	void repurpose(std::size_t size, const void* data = nullptr, buffer_usage usage = buffer_usage::static_draw);
	void resize(std::size_t size, const void* data = nullptr);

	void update(int offset, std::size_t size, const void* data);

	std::size_t get_size() const;
	buffer_usage get_usage() const;

private:
	friend class vertex_array;

	unsigned int gl_buffer_id;
	std::size_t size;
	buffer_usage usage;
};

inline std::size_t vertex_buffer::get_size() const
{
	return size;
}

inline buffer_usage vertex_buffer::get_usage() const
{
	return usage;
}

#endif // ANTKEEPER_VERTEX_BUFFER_HPP

