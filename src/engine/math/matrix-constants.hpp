// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_MATH_MATRIX_CONSTANTS_HPP
#define ANTKEEPER_MATH_MATRIX_CONSTANTS_HPP

#include <engine/math/matrix-type.hpp>
#include <engine/math/numbers.hpp>
#include <cstddef>
#include <utility>

// export module math.matrix:constants;
// import :type;
// import math.numbers;
// import <cstddef>;
// import <utility>;

namespace math
{
	/// @private
	template <class T, std::size_t N, std::size_t M>
	[[nodiscard]] consteval matrix<T, N, M> fill_matrix(const T& value) noexcept
	{
		vector<T, M> v;
		v.fill(value);
		matrix<T, N, M> m;
		m.fill(v);
		return m;
	}
	
	/// @private
	template <class T, std::size_t N, std::size_t M, std::size_t... I>
	[[nodiscard]] consteval vector<T, M> identity_matrix_column(std::size_t i, std::index_sequence<I...>) noexcept
	{
		return {(I == i ? T{1} : T{0}) ...};
	}
	
	/// @private
	template <class T, std::size_t N, std::size_t M, std::size_t... I>
	[[nodiscard]] consteval matrix<T, N, M> identity_matrix(std::index_sequence<I...>) noexcept
	{
		return {identity_matrix_column<T, N, M>(I, std::make_index_sequence<M>{}) ...};
	}
	
	/// Zero matrix.
	template <class T, std::size_t N, std::size_t M>
	inline constexpr matrix<T, N, M> zero<matrix<T, N, M>> = {};
	
	/// Matrix of ones.
	template <class T, std::size_t N, std::size_t M>
	inline constexpr matrix<T, N, M> one<matrix<T, N, M>> = fill_matrix<T, N, M>(one<T>);
	
	/// Matrix of infinities.
	template <class T, std::size_t N, std::size_t M>
	inline constexpr matrix<T, N, M> inf<matrix<T, N, M>> = fill_matrix<T, N, M>(inf<T>);
	
	/// Identity matrix.
	template <class T, std::size_t N, std::size_t M>
	inline constexpr matrix<T, N, M> identity<matrix<T, N, M>> = identity_matrix<T, N, M>(std::make_index_sequence<N>{});
}

#endif // ANTKEEPER_MATH_MATRIX_CONSTANTS_HPP
