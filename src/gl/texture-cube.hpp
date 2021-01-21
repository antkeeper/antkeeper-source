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

#ifndef ANTKEEPER_GL_TEXTURE_CUBE_HPP
#define ANTKEEPER_GL_TEXTURE_CUBE_HPP

namespace gl {

class shader_input;

/**
 * A cube texture which can be uploaded to shaders via shader inputs.
 */
class texture_cube
{
public:
	/**
	 * Creates a cube texture.
	 */
	texture_cube();
	
	/**
	 * Destroys a cube texture.
	 */
	~texture_cube();

	/// Returns the linear size of a cube face, in pixels.
	int get_face_size() const;
	
private:
	friend class shader_input;

	unsigned int gl_texture_id;
	int face_size;
};

inline int texture_cube::get_face_size() const
{
	return face_size;
}

} // namespace gl

#endif // ANTKEEPER_GL_TEXTURE_CUBE_HPP

