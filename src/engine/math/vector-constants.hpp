// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/vector-type.hpp>
#include <engine/math/constants.hpp>
#include <engine/utility/sized-types.hpp>

/// @private
template <class T, engine::usize N>
[[nodiscard]] consteval engine::math::vector<T, N> fill_vector(const T& value)
{
	engine::math::vector<T, N> v;
	v.fill(value);
	return v;
}

namespace engine::math::inline constants
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
