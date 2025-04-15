// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.scene.spot_light;
export import engine.scene.light;
export import engine.math.vector;
import engine.math.constants;

export namespace engine::scene
{
	/// Directional cone light source.
	class spot_light: public light
	{
	public:
		/// Destructs a spot light.
		~spot_light() override = default;

		/// Returns light_type::spot.
		[[nodiscard]] inline light_type get_light_type() const noexcept override
		{
			return light_type::spot;
		}

		/// Sets the luminous flux of the spot light.
		/// @param luminous_flux Luminous flux, in *lm*.
		inline void set_luminous_flux(const math::fvec3& luminous_flux) noexcept
		{
			m_luminous_flux = luminous_flux;
		}

		/// Returns the luminous flux of the spot light, in *lm*.
		[[nodiscard]] inline const math::fvec3& get_luminous_flux() const noexcept
		{
			return m_luminous_flux;
		}

		/// Sets the spot light cutoff angles.
		/// @param cutoff Vector containing the inner and outer cutoff angles, as x and y, respectively.
		void set_cutoff(const math::fvec2& cutoff);

		/// Returns the direction vector.
		[[nodiscard]] inline const math::fvec3& get_direction() const noexcept
		{
			return m_direction;
		}

		/// Returns the spot light cutoff angles.
		[[nodiscard]] inline const math::fvec2& get_cutoff() const noexcept
		{
			return m_cutoff;
		}

		/// Returns the cosine of the spot light cutoff angles.
		[[nodiscard]] inline const math::fvec2& get_cosine_cutoff() const noexcept
		{
			return m_cosine_cutoff;
		}

	private:
		void transformed() override;

		math::fvec3 m_luminous_flux{0.0f, 0.0f, 0.0f};
		math::fvec3 m_direction{0.0f, 0.0f, -1.0f};
		math::fvec2 m_cutoff{math::pi<float>, math::pi<float>};
		math::fvec2 m_cosine_cutoff{-1.0f, -1.0f};
	};
}
