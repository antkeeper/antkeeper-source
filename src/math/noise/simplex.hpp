/*
 * Copyright (C) 2021  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANTKEEPER_MATH_NOISE_SIMPLEX_HPP
#define ANTKEEPER_MATH_NOISE_SIMPLEX_HPP

#include "math/vector.hpp"
#include <algorithm>
#include <utility>

namespace math {
namespace noise {

/// @private
/// @{

/// Number of corners in an *n*-dimensional simplex lattice cell.
template <std::size_t N>
constexpr std::size_t simplex_corner_count = std::size_t(2) << std::max<std::size_t>(0, N - 1);

/// Number of edges in an *n*-dimensional simplex lattice cell.
template <std::size_t N>
constexpr std::size_t simplex_edge_count = (N > 1) ? N * simplex_corner_count<N - 1> : 2;

/// Returns the simplex lattice cell corner vector for a given dimension and index.
template <class T, std::size_t N, std::size_t... I>
constexpr vector<T, N> make_simplex_corner(std::size_t i, std::index_sequence<I...>)
{
	return {((i >> I) % 2) * T{2} - T{1}...};
}

/// Builds an array of simplex lattice cell corner vectors for a given dimension.
template <class T, std::size_t N, std::size_t... I>
constexpr std::array<vector<T, N>, simplex_corner_count<N>> make_simplex_corners(std::index_sequence<I...>)
{
	return {make_simplex_corner<T, N>(I, std::make_index_sequence<N>{})...};
}

/// Array of simplex lattice cell corner vectors for a given dimension.
template <class T, std::size_t N>
constexpr auto simplex_corners = make_simplex_corners<T, N>(std::make_index_sequence<simplex_corner_count<N>>{});

/// Returns the simplex lattice cell edge vector for a given dimension and index.
template <class T, std::size_t N, std::size_t... I>
constexpr vector<T, N> make_simplex_edge(std::size_t i, std::index_sequence<I...>)
{
	std::size_t j = i / (simplex_edge_count<N> / N);
	
	return
	{
		I < j ?
			simplex_corners<T, N - 1>[i % simplex_corner_count<N - 1>][I]
		:
		I > j ?
			simplex_corners<T, N - 1>[i % simplex_corner_count<N - 1>][I - 1]
		:
			T{0}
		...
	};
}

/// Builds an array of simplex lattice cell edge vectors for a given dimension.
template <class T, std::size_t N, std::size_t... I>
constexpr std::array<vector<T, N>, simplex_edge_count<N>> make_simplex_edges(std::index_sequence<I...>)
{
	if constexpr (N == 1)
		return std::array<vector<T, N>, simplex_edge_count<N>>{vector<T, N>{T{1}}, vector<T, N>{T{-1}}};
	else
		return {make_simplex_edge<T, N>(I, std::make_index_sequence<N>{})...};
}

/// Array of simplex lattice cell edge vectors for a given dimension.
template <class T, std::size_t N>
constexpr auto simplex_edges = make_simplex_edges<T, N>(std::make_index_sequence<simplex_edge_count<N>>{});

/// @}

/**
 * *n*-dimensional simplex noise.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 *
 * @param x Input vector.
 * @param hash Hash function.
 *
 * @return Noise value, on `[-1, 1]`.
 *
 * @see https://en.wikipedia.org/wiki/Simplex_noise
 * @see https://catlikecoding.com/unity/tutorials/pseudorandom-noise/simplex-noise/
 * @see https://briansharpe.wordpress.com/2012/01/13/simplex-noise/
 * @see https://briansharpe.wordpress.com/2011/11/14/two-useful-interpolation-functions-for-noise-development/
 * @see https://math.stackexchange.com/questions/474638/radius-and-amplitude-of-kernel-for-simplex-noise/1901116
 */
template <class T, std::size_t N>
T simplex(const vector<T, N>& x, std::uint32_t (*hash)(const vector<T, N>&))
{
	// Skewing (F) and unskewing (G) factors
	static const T f = (std::sqrt(static_cast<T>(N + 1)) - T{1}) / static_cast<T>(N);
	static const T g = f / (T{1} + f * static_cast<T>(N));
	
	// Kernel radius set to the height of the equilateral triangle, `sqrt(0.5)`
	constexpr T sqr_kernel_radius = T{0.5};
	
	/**
	 * C2-continuous kernel falloff function.
	 *
	 * @param sqr_distance Squared distance from the kernel center.
	 *
	 * @return Kernel strength at the given distance.
	 */
	auto falloff = [sqr_kernel_radius](T sqr_distance) constexpr
	{
		sqr_distance = sqr_kernel_radius - sqr_distance;
		return sqr_distance * sqr_distance * sqr_distance;
	};
	
	// Normalization factor when using corner gradient vectors
	// @see https://math.stackexchange.com/questions/474638/radius-and-amplitude-of-kernel-for-simplex-noise/1901116
	static const T corner_normalization = T{1} / ((static_cast<T>(N) / std::sqrt(static_cast<T>(N + 1))) * falloff(static_cast<T>(N) / (T{4} * static_cast<T>(N + 1))));
	
	// Adjust normalization factor for difference in length between corner gradient vectors and edge gradient vectors
	static const T edge_normalization = corner_normalization * (std::sqrt(static_cast<T>(N)) / math::length(simplex_edges<T, N>[0]));
	
	// Skew input position to get the origin vertex of the unit hypercube cell to which they belong
	const vector<T, N> origin_vertex = math::floor(x + math::sum(x) * f);
	
	// Displacement vector from origin vertex position to input position
	const vector<T, N> dx = x - origin_vertex + math::sum(origin_vertex) * g;
	
	// Find axis traversal order
	vector<std::size_t, N> axis_order;
	for (std::size_t i = 0; i < N; ++i)
		axis_order[i] = i;
	std::sort
	(
		axis_order.begin(),
		axis_order.end(),
		[&dx](std::size_t lhs, std::size_t rhs)
		{
			return dx[lhs] > dx[rhs];
		}
	);
	
	T n = T{0};
	vector<T, N> current_vertex = origin_vertex;
	for (std::size_t i = 0; i <= N; ++i)
	{
		if (i)
			++current_vertex[axis_order[i - 1]];
		
		// Calculate displacement vector from current vertex to input position
		const vector<T, N> d = dx - (current_vertex - origin_vertex) + g * static_cast<T>(i);
		
		// Calculate falloff
		T t = falloff(math::length_squared(d));
		if (t > T{0})
		{
			const auto gradient_index = hash(current_vertex) % simplex_edges<T, N>.size();
			const vector<T, N>& gradient = simplex_edges<T, N>[gradient_index];
			
			n += math::dot(d, gradient) * t;
		}
	}
	
	// Normalize value
	return n * edge_normalization;
}

} // namespace noise
} // namespace math

#endif // ANTKEEPER_MATH_NOISE_SIMPLEX_HPP
