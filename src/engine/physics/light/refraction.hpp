// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/functions.hpp>

/// Index of refraction formulas.
namespace engine::physics::light::ior
{
	/// Two-term form of Cauchy's transmission equation.
	/// @param lambda Wavelength of light, in micrometers.
	/// @param a First coefficient of the medium.
	/// @param b Second coefficient of the medium.
	/// @return Refractive index.
	/// @see https://en.wikipedia.org/wiki/Cauchy%27s_equation
	template <class T>
	[[nodiscard]] T cauchy(T lambda, T a, T b) noexcept
	{
		return a + b / (lambda * lambda);
	}

	/// Three-term form of Cauchy's transmission equation.
	/// @param lambda Wavelength of light, in micrometers.
	/// @param a First coefficient.
	/// @param b Second coefficient.
	/// @param c Third coefficient.
	/// @return Refractive index.
	/// @see https://en.wikipedia.org/wiki/Cauchy%27s_equation
	template <class T>
	[[nodiscard]] T cauchy(T lambda, T a, T b, T c) noexcept
	{
		const T lambda2 = lambda * lambda;
		return a + b / lambda2 + c / (lambda2 * lambda2);
	}

	/// Two-term form of the Sellmeier equation.
	/// @param lambda Wavelength of light, in micrometers.
	/// @param b1 B1 coefficient.
	/// @param c1 C1 coefficient.
	/// @param b2 B2 coefficient.
	/// @param c2 C2 coefficient.
	/// @return Refractive index.
	/// @see https://en.wikipedia.org/wiki/Sellmeier_equation
	template <class T>
	[[nodiscard]] T sellmeier(T lambda, T b1, T c1, T b2, T c2)
	{
		const T lambda2 = lambda * lambda;
		const T t1 = (b1 * lambda2) / (lambda2 - c1);
		const T t2 = (b2 * lambda2) / (lambda2 - c2);
		return math::sqrt(T{1} + t1 + t2);
	}

	/// Three-term form of the Sellmeier equation.
	/// @param lambda Wavelength of light, in micrometers.
	/// @param b1 B1 coefficient.
	/// @param c1 C1 coefficient.
	/// @param b2 B2 coefficient.
	/// @param c2 C2 coefficient.
	/// @param b3 B3 coefficient.
	/// @param c3 C3 coefficient.
	/// @return Refractive index.
	/// @see https://en.wikipedia.org/wiki/Sellmeier_equation
	template <class T>
	[[nodiscard]] T sellmeier(T lambda, T b1, T c1, T b2, T c2, T b3, T c3)
	{
		const T lambda2 = lambda * lambda;
		const T t1 = (b1 * lambda2) / (lambda2 - c1);
		const T t2 = (b2 * lambda2) / (lambda2 - c2);
		const T t3 = (b3 * lambda2) / (lambda2 - c3);
		return math::sqrt(T{1} + t1 + t2 + t3);
	}
}
