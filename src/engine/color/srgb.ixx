// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.color.srgb;
export import engine.color.rgb;
export import engine.math.vector;
import engine.math.functions;

export namespace engine::color
{
	/// @name sRGB color space
	/// @{

	/// sRGB Electro-Optical Transfer Function (EOTF).
	/// @tparam T Scalar type.
	/// @param x Non-linear sRGB signal.
	/// @return Linear sRGB tristimulus.
	template <class T>
	[[nodiscard]] math::vec3<T> srgb_eotf(const math::vec3<T>& x)
	{
		auto f = [](T x) -> T
		{
			return x <= T{12.92} * T{0.0031308} ? x / T{12.92} : math::pow((x + T{0.055}) / T{1.055}, T{2.4});
		};
	
		return {f(x[0]), f(x[1]), f(x[2])};
	}

	/// sRGB inverse EOTF.
	/// @tparam T Scalar type.
	/// @param x Linear sRGB tristimulus.
	/// @return Non-linear sRGB signal.
	template <class T>
	[[nodiscard]] math::vec3<T> srgb_inverse_eotf(const math::vec3<T>& x)
	{
		auto f = [](T x) -> T
		{
			return x <= T{0.0031308} ? T{12.92} * x : T{1.055} * math::pow(x, T{1.0 / 2.4}) - T{0.055};
		};
	
		return {f(x[0]), f(x[1]), f(x[2])};
	}

	/// sRGB color space.
	/// @tparam T Scalar type.
	/// @see IEC 61966-2-1:1999
	template <class T>
	inline constexpr rgb_color_space<T> srgb
	{
		{T{0.6400}, T{0.3300}},
		{T{0.3000}, T{0.6000}},
		{T{0.1500}, T{0.0600}},
		{T{0.3127}, T{0.3290}},
		&srgb_inverse_eotf<T>,
		&srgb_eotf<T>
	};

	/// @}
}
