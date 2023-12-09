// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_NOISE_SIMPLEX_HPP
#define ANTKEEPER_NOISE_SIMPLEX_HPP

#include <engine/hash/make-uint.hpp>
#include <engine/hash/pcg.hpp>
#include <engine/math/vector.hpp>
#include <engine/math/functions.hpp>
#include <algorithm>

// export module noise.simplex;
// import hash.make_uint;
// import hash.pcg;
// import math.common;
// import math.vector;
// import <algorithm>;

namespace noise {

/**
 * Number of corners in an *n*-dimensional simplex lattice cell.
 *
 * @private
 */
template <std::size_t N>
constexpr std::size_t simplex_corner_count = std::size_t(2) << std::max<std::size_t>(0, N - 1);

/**
 * Number of edges in an *n*-dimensional simplex lattice cell.
 *
 * @private
 */
template <std::size_t N>
constexpr std::size_t simplex_edge_count = (N > 1) ? N * simplex_corner_count<N - 1> : 2;

/**
 * Returns the simplex lattice cell corner vector for a given dimension and index.
 *
 * @private
 */
template <class T, std::size_t N, std::size_t... I>
[[nodiscard]] constexpr math::vector<T, N> make_simplex_corner(std::size_t i, std::index_sequence<I...>)
{
	return {((i >> I) % 2) * T{2} - T{1}...};
}

/**
 * Builds an array of simplex lattice cell corner vectors for a given dimension.
 *
 * @private
 */
template <class T, std::size_t N, std::size_t... I>
[[nodiscard]] constexpr std::array<math::vector<T, N>, simplex_corner_count<N>> make_simplex_corners(std::index_sequence<I...>)
{
	return {make_simplex_corner<T, N>(I, std::make_index_sequence<N>{})...};
}

/**
 * Array of simplex lattice cell corner vectors for a given dimension.
 *
 * @private
 */
template <class T, std::size_t N>
constexpr auto simplex_corners = make_simplex_corners<T, N>(std::make_index_sequence<simplex_corner_count<N>>{});

/**
 * Returns the simplex lattice cell edge vector for a given dimension and index.
 *
 * @private
 */
template <class T, std::size_t N, std::size_t... I>
[[nodiscard]] constexpr math::vector<T, N> make_simplex_edge(std::size_t i, std::index_sequence<I...>)
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

/**
 * Builds an array of simplex lattice cell edge vectors for a given dimension.
 *
 * @private
 */
template <class T, std::size_t N, std::size_t... I>
[[nodiscard]] constexpr std::array<math::vector<T, N>, simplex_edge_count<N>> make_simplex_edges(std::index_sequence<I...>)
{
	if constexpr (N == 1)
		return std::array<math::vector<T, N>, simplex_edge_count<N>>{math::vector<T, N>{T{1}}, math::vector<T, N>{T{-1}}};
	else
		return {make_simplex_edge<T, N>(I, std::make_index_sequence<N>{})...};
}

/**
 * Array of simplex lattice cell edge vectors for a given dimension.
 *
 * @private
 */
template <class T, std::size_t N>
constexpr auto simplex_edges = make_simplex_edges<T, N>(std::make_index_sequence<simplex_edge_count<N>>{});

/**
 * *n*-dimensional simplex noise.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 *
 * @param position Input position.
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
[[nodiscard]] T simplex
(
	const math::vector<T, N>& position,
	math::vector<hash::make_uint_t<T>, N> (*hash)(const math::vector<T, N>&) = &hash::pcg<T, N>
)
{
	// Skewing (F) and unskewing (G) factors
	static const T f = (sqrt(static_cast<T>(N + 1)) - T{1}) / static_cast<T>(N);
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
	static const T corner_normalization = T{1} / ((static_cast<T>(N) / sqrt(static_cast<T>(N + 1))) * falloff(static_cast<T>(N) / (T{4} * static_cast<T>(N + 1))));
	
	// Adjust normalization factor for difference in length between corner gradient vectors and edge gradient vectors
	static const T edge_normalization = corner_normalization * (sqrt(static_cast<T>(N)) / length(simplex_edges<T, N>[0]));
	
	// Skew input position to get the origin vertex of the unit hypercube cell to which they belong
	const math::vector<T, N> origin_vertex = floor(position + sum(position) * f);
	
	// Displacement vector from origin vertex position to input position
	const math::vector<T, N> dx = position - origin_vertex + sum(origin_vertex) * g;
	
	// Find axis traversal order
	math::vector<std::size_t, N> axis_order;
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
	math::vector<T, N> current_vertex = origin_vertex;
	for (std::size_t i = 0; i <= N; ++i)
	{
		if (i)
			++current_vertex[axis_order[i - 1]];
		
		// Calculate displacement vector from current vertex to input position
		const math::vector<T, N> d = dx - (current_vertex - origin_vertex) + g * static_cast<T>(i);
		
		// Calculate falloff
		T t = falloff(sqr_length(d));
		if (t > T{0})
		{
			const hash::make_uint_t<T> gradient_index = hash(current_vertex)[0] % simplex_edges<T, N>.size();
			const math::vector<T, N>& gradient = simplex_edges<T, N>[gradient_index];
			
			n += dot(d, gradient) * t;
		}
	}
	
	// Normalize value
	return n * edge_normalization;
}

} // namespace noise

#endif // ANTKEEPER_NOISE_SIMPLEX_HPP
