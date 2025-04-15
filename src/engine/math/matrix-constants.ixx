// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.math.matrix:constants;
import :type;
import engine.math.constants;
import engine.utility.sized_types;
import <utility>;

using namespace engine::sized_types;
using namespace engine::math::types;

/// @private
template <class T, usize N, usize M>
[[nodiscard]] consteval matrix<T, N, M> fill_matrix(const T& value)
{
	matrix<T, N, M> m;
	m.fill(value);
	return m;
}

/// @private
template <class T, usize N, usize M, usize... I>
[[nodiscard]] consteval vector<T, M> identity_matrix_column(usize i, std::index_sequence<I...>)
{
	return {(I == i ? T{1} : T{0}) ...};
}

/// @private
template <class T, usize N, usize M, usize... I>
[[nodiscard]] consteval matrix<T, N, M> identity_matrix(std::index_sequence<I...>)
{
	return {identity_matrix_column<T, N, M>(I, std::make_index_sequence<M>{}) ...};
}

export namespace engine::math::inline constants
{
	/// @name Matrix constants
	/// @{

	/// Zero matrix.
	template <class T, usize N, usize M>
	inline constexpr matrix<T, N, M> zero<matrix<T, N, M>> = {};

	/// Matrix of ones.
	template <class T, usize N, usize M>
	inline constexpr matrix<T, N, M> one<matrix<T, N, M>> = fill_matrix<T, N, M>(one<T>);

	/// Matrix of infinities.
	template <class T, usize N, usize M>
	inline constexpr matrix<T, N, M> inf<matrix<T, N, M>> = fill_matrix<T, N, M>(inf<T>);

	/// Identity matrix.
	template <class T, usize N, usize M>
	inline constexpr matrix<T, N, M> identity<matrix<T, N, M>> = identity_matrix<T, N, M>(std::make_index_sequence<N>{});

	/// @}
}
