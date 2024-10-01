// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_SCENE_SPOT_LIGHT_HPP
#define ANTKEEPER_SCENE_SPOT_LIGHT_HPP

#include <engine/scene/light.hpp>
#include <engine/math/constants.hpp>
#include <engine/math/vector.hpp>

namespace scene {

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

} // namespace scene

#endif // ANTKEEPER_SCENE_SPOT_LIGHT_HPP

