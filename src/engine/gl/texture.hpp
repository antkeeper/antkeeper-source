// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/gl/image-view.hpp>
#include <engine/gl/sampler.hpp>
#include <memory>

namespace engine::gl
{
	/// Image view and sampler object pair.
	class texture
	{
	public:
		/// Constructs a texture.
		/// @param sampler Sampler object.
		inline explicit texture(std::shared_ptr<gl::sampler> sampler) noexcept:
			m_sampler(sampler)
		{
		}

		/// Constructs a texture.
		constexpr texture() noexcept = default;

		/// Destructs a texture.
		virtual ~texture() = default;

		/// Sets the sampler object.
		/// @param sampler Sampler object.
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

	/// 1D texture.
	class texture_1d: public texture
	{
	public:
		/// Constructs a 1D texture.
		/// @param image_view Image view.
		/// @param sampler Sampler object.
		inline texture_1d(std::shared_ptr<image_view_1d> image_view, std::shared_ptr<gl::sampler> sampler) noexcept:
			texture(sampler),
			m_image_view(image_view)
		{
		}

		/// Constructs a 1D texture.
		constexpr texture_1d() noexcept = default;

		/// Destructs a 1D texture.
		~texture_1d() override = default;

		/// Sets the image view.
		/// @param image_view Image view.
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

	/// 1D texture array.
	class texture_1d_array: public texture
	{
	public:
		/// Constructs a 1D texture array.
		/// @param image_view Image view.
		/// @param sampler Sampler object.
		inline texture_1d_array(std::shared_ptr<image_view_1d_array> image_view, std::shared_ptr<gl::sampler> sampler) noexcept:
			texture(sampler),
			m_image_view(image_view)
		{
		}

		/// Constructs a 1D texture array.
		constexpr texture_1d_array() noexcept = default;

		/// Destructs a 1D texture array.
		~texture_1d_array() override = default;

		/// Sets the image view.
		/// @param image_view Image view.
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

	/// 2D texture.
	class texture_2d: public texture
	{
	public:
		/// Constructs a 2D texture.
		/// @param image_view Image view.
		/// @param sampler Sampler object.
		inline texture_2d(std::shared_ptr<image_view_2d> image_view, std::shared_ptr<gl::sampler> sampler) noexcept:
			texture(sampler),
			m_image_view(image_view)
		{
		}

		/// Constructs a 2D texture.
		constexpr texture_2d() noexcept = default;

		/// Destructs a 2D texture.
		~texture_2d() override = default;

		/// Sets the image view.
		/// @param image_view Image view.
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

	/// 2D texture array.
	class texture_2d_array: public texture
	{
	public:
		/// Constructs a 2D texture array.
		/// @param image_view Image view.
		/// @param sampler Sampler object.
		inline texture_2d_array(std::shared_ptr<image_view_2d_array> image_view, std::shared_ptr<gl::sampler> sampler) noexcept:
			texture(sampler),
			m_image_view(image_view)
		{
		}

		/// Constructs a 2D texture array.
		constexpr texture_2d_array() noexcept = default;

		/// Destructs a 2D texture array.
		~texture_2d_array() override = default;

		/// Sets the image view.
		/// @param image_view Image view.
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

	/// 3D texture.
	class texture_3d: public texture
	{
	public:
		/// Constructs a 3D texture.
		/// @param image_view Image view.
		/// @param sampler Sampler object.
		inline texture_3d(std::shared_ptr<image_view_3d> image_view, std::shared_ptr<gl::sampler> sampler) noexcept:
			texture(sampler),
			m_image_view(image_view)
		{
		}

		/// Constructs a 3D texture.
		constexpr texture_3d() noexcept = default;

		/// Destructs a 3D texture.
		~texture_3d() override = default;

		/// Sets the image view.
		/// @param image_view Image view.
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

	/// Cube texture.
	class texture_cube: public texture
	{
	public:
		/// Constructs a cube texture.
		/// @param image_view Image view.
		/// @param sampler Sampler object.
		inline texture_cube(std::shared_ptr<image_view_cube> image_view, std::shared_ptr<gl::sampler> sampler) noexcept:
			texture(sampler),
			m_image_view(image_view)
		{
		}

		/// Constructs a cube texture.
		constexpr texture_cube() noexcept = default;

		/// Destructs a cube texture.
		~texture_cube() override = default;

		/// Sets the image view.
		/// @param image_view Image view.
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

	/// Cube texture array.
	class texture_cube_array: public texture
	{
	public:
		/// Constructs a cube texture array.
		/// @param image_view Image view.
		/// @param sampler Sampler object.
		inline texture_cube_array(std::shared_ptr<image_view_cube_array> image_view, std::shared_ptr<gl::sampler> sampler) noexcept:
			texture(sampler),
			m_image_view(image_view)
		{
		}

		/// Constructs a cube texture array.
		constexpr texture_cube_array() noexcept = default;

		/// Destructs a cube texture array.
		~texture_cube_array() override = default;

		/// Sets the image view.
		/// @param image_view Image view.
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
}
