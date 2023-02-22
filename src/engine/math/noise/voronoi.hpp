/*
 * Copyright (C) 2023  Christopher J. Howard
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

#ifndef ANTKEEPER_MATH_NOISE_VORONOI_HPP
#define ANTKEEPER_MATH_NOISE_VORONOI_HPP

#include <engine/math/vector.hpp>
#include <engine/math/hash/make-uint.hpp>
#include <engine/math/hash/pcg.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <tuple>
#include <limits>
#include <utility>

namespace math {
namespace noise {

/// Voronoi functions.
namespace voronoi {

/**
 * Number of Voronoi cells to search.
 *
 * @tparam N Number of dimensions.
 *
 * @private
 */
template <std::size_t N>
constexpr std::size_t kernel_size = 4 << std::max<std::size_t>(0, (2 * (N - 1)));

/**
 * Generates an kernel offset vector for a given index.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 * @tparam I Index sequence.
 *
 * @param i Index of a kernel offset vector.
 *
 * @return Kernel offset vector.
 *
 * @private
 */
template <class T, std::size_t N, std::size_t... I>
[[nodiscard]] constexpr vector<T, N> kernel_offset(std::size_t i, std::index_sequence<I...>)
{
	return {static_cast<T>((I ? (i / (2 << std::max<std::size_t>(0, 2 * I - 1))) : i) % 4)...};
}

/**
 * Generates a Voronoi search kernel.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 * @tparam I Index sequence.
 *
 * @return Voronoi search kernel.
 *
 * @private
 */
template <class T, std::size_t N, std::size_t... I>
[[nodiscard]] constexpr std::array<vector<T, N>, kernel_size<N>> generate_kernel(std::index_sequence<I...>)
{
	return {kernel_offset<T, N>(I, std::make_index_sequence<N>{})...};
}

/**
 * *n*-dimensional search kernel.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 *
 * @private
 */
template <class T, std::size_t N>
constexpr auto kernel = generate_kernel<T, N>(std::make_index_sequence<kernel_size<N>>{});

/**
 * Finds the Voronoi cell (F1) containing the input position.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 *
 * @param position Input position.
 * @param randomness Degree of randomness, on `[0, 1]`.
 * @param tiling Distance at which the Voronoi pattern should repeat. A value of `0` indicates no repetition.
 * @param hash Hash function.
 *
 * @return Tuple containing the square Euclidean distance from @p position to the F1 cell, the displacement vector from the input position to the F1 cell center, and a hash value indicating the ID of the F1 cell.
 */
template <class T, std::size_t N>
[[nodiscard]] std::tuple
<
	// F1 square distance to center
	T,
	
	// F1 position to center displacement
	vector<T, N>,
	
	// F1 hash
	hash::make_uint_t<T>
>
f1
(
	const vector<T, N>& position,
	T randomness = T{1},
	const vector<T, N>& tiling = vector<T, N>::zero(),
	vector<hash::make_uint_t<T>, N> (*hash)(const vector<T, N>&) = &hash::pcg<T, N>
)
{
	// Calculate factor which scales hash value onto `[0, 1]`, modulated by desired randomness
	T hash_scale = (T{1} / static_cast<T>(std::numeric_limits<hash::make_uint_t<T>>::max())) * randomness;
	
	// Get integer and fractional parts
    vector<T, N> position_i = floor(position - T{1.5});
    vector<T, N> position_f = position - position_i;
	
    // Find the F1 cell 
	T f1_sqr_distance = std::numeric_limits<T>::infinity();
	vector<T, N> f1_displacement;
	hash::make_uint_t<T> f1_hash;
	for (std::size_t i = 0; i < kernel_size<N>; ++i)
	{
		// Get kernel offset for current cell
		const vector<T, N>& offset_i = kernel<T, N>[i];
		
		// Calculate hash input position, tiling where specified
		vector<T, N> hash_position = position_i + offset_i;
		for (std::size_t j = 0; j < N; ++j)
		{
			if (tiling[j])
			{
				hash_position[j] = std::fmod(hash_position[j], tiling[j]);
				if (hash_position[j] < T{0})
					hash_position[j] += tiling[j];
			}
		}
		
		// Calculate hash values for the hash position
		vector<hash::make_uint_t<T>, N> hash_i = hash(hash_position);
		
		// Convert hash values to pseudorandom fractional offset
		vector<T, N> offset_f = vector<T, N>(hash_i) * hash_scale;
		
		// Calculate displacement from input position to cell center
		vector<T, N> displacement = (offset_i + offset_f) - position_f;
		
		// Calculate square distance to the current cell center
		T sqr_distance = sqr_length(displacement);
		
		// Update F1 cell
		if (sqr_distance < f1_sqr_distance)
		{
			f1_sqr_distance = sqr_distance;
			f1_displacement = displacement;
			f1_hash = hash_i[0];
		}
	}
	
    return
	{
		f1_sqr_distance,
		f1_displacement,
		f1_hash
	};
}

