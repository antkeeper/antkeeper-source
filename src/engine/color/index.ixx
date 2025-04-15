// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.color.index;

export namespace engine::color
{
	/// @name Color indices
	/// @{

	/// Approximates the temperature of a star, given its B-V index.
	/// @tparam T Scalar type.
	/// @param bv B-V index.
	/// @return Correlated color temperature, in Kelvin.
	/// @see Ballesteros, F. J. (2012). "New insights into black bodies". EPL 97 (2012) 34008.
	template <class T>
	[[nodiscard]] constexpr T bv_to_cct(T bv) noexcept
	{
		return T{4600} * (T{1} / (T{0.92} * bv + T{1.7}) + T{1} / (T{0.92} * bv + T{0.62}));
	}

	/// @}
}
