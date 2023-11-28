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

#ifndef ANTKEEPER_GL_TEXTURE_HPP
#define ANTKEEPER_GL_TEXTURE_HPP

#include <engine/gl/image-view.hpp>
#include <engine/gl/sampler.hpp>
#include <memory>

namespace gl {

/**
 * Image view and sampler object pair.
 */
class texture
{
public:
	/**
	 * Constructs a texture.
	 *
	 * @param image_view Texture image view.
	 * @param sampler Texture sampler.
	 */
	/// @{
	inline explicit texture(std::shared_ptr<gl::sampler> sampler) noexcept:
		m_sampler(sampler)
	{}
	
	constexpr texture() noexcept = default;
	/// @}
	
	/**
	 * Sets the sampler object.
	 *
	 * @param sampler Sampler object.
	 */
	inline void set_sampler(std::shared_ptr<gl::sampler> sampler)
	{
		m_sampler = sampler;
	}
	
	/// Returns the sampler object.
	[[nodiscard]] inline constexpr const std::shared_ptr<sampler>& get_sampler() const noexcept
	{
		return m_sampler;
	}
	
private:
	std::shared_ptr<sampler> m_sampler;
};

/**
 * 1D texture.
 */
class texture_1d: public texture
{
public:
	/// @copydoc texture::texture
	/// @{
	inline texture_1d(std::shared_ptr<image_view_1d> image_view, std::shared_ptr<gl::sampler> sampler) noexcept:
		texture(sampler),
		m_image_view(image_view)
	{}
	
	constexpr texture_1d() noexcept = default;
	/// @}
	
	/**
	 * Sets the image view.
	 */
	inline void set_image_view(std::shared_ptr<gl::image_view_1d> image_view)
	{
		m_image_view = image_view;
	}
	
	/// Returns the image view.
	[[nodiscard]] inline constexpr const std::shared_ptr<image_view_1d>& get_image_view() const noexcept
	{
		return m_image_view;
	}
	
private:
	std::shared_ptr<image_view_1d> m_image_view;
};

/**
 * 1D texture array.
 */
class texture_1d_array: public texture
{
public:
	/// @copydoc texture::texture
	/// @{
	inline texture_1d_array(std::shared_ptr<image_view_1d_array> image_view, std::shared_ptr<gl::sampler> sampler) noexcept:
		texture(sampler),
		m_image_view(image_view)
	{}
	
	constexpr texture_1d_array() noexcept = default;
	/// @}
	
	/**
	 * Sets the image view.
	 */
	inline void set_image_view(std::shared_ptr<gl::image_view_1d_array> image_view)
	{
		m_image_view = image_view;
	}
	
	/// Returns the image view.
	[[nodiscard]] inline constexpr const std::shared_ptr<image_view_1d_array>& get_image_view() const noexcept
	{
		return m_image_view;
	}
	
private:
	std::shared_ptr<image_view_1d_array> m_image_view;
};

/**
 * 2D texture.
 */
class texture_2d: public texture
{
public:
	/// @copydoc texture::texture
	/// @{
	inline texture_2d(std::shared_ptr<image_view_2d> image_view, std::shared_ptr<gl::sampler> sampler) noexcept:
		texture(sampler),
		m_image_view(image_view)
	{}
	
	constexpr texture_2d() noexcept = default;
	/// @}
	
	/**
	 * Sets the image view.
	 */
	inline void set_image_view(std::shared_ptr<gl::image_view_2d> image_view)
	{
		m_image_view = image_view;
	}
	
	/// Returns the image view.
	[[nodiscard]] inline constexpr const std::shared_ptr<image_view_2d>& get_image_view() const noexcept
	{
		return m_image_view;
	}
	
private:
	std::shared_ptr<image_view_2d> m_image_view;
};

/**
 * 2D texture array.
 */
class texture_2d_array: public texture
{
public:
	/// @copydoc texture::texture
	/// @{
	inline texture_2d_array(std::shared_ptr<image_view_2d_array> image_view, std::shared_ptr<gl::sampler> sampler) noexcept:
		texture(sampler),
		m_image_view(image_view)
	{}
	
	constexpr texture_2d_array() noexcept = default;
	/// @}
	
	/**
	 * Sets the image view.
	 */
	inline void set_image_view(std::shared_ptr<gl::image_view_2d_array> image_view)
	{
		m_image_view = image_view;
	}
	
	/// Returns the image view.
	[[nodiscard]] inline constexpr const std::shared_ptr<image_view_2d_array>& get_image_view() const noexcept
	{
		return m_image_view;
	}
	
private:
	std::shared_ptr<image_view_2d_array> m_image_view;
};

/**
 * 3D texture.
 */
class texture_3d: public texture
{
public:
	/// @copydoc texture::texture
	/// @{
	inline texture_3d(std::shared_ptr<image_view_3d> image_view, std::shared_ptr<gl::sampler> sampler) noexcept:
		texture(sampler),
		m_image_view(image_view)
	{}
	
	constexpr texture_3d() noexcept = default;
	/// @}
	
	/**
	 * Sets the image view.
	 */
	inline void set_image_view(std::shared_ptr<gl::image_view_3d> image_view)
	{
		m_image_view = image_view;
	}
	
	/// Returns the image view.
	[[nodiscard]] inline constexpr const std::shared_ptr<image_view_3d>& get_image_view() const noexcept
	{
		return m_image_view;
	}
	
private:
	std::shared_ptr<image_view_3d> m_image_view;
};

/**
 * Cube texture.
 */
class texture_cube: public texture
{
public:
	/// @copydoc texture::texture
	/// @{
	inline texture_cube(std::shared_ptr<image_view_cube> image_view, std::shared_ptr<gl::sampler> sampler) noexcept:
		texture(sampler),
		m_image_view(image_view)
	{}
	
	constexpr texture_cube() noexcept = default;
	/// @}
	
	/**
	 * Sets the image view.
	 */
	inline void set_image_view(std::shared_ptr<gl::image_view_cube> image_view)
	{
		m_image_view = image_view;
	}
	
	/// Returns the image view.
	[[nodiscard]] inline constexpr const std::shared_ptr<image_view_cube>& get_image_view() const noexcept
	{
		return m_image_view;
	}
	
private:
	std::shared_ptr<image_view_cube> m_image_view;
};

/**
 * Cube texture array.
 */
class texture_cube_array: public texture
{
public:
	/// @copydoc texture::texture
	/// @{
	inline texture_cube_array(std::shared_ptr<image_view_cube_array> image_view, std::shared_ptr<gl::sampler> sampler) noexcept:
		texture(sampler),
		m_image_view(image_view)
	{}
	
	constexpr texture_cube_array() noexcept = default;
	/// @}
	
	/**
	 * Sets the image view.
	 */
	inline void set_image_view(std::shared_ptr<gl::image_view_cube_array> image_view)
	{
		m_image_view = image_view;
	}
	
	/// Returns the image view.
	[[nodiscard]] inline constexpr const std::shared_ptr<image_view_cube_array>& get_image_view() const noexcept
	{
		return m_image_view;
	}
	
private:
	std::shared_ptr<image_view_cube_array> m_image_view;
};

} // namespace gl

#endif // ANTKEEPER_GL_TEXTURE_HPP
