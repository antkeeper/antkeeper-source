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

#ifndef ANTKEEPER_GL_BUFFER_USAGE_HPP
#define ANTKEEPER_GL_BUFFER_USAGE_HPP

namespace gl {

/**
 * Usage hints for vertex buffers.
 *
 * @see gl::vertex_buffer
 */
enum class buffer_usage
{
	/// Data will be modified once, by the application, and used at most a few times, for drawing commands.
	stream_draw,
	
	/// Data will be modified once, by reading data from the GL, and used at most a few times, to return that data to the application.
	stream_read,
	
	/// Data will be modified once, by reading data from the GL, and used at most a few times, for drawing commands.
	stream_copy,
	
	/// Data will be modified once, by the application, and used many times, for drawing commands.
	static_draw,
	
	/// Data will be modified once, by reading data from the GL, and used many times, to return that data to the application.
	static_read,
	
	/// Data will be modified once, by reading data from the GL, and many times, for drawing commands.
	static_copy,
	
	/// Data will be modified repeatedly, by the application, and used many times, for drawing commands.
	dynamic_draw,
	
	/// Data will be modified repeatedly, by reading data from the GL, and used many times, to return that data to the application.
	dynamic_read,
	
	/// Data will be modified repeatedly, by reading data from the GL, and used many times, for drawing commands.
	dynamic_copy
};

} // namespace gl

#endif // ANTKEEPER_GL_BUFFER_USAGE_HPP
