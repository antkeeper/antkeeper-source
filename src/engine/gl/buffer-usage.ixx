// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.buffer_usage;
import engine.utility.sized_types;

export namespace engine::gl
{
	/// Usage hints for vertex buffers.
	/// @see gl::vertex_buffer
	enum class buffer_usage: u8
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
}
