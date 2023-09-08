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

#ifndef ANTKEEPER_GL_TEXTURE_CUBE_HPP
#define ANTKEEPER_GL_TEXTURE_CUBE_HPP

#include <engine/gl/texture.hpp>
#include <engine/gl/cube-map-layout.hpp>

namespace gl {

/**
 * A cube texture which can be uploaded to shaders via shader inputs.
 */
class texture_cube: public texture
{
public:
	/**
	 * Infers the layout of a cube map from its aspect ratio.
	 *
	 * @param w Width of the cube map, in pixels.
	 * @param h Height of the cube map, in pixels.
	 *
	 * @return Inferred cube map layout.
	 */
	[[nodiscard]] static cube_map_layout infer_cube_map_layout(std::uint16_t w, std::uint16_t h) noexcept;
	
	/**
	 * Infers the edge length of a cube map face from its layout and resolution.
	 *
	 * @param layout Layout of the cube map.
	 * @param w Width of the cube map, in pixels.
	 * @param h Height of the cube map, in pixels.
	 *
	 * @return Edge length of the cube map faces, in pixels.
	 */
	[[nodiscard]] static std::uint16_t infer_cube_map_face_size(cube_map_layout layout, std::uint16_t w, std::uint16_t h) noexcept;
	
	/// Constructs a cube texture.
	texture_cube(std::uint16_t width, std::uint16_t height, gl::pixel_type type = gl::pixel_type::uint_8, gl::pixel_format format = gl::pixel_format::rgba, gl::transfer_function transfer_function = gl::transfer_function::linear, const std::byte* data = nullptr);
	
	[[nodiscard]] inline constexpr texture_type get_texture_type() const noexcept override
	{
		return texture_type::cube;
	}
	
	/// @copydoc texture::resize(std::uint16_t, std::uint16_t, gl::pixel_type, gl::pixel_format, gl::transfer_function, const std::byte*)
	void resize(std::uint16_t width, std::uint16_t height, gl::pixel_type type, gl::pixel_format format, gl::transfer_function transfer_function, const std::byte* data) override;
	
	/**
	 * Resizes the texture.
	 *
	 * @param width Texture width, in pixels.
	 * @param height Texture height, in pixels.
	 * @param data Pointer to pixel data.
	 */
	void resize(std::uint16_t width, std::uint16_t height, const std::byte* data);
	
	/// @copydoc texture::set_wrapping(gl::texture_wrapping, gl::texture_wrapping, gl::texture_wrapping)
	virtual void set_wrapping(gl::texture_wrapping wrap_s, texture_wrapping wrap_t, texture_wrapping wrap_r);
	
	/// Returns the edge length of the cube texture faces, in pixels.
	[[nodiscard]] inline std::uint16_t get_face_size() const noexcept
	{
		return m_face_size;
	}
	
private:
	void resized();
	
	std::uint16_t m_face_size{};
};

} // namespace gl

#endif // ANTKEEPER_GL_TEXTURE_CUBE_HPP