/**
 * Finds the Voronoi cell (F1) containing the input position, along with the distance to the nearest edge.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 *
 * @param position Input position.
 * @param randomness Degree of randomness, on `[0, 1]`.
 * @param tiling Distance at which the Voronoi pattern should repeat. A value of `0` indicates no repetition.
 * @param hash Hash function.
 *
 * @return Tuple containing the square Euclidean distance from @p position to the F1 cell center, the displacement vector from the input position to the F1 cell center, a hash value indicating the ID of the F1 cell, and the square Euclidean distance from @p position to the nearest edge.
 */
template <class T, std::size_t N>
[[nodiscard]] std::tuple
<
	// F1 square distance to center
	T,
	
	// F1 position to center displacement
	vector<T, N>,
	
	// F1 hash
	hash::make_uint_t<T>,
	
	// Edge square distance
	T
>
f1_edge
(
	const vector<T, N>& position,
	T randomness = T{1},
	const vector<T, N>& tiling = vector<T, N>::zero(),
	vector<hash::make_uint_t<T>, N> (*hash)(const vector<T, N>&) = &hash::pcg<T, N>
)
{
	// Calculate factor which scales hash value onto `[0, 1]`, modulated by desired randomness
	T hash_scale = (T{1} / static_cast<T>(std::numeric_limits<hash::make_uint_t<T>>::max())) * randomness;
	
	// Get integer and fractional parts
    vector<T, N> position_i = floor(position - T{1.5});
    vector<T, N> position_f = position - position_i;
	
    // Find F1 cell
	T f1_sqr_distance_center = std::numeric_limits<T>::infinity();
	vector<T, N> displacement_cache[kernel_size<N>];
	std::size_t f1_i = 0;
	hash::make_uint_t<T> f1_hash;
	for (std::size_t i = 0; i < kernel_size<N>; ++i)
	{
		// Get kernel offset for current cell
		const vector<T, N>& offset_i = kernel<T, N>[i];
		
		// Calculate hash input position, tiling where specified
		vector<T, N> hash_position = position_i + offset_i;
		for (std::size_t j = 0; j < N; ++j)
		{
			if (tiling[j])
			{
				hash_position[j] = std::fmod(hash_position[j], tiling[j]);
				if (hash_position[j] < T{0})
					hash_position[j] += tiling[j];
			}
		}
		
		// Calculate hash values for the hash position
		vector<hash::make_uint_t<T>, N> hash_i = hash(hash_position);
		
		// Convert hash values to pseudorandom fractional offset
		vector<T, N> offset_f = vector<T, N>(hash_i) * hash_scale;
		
		// Calculate and cache displacement from input position to cell center
		displacement_cache[i] = (offset_i + offset_f) - position_f;
		
		// Calculate square distance to the current cell center
		T sqr_distance = sqr_length(displacement_cache[i]);
		
		// Update F1 cell
		if (sqr_distance < f1_sqr_distance_center)
		{
			f1_sqr_distance_center = sqr_distance;
			f1_i = i;
			f1_hash = hash_i[0];
		}
	}
	
	// Get displacement vector from input position to the F1 cell center
	const vector<T, N>& f1_displacement = displacement_cache[f1_i];
	
   // Find distance to the closest edge
	T edge_sqr_distance_edge = std::numeric_limits<T>::infinity();
	for (std::size_t i = 0; i < kernel_size<N>; ++i)
	{
		// Skip F1 cell
		if (i == f1_i)
			continue;
		
		// Fetch cached displacement vector for current cell
		const vector<T, N>& displacement = displacement_cache[i];
		
		// Find midpoint between the displacement vectors
		const vector<T, N> midpoint = (f1_displacement + displacement) * T{0.5};
		
		// Calculate direction from the F1 cell to current cell
		const vector<T, N> direction = normalize(displacement - f1_displacement);
		
		// Calculate square distance to the edge
		const T sqr_distance = dot(midpoint, direction);
		
		// Update minimum edge distance if closer than the nearest edge
		if (sqr_distance < edge_sqr_distance_edge)
			edge_sqr_distance_edge = sqr_distance;
	}
	
    return
	{
		f1_sqr_distance_center,
		f1_displacement,
		f1_hash,
		edge_sqr_distance_edge
	};
}

