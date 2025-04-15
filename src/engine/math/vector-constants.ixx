// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.math.vector:constants;
import :type;
import engine.math.constants;
import engine.utility.sized_types;

/// @private
template <class T, engine::usize N>
[[nodiscard]] consteval engine::math::vector<T, N> fill_vector(const T& value)
{
	engine::math::vector<T, N> v;
	v.fill(value);
	return v;
}

export namespace engine::math::inline constants
{
	/// @name Vector constants
	/// @{

	/// Vector of zeros.
	template <class T, usize N>
	inline constexpr vector<T, N> zero<vector<T, N>> = {};

	/// Vector of ones.
	template <class T, usize N>
	inline constexpr vector<T, N> one<vector<T, N>> = fill_vector<T, N>(one<T>);

	/// Vector of infinities.
	template <class T, usize N>
	inline constexpr vector<T, N> inf<vector<T, N>> = fill_vector<T, N>(inf<T>);

	/// @}
}
