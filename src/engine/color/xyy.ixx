// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.color.xyy;
export import engine.math.vector;

export namespace engine::color
{
	/// @name CIE xyY color space.
	/// @{

	/// Returns the luminance of a CIE xyY color.
	/// @tparam T Scalar type.
	/// @param x CIE xyY color.
	/// @return return Luminance of @p x.
	template <class T>
	[[nodiscard]] inline constexpr T xyy_to_luminance(const math::vec3<T>& x) noexcept
	{
		return x[2];
	}

	/// Transforms a CIE xyY color into the CIE 1960 UCS colorspace.
	/// @tparam T Scalar type.
	/// @param x CIE xyY color.
	/// @return CIE 1960 UCS color.
	template <class T>
	[[nodiscard]] constexpr math::vec2<T> xyy_to_ucs(const math::vec3<T>& x) noexcept
	{
		const T d = (T{1} / (T{-2} * x[0] + T{12} * x[1] + T{3}));
		return {(T{4} * x[0]) * d, (T{6} * x[1]) * d};
	}

	/// Transforms a CIE xyY color into the CIE XYZ colorspace.
	/// @tparam T Scalar type.
	/// @param x CIE xyY color.
	/// @return CIE XYZ color.
	template <class T>
	[[nodiscard]] constexpr math::vec3<T> xyy_to_xyz(const math::vec3<T>& x) noexcept
	{
		return {(x[0] * x[2]) / x[1], x[2], ((T{1} - x[0] - x[1]) * x[2]) / x[1]};
	}

	/// @}
}
