// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/functions.hpp>

namespace engine::physics::light
{
	/// Fitted Gaussian approximation to the CIE 1931 standard observer photopic luminosity function.
	/// @param lambda Wavelength of light, in nanometers.
	/// @return Luminous efficiency, on `[0, 1]`.
	/// @see Wyman, C., Sloan, P.J., & Shirley, P. (2013). Simple Analytic Approximations to the CIE XYZ Color Matching Functions.
	template <class T>
	[[nodiscard]] T photopic_luminosity(T lambda)
	{
		const T t0 = (lambda - T{568.8}) * ((lambda < T{568.8}) ? T{0.0213} : T{0.0247});
		const T t1 = (lambda - T{530.9}) * ((lambda < T{530.9}) ? T{0.0613} : T{0.0322});
		const T y0 = T{0.821} * math::exp(T{-0.5} * t0 * t0);
		const T y1 = T{0.286} * math::exp(T{-0.5} * t1 * t1);
		return y0 + y1;
	}
}
