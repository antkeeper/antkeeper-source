// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/constants.hpp>
#include <engine/math/quadrature.hpp>

namespace engine::physics::light
{
	/// Maximum luminous efficacy of an ideal monochromatic source, in lumen per watt.
	template <class T>
	inline constexpr T max_luminous_efficacy = T{683.002};

	/// Calculates the luminous efficiency of a light source.
	/// @param spd Unary function object that returns spectral radiance given a wavelength.
	/// @param lef Unary function object that returns luminous efficiency given a wavelength.
	/// @param first,last Range of sample wavelengths.
	/// @return Luminous efficiency, on `[0, 1]`.
	template <class T, class UnaryOp1, class UnaryOp2, class InputIt>
	[[nodiscard]] T luminous_efficiency(UnaryOp1 spd, UnaryOp2 lef, InputIt first, InputIt last)
	{
		auto spd_lef = [&spd, &lef](T x) -> T
			{
				return spd(x) * lef(x);
			};

		const T num = math::simpson(spd_lef, first, last);
		const T den = math::simpson(spd, first, last);

		return num / den;
	}

	/// Calculates luminous efficacy given luminous efficiency.
	/// @param efficiency Luminous efficiency, on `[0, 1]`.
	/// @return Luminous flux, in lumens.
	template <class T>
	[[nodiscard]] T luminous_efficacy(T efficiency) noexcept
	{
		return max_luminous_efficacy<T> *efficiency;
	}

	/// Converts watts (radiant flux) to lumens (luminous flux).
	/// @param radiant_flux Radiant flux, in watts.
	/// @param efficiency Luminous efficiency, on `[0, 1]`.
	/// @return Luminous flux, in lumens.
	template <class T>
	[[nodiscard]] T watts_to_lumens(T radiant_flux, T efficiency) noexcept
	{
		return radiant_flux * luminous_efficacy(efficiency);
	}
}
