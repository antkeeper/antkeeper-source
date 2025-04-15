// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.sampler;
export import engine.gl.compare_op;
export import engine.utility.sized_types;
export import <array>;

namespace engine::gl::opengl
{
	class shader_texture_1d;
	class shader_texture_2d;
	class shader_texture_3d;
	class shader_texture_cube;
};

export namespace engine::gl
{
	class pipeline;

	/// Filters used for texture lookups.
	enum class sampler_filter: u8
	{
		/// Nearest filtering.
		nearest,

		/// Linear filtering.
		linear
	};

	/// Mipmap modes used for texture lookups.
	enum class sampler_mipmap_mode: u8
	{
		/// Nearest filtering.
		nearest,

		/// Linear filtering.
		linear
	};

	/// Behaviors of sampling with texture coordinates outside an image.
	enum class sampler_address_mode: u8
	{
		/// Repeat wrap mode.
		repeat,

		/// Mirrored repeat wrap mode.
		mirrored_repeat,

		/// Clamp to edge wrap mode.
		clamp_to_edge,

		/// Clamp to border wrap mode.
		clamp_to_border,

		/// Mirror clamp to edge wrap mode.
		mirror_clamp_to_edge
	};

	/// Sampler object.
	class sampler
	{
	public:
		/// Constructs a sampler object.
		/// @param mag_filter Magnification filter to apply to lookups.
		/// @param min_filter Minification filter to apply to lookups.
		/// @param mipmap_mode Mipmap filter to apply to lookups.
		/// @param address_mode_u Addressing mode for U-coordinates outside `[0, 1)`.
		/// @param address_mode_v Addressing mode for V-coordinates outside `[0, 1)`.
		/// @param address_mode_w Addressing mode for W-coordinates outside `[0, 1)`.
		/// @param mip_lod_bias Bias to be added to mipmap LOD calculation.
		/// @param max_anisotropy Anisotropy clamp value.
		/// @param compare_enabled `true` to enable comparison against a reference value during lookups, `false` otherwise.
		/// @param compare_op Comparison operator to apply to fetched data, if compare is enabled.
		/// @param min_lod Minimum clamp value of the computed LOD.
		/// @param max_lod Maximum clamp value of the computed LOD.
		/// @param border_color Border color used for texture lookups.
		explicit sampler
		(
			sampler_filter mag_filter = sampler_filter::linear,
			sampler_filter min_filter = sampler_filter::nearest,
			sampler_mipmap_mode mipmap_mode = sampler_mipmap_mode::linear,
			sampler_address_mode address_mode_u = sampler_address_mode::repeat,
			sampler_address_mode address_mode_v = sampler_address_mode::repeat,
			sampler_address_mode address_mode_w = sampler_address_mode::repeat,
			float mip_lod_bias = 0.0f,
			float max_anisotropy = 0.0f,
			bool compare_enabled = false,
			gl::compare_op compare_op = gl::compare_op::less,
			float min_lod = -1000.0f,
			float max_lod = 1000.0f,
			const std::array<float, 4>& border_color = {0.0f, 0.0f, 0.0f, 0.0f}
		);

		/// Destroys a sampler object.
		~sampler();

		sampler(const sampler&) = delete;
		sampler(sampler&&) = delete;
		sampler& operator=(const sampler&) = delete;
		sampler& operator=(sampler&&) = delete;

		/// Sets the magnification filter to apply to lookups.
		/// @param filter Magnification filter to apply to lookups.
		void set_mag_filter(sampler_filter filter);

		/// Sets the minification filter to apply to lookups.
		/// @param filter Minification filter to apply to lookups.
		void set_min_filter(sampler_filter filter);

		/// Sets the mipmap filter to apply to lookups.
		/// @param mode Mipmap filter to apply to lookups.
		void set_mipmap_mode(sampler_mipmap_mode mode);

		/// Sets the addressing mode for U-coordinates outside `[0, 1)`.
		/// @param mode Addressing mode for U-coordinates outside `[0, 1)`.
		void set_address_mode_u(sampler_address_mode mode);

		/// Sets the addressing mode for V-coordinates outside `[0, 1)`.
		/// @param mode Addressing mode for V-coordinates outside `[0, 1)`.
		void set_address_mode_v(sampler_address_mode mode);

		/// Sets the addressing mode for W-coordinates outside `[0, 1)`.
		/// @param mode Addressing mode for W-coordinates outside `[0, 1)`.
		void set_address_mode_w(sampler_address_mode mode);

		/// Sets the bias to be added to mipmap LOD calculation.
		/// @param bias Bias to be added to mipmap LOD calculation.
		void set_mip_lod_bias(float bias);

