// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/simd/vector-type.hpp>
#include <engine/math/constants.hpp>
#include <engine/utility/sized-types.hpp>

namespace engine::math::inline constants
{
	/// @name Vector constants
	/// @{

	/// Vector of zeros.
	template <class T, usize N>
	const simd::vec<T, N> zero<simd::vec<T, N>> = {};

	/// Vector of ones.
	template <class T, usize N>
	const simd::vec<T, N> one<simd::vec<T, N>> = simd::vec<T, N>(one<T>);

	/// Vector of infinities.
	template <class T, usize N>
	const simd::vec<T, N> inf<simd::vec<T, N>> = simd::vec<T, N>(inf<T>);

	/// @}
}
