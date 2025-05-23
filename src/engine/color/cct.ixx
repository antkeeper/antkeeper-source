// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.color.cct;
export import engine.math.vector;
import engine.color.ucs;
import engine.color.xyy;

export namespace engine::color
{
	/// @name Correlated color temperature (CCT)
	/// @{

	/// Calculates CIE 1960 UCS colorspace chromaticity coordinates given a correlated color temperature using Krystek's algorithm.
	/// @tparam T Scalar type.
	/// @param t Correlated color temperature, in Kelvin.
	/// @return CIE 1960 UCS colorspace chromaticity coordinates.
	/// @see Krystek, M. (1985), An algorithm to calculate correlated colour temperature. Color Res. Appl., 10: 38-40.
	template <class T>
	[[nodiscard]] math::vec2<T> cct_to_ucs(T t) noexcept
	{
		const T tt = t * t;
		return math::vec2<T>
		{
			(T{0.860117757} + T{1.54118254e-4} * t + T{1.28641212e-7} * tt) / (T{1} + T{8.42420235e-4} * t + T{7.08145163e-7} * tt),
			(T{0.317398726} + T{4.22806245e-5} * t + T{4.20481691e-8} * tt) / (T{1} - T{2.89741816e-5} * t + T{1.61456053e-7} * tt)
		};
	}

	/// Calculates CIE xyY colorspace chromaticity coordinates given a correlated color temperature using Krystek's algorithm.
	/// @tparam T Scalar type.
	/// @param t Correlated color temperature, in Kelvin.
	/// @return CIE xyY color with `Y = 1`.
	template <class T>
	[[nodiscard]] math::vec3<T> cct_to_xyy(T t)
	{
		return ucs_to_xyy(cct_to_ucs(t), T{1});
	}

	/// Calculates CIE XYZ colorspace chromaticity coordinates given a correlated color temperature using Krystek's algorithm.
	/// @tparam T Scalar type.
	/// @param t Correlated color temperature, in Kelvin.
	/// @return CIE XYZ color with `Y = 1`.
	template <class T>
	[[nodiscard]] math::vec3<T> cct_to_xyz(T t)
	{
		return xyy_to_xyz(cct_to_xyy(t));
	}
}