/**
 * Finds the Voronoi cell (F1) containing the input position, as well as the nearest neighboring cell (F2).
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 *
 * @param position Input position.
 * @param randomness Degree of randomness, on `[0, 1]`.
 * @param tiling Distance at which the Voronoi pattern should repeat. A value of `0` indicates no repetition.
 * @param hash Hash function.
 *
 * @return Tuple containing the square Euclidean distances, displacement vectors from the input position to the cell centers, and hash values indicating the cell IDs, for both the F1 and F2 cells.
 */
template <class T, std::size_t N>
[[nodiscard]] std::tuple
<
	// F1 square distance to center
	T,
	
	// F1 position to center displacement
	vector<T, N>,
	
	// F1 hash
	hash::make_uint_t<T>,
	
	// F2 square distance to center
	T,
	
	// F2 position to center displacement
	vector<T, N>,
	
	// F2 hash
	hash::make_uint_t<T>
>
f1_f2
(
	const vector<T, N>& position,
	T randomness = T{1},
	const vector<T, N>& tiling = vector<T, N>::zero(),
	vector<hash::make_uint_t<T>, N> (*hash)(const vector<T, N>&) = &hash::pcg<T, N>
)
{
	// Calculate factor which scales hash value onto `[0, 1]`, modulated by desired randomness
	T hash_scale = (T{1} / static_cast<T>(std::numeric_limits<hash::make_uint_t<T>>::max())) * randomness;
	
	// Get integer and fractional parts
    vector<T, N> position_i = floor(position - T{1.5});
    vector<T, N> position_f = position - position_i;
	
    // Find the F1 and F2 cells
	T f1_sqr_distance_center = std::numeric_limits<T>::infinity();
	vector<T, N> f1_displacement = {0, 0};
	hash::make_uint_t<T> f1_hash = 0;
	T f2_sqr_distance_center = std::numeric_limits<T>::infinity();
	vector<T, N> f2_displacement = {0, 0};
	hash::make_uint_t<T> f2_hash = 0;
	for (std::size_t i = 0; i < kernel_size<N>; ++i)
	{
		// Get kernel offset for current cell
		const vector<T, N>& offset_i = kernel<T, N>[i];
		
		// Calculate hash input position, tiling where specified
		vector<T, N> hash_position = position_i + offset_i;
		for (std::size_t j = 0; j < N; ++j)
		{
			if (tiling[j])
			{
				hash_position[j] = std::fmod(hash_position[j], tiling[j]);
				if (hash_position[j] < T{0})
					hash_position[j] += tiling[j];
			}
		}
		
		// Calculate hash values for the hash position
		vector<hash::make_uint_t<T>, N> hash_i = hash(hash_position);
		
		// Convert hash values to pseudorandom fractional offset
		vector<T, N> offset_f = vector<T, N>(hash_i) * hash_scale;
		
		// Calculate displacement from input position to cell center
		vector<T, N> displacement = (offset_i + offset_f) - position_f;
		
		// Calculate square distance to the current cell center
		T sqr_distance = sqr_length(displacement);
		
		// Update F1 and F2 cells
		if (sqr_distance < f1_sqr_distance_center)
		{
			f2_sqr_distance_center = f1_sqr_distance_center;
			f2_displacement = f1_displacement;
			f2_hash = f1_hash;
			
			f1_sqr_distance_center = sqr_distance;
			f1_displacement = displacement;
			f1_hash = hash_i[0];
		}
		else if (sqr_distance < f2_sqr_distance_center)
		{
			f2_sqr_distance_center = sqr_distance;
			f2_displacement = displacement;
			f2_hash = hash_i[0];
		}
	}
	
    return
	{
		f1_sqr_distance_center,
		f1_displacement,
		f1_hash,
		f2_sqr_distance_center,
		f2_displacement,
		f2_hash,
	};
}

} // namespace voronoi

} // namespace noise
} // namespace math

#endif // ANTKEEPER_MATH_NOISE_VORONOI_HPP
