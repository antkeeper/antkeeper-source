// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.physics.light.phase;
import engine.math.constants;
import engine.math.functions;

/// Light-scattering phase functions.
export namespace engine::physics::light::phase
{
	/// Cornette-Shanks phase function.
	/// @param mu Cosine of the angle between the light and view directions.
	/// @param g Asymmetry factor, on [-1, 1]. Positive values cause forward scattering, negative values cause back scattering.
	template <class T>
	[[nodiscard]] T cornette_shanks(T mu, T g)
	{
		inline constexpr T k = T{3} / (T{8} * math::pi<T>);
		const T gg = g * g;
		const T num = (T{1} - gg) * (T{1} + mu * mu);
		const T den = (T{2} + gg) * math::pow(T{1} + gg - T{2} * g * mu, T{1.5});
		return k * num / den;
	}

	/// Henyey–Greenstein phase function.
	/// @param mu Cosine of the angle between the light and view directions.
	/// @param g Asymmetry factor, on `[-1, 1]`. Positive values cause forward scattering, negative values cause back scattering.
	/// @see http://www.pbr-book.org/3ed-2018/Volume_Scattering/Phase_Functions.html
	template <class T>
	[[nodiscard]] T henyey_greenstein(T mu, T g)
	{
		const T gg = g * g;
		return (T{1} - gg) / (T{4} * math::pi<T> * math::pow(T{1} + gg - T{2} * g * mu, T{1.5}));
	}

	/// Isotropic phase function.
	template <class T>
	[[nodiscard]] consteval T isotropic()
	{
		return T{1} / (T{4} * math::pi<T>);
	}

	/// Rayleigh phase function.
	/// @param mu Cosine of the angle between the light and view directions.
	template <class T>
	[[nodiscard]] T rayleigh(T mu) noexcept
	{
		inline constexpr T k = T{3} / (T{16} * math::pi<T>);
		return k * (T{1} + mu * mu);
	}
}