		/// Sets the anisotropy clamp value.
		/// @param anisotropy Anisotropy clamp value.
		void set_max_anisotropy(float anisotropy);

		/// Enables or disables a comparison against a reference value during lookups.
		/// @param enabled `true` to enable comparison against a reference value during lookups, `false` otherwise.
		void set_compare_enabled(bool enabled);

		/// Sets the comparison operator to apply to fetched data, if compare is enabled.
		/// @param op Comparison operator to apply to fetched data, if compare is enabled.
		void set_compare_op(gl::compare_op op);

		/// Sets the minimum clamp value of the computed LOD.
		/// @param lod Minimum clamp value of the computed LOD.
		void set_min_lod(float lod);

		/// Sets the maximum clamp value of the computed LOD.
		/// @param lod Maximum clamp value of the computed LOD.
		void set_max_lod(float lod);

		/// Sets the border color used for texture lookups.
		/// @param color Border color used for texture lookups.
		void set_border_color(const std::array<float, 4>& color);

		/// Returns the magnification filter to apply to lookups.
		[[nodiscard]] inline constexpr sampler_filter get_mag_filter() const noexcept
		{
			return m_mag_filter;
		}

		/// Returns the minification filter to apply to lookups.
		[[nodiscard]] inline constexpr sampler_filter get_min_filter() const noexcept
		{
			return m_min_filter;
		}

		/// Returns the mipmap filter to apply to lookups.
		[[nodiscard]] inline constexpr sampler_mipmap_mode get_mipmap_mode() const noexcept
		{
			return m_mipmap_mode;
		}

		/// Returns the addressing mode for U-coordinates outside `[0, 1)`.
		[[nodiscard]] inline constexpr sampler_address_mode get_address_mode_u() const noexcept
		{
			return m_address_mode_u;
		}

		/// Returns the addressing mode for V-coordinates outside `[0, 1)`.
		[[nodiscard]] inline constexpr sampler_address_mode get_address_mode_v() const noexcept
		{
			return m_address_mode_v;
		}

		/// Returns the addressing mode for W-coordinates outside `[0, 1)`.
		[[nodiscard]] inline constexpr sampler_address_mode get_address_mode_w() const noexcept
		{
			return m_address_mode_w;
		}

		/// Returns the bias to be added to mipmap LOD calculation.
		[[nodiscard]] inline constexpr float get_mip_lod_bias() const noexcept
		{
			return m_mip_lod_bias;
		}

		/// Returns the anisotropy clamp value.
		[[nodiscard]] inline constexpr float get_max_anisotropy() const noexcept
		{
			return m_max_anisotropy;
		}

		/// Returns `true` if comparison against a reference value during lookups is enabled, `false` otherwise. 
		[[nodiscard]] inline constexpr float get_compare_enabled() const noexcept
		{
			return m_compare_enabled;
		}

		/// Returns the comparison operator to apply to fetched data, if compare is enabled.
		[[nodiscard]] inline constexpr compare_op get_compare_op() const noexcept
		{
			return m_compare_op;
		}

		/// Returns the minimum clamp value of the computed LOD.
		[[nodiscard]] inline constexpr float get_min_lod() const noexcept
		{
			return m_min_lod;
		}

		/// Returns the maximum clamp value of the computed LOD.
		[[nodiscard]] inline constexpr float get_max_lod() const noexcept
		{
			return m_max_lod;
		}

		/// Returns the border color used for texture lookups.
		[[nodiscard]] inline constexpr const std::array<float, 4>& get_border_color() const noexcept
		{
			return m_border_color;
		}

	private:
		friend class pipeline;
		friend class opengl::shader_texture_1d;
		friend class opengl::shader_texture_2d;
		friend class opengl::shader_texture_3d;
		friend class opengl::shader_texture_cube;

		unsigned int m_gl_named_sampler{0};

		sampler_filter m_mag_filter{sampler_filter::linear};
		sampler_filter m_min_filter{sampler_filter::nearest};
		sampler_mipmap_mode m_mipmap_mode{sampler_mipmap_mode::linear};
		sampler_address_mode m_address_mode_u{sampler_address_mode::repeat};
		sampler_address_mode m_address_mode_v{sampler_address_mode::repeat};
		sampler_address_mode m_address_mode_w{sampler_address_mode::repeat};
		float m_mip_lod_bias{0.0f};
		float m_max_anisotropy{0.0f};
		bool m_compare_enabled{false};
		gl::compare_op m_compare_op{gl::compare_op::less};
		float m_min_lod{-1000.0f};
		float m_max_lod{1000.0f};
		std::array<float, 4> m_border_color{0.0f, 0.0f, 0.0f, 0.0f};
	};
}
