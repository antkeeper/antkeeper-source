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

#ifndef ANTKEEPER_MATH_NOISE_VORONOI_HPP
#define ANTKEEPER_MATH_NOISE_VORONOI_HPP

#include "math/vector.hpp"
#include <algorithm>
#include <cmath>
#include <tuple>
#include <limits>

namespace math {
namespace noise {

/// Voronoi functions.
namespace voronoi {

/**
 * Number of neighboring cells that must be searched for F1.
 *
 * @private
 */
constexpr std::size_t f1_kernel_size = 12;

/**
 * Offsets to neighboring cells that must be searched for F1.
 *
 * @private
 */
template <class T>
constexpr vector<T, 2> f1_kernel[f1_kernel_size] =
{
	/*****/ {1, 0}, {2, 0}, /*****/
	{0, 1}, {1, 1}, {2, 1}, {3, 1},
	{0, 2}, {1, 2}, {2, 2}, {3, 2},
	/*****/ {1, 3}, {2, 3}  /*****/
};

/**
 * Maximum squared distance to the nearest F1 cell center.
 *
 * @private
 */
template <class T>
constexpr T f1_max_sqr_distance = T{8};

/**
 * Finds the Voronoi cell (F1) containing the input position.
 *
 * @tparam T Real type.
 * @tparam U Hash function return type.
 *
 * @param position Input position.
 * @param randomness Degree of randomness, on `[0, 1]`.
 * @param tiling Distance at which the Voronoi pattern should repeat. A value of `0` indicates no repetition.
 * @param hash Hash function.
 *
 * @return Tuple containing the square Euclidean distance from @p position to the F1 cell, the displacement vector from the input position to the F1 cell center, and a hash value indicating the ID of the F1 cell.
 */
template <class T, class U>
std::tuple
<
	// F1 square distance to center
	T,
	
	// F1 position to center displacement
	vector<T, 2>,
	
	// F1 hash
	U
>
f1
(
	const vector<T, 2>& position,
	T randomness,
	const vector<T, 2>& tiling,
	vector<U, 3> (*hash)(const vector<T, 2>&)
)
{
	// Calculate factor which scales hash value onto `[0, 1]`, modulated by desired randomness
	T hash_scale = (T{1} / static_cast<T>(std::numeric_limits<U>::max())) * randomness;
	
	// Get integer and fractional parts
    vector<T, 2> position_i = math::floor(position - T{1.5});
    vector<T, 2> position_f = position - position_i;
	
    // Find the F1 cell 
	T f1_sqr_distance = f1_max_sqr_distance<T>;
	vector<T, 2> f1_displacement;
	U f1_hash;
	
	for (std::size_t i = 0; i < f1_kernel_size; ++i)
	{
		// Get kernel offset for current cell
		const vector<T, 2>& offset_i = f1_kernel<T>[i];
		
		// Calculate hash input position, tiling where specified
		vector<T, 2> hash_position = position_i + offset_i;
		if (tiling[0])
			hash_position[0] = std::fmod(hash_position[0], tiling[0]);
		if (tiling[1])
			hash_position[1] = std::fmod(hash_position[1], tiling[1]);
		
		// Calculate hash values for the hash position
		vector<U, 2> hash_i = vector<U, 2>(hash(hash_position));
		
		// Convert hash values to pseudorandom fractional offset
		vector<T, 2> offset_f = vector<T, 2>(hash_i) * hash_scale;
		
		// Calculate displacement from input position to cell center
		vector<T, 2> displacement = (offset_i + offset_f) - position_f;
		
		// Calculate square distance to the current cell center
		T sqr_distance = math::length_squared(displacement);
		
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
 * @tparam U Hash function return type.
 *
 * @param position Input position.
 * @param randomness Degree of randomness, on `[0, 1]`.
 * @param tiling Distance at which the Voronoi pattern should repeat. A value of `0` indicates no repetition.
 * @param hash Hash function.
 *
 * @return Tuple containing the square Euclidean distance from @p position to the F1 cell center, the displacement vector from the input position to the F1 cell center, a hash value indicating the ID of the F1 cell, and the square Euclidean distance from @p position to the nearest edge.
 */
template <class T, class U>
std::tuple
<
	// F1 square distance to center
	T,
	
	// F1 position to center displacement
	vector<T, 2>,
	
	// F1 hash
	U,
	
	// Edge square distance
	T
>
f1_edge
(
	const vector<T, 2>& position,
	T randomness,
	const vector<T, 2>& tiling,
	vector<U, 3> (*hash)(const vector<T, 2>&)
)
{
	// Calculate factor which scales hash value onto `[0, 1]`, modulated by desired randomness
	T hash_scale = (T{1} / static_cast<T>(std::numeric_limits<U>::max())) * randomness;
	
	// Get integer and fractional parts
    vector<T, 2> position_i = math::floor(position - T{1.5});
    vector<T, 2> position_f = position - position_i;
	
    // Find F1 cell
	T f1_sqr_distance_center = f1_max_sqr_distance<T>;
	vector<T, 2> displacement_cache[4][4];
	int f1_i = 0;
	int f1_j = 0;
	U f1_hash;
	vector<T, 2> offset_i;
    for (int i = 0; i < 4; ++i)
	{
		offset_i[0] = static_cast<T>(i);
		
		for (int j = 0; j < 4; ++j)
		{
			offset_i[1] = static_cast<T>(j);
			
			// Calculate hash input position, tiling where specified
			vector<T, 2> hash_position = position_i + offset_i;
			if (tiling[0])
				hash_position[0] = std::fmod(hash_position[0], tiling[0]);
			if (tiling[1])
				hash_position[1] = std::fmod(hash_position[1], tiling[1]);
			
			// Calculate hash values for the hash position
			vector<U, 2> hash_i = vector<U, 2>(hash(hash_position));
			
			// Convert hash values to pseudorandom fractional offset
			vector<T, 2> offset_f = vector<T, 2>(hash_i) * hash_scale;
			
			// Calculate and cache displacement from input position to cell center
			displacement_cache[i][j] = (offset_i + offset_f) - position_f;
			
			// Calculate square distance to the current cell center
			T sqr_distance = math::length_squared(displacement_cache[i][j]);
			
			// Update F1 cell
			if (sqr_distance < f1_sqr_distance_center)
			{
				f1_sqr_distance_center = sqr_distance;
				f1_i = i;
				f1_j = j;
				f1_hash = hash_i[0];
			}
		}
	}
	
	// Get displacement vector from input position to the F1 cell center
	const vector<T, 2>& f1_displacement = displacement_cache[f1_i][f1_j];
	
   // Find distance to the closest edge
	T edge_sqr_distance_edge = f1_max_sqr_distance<T>;
    for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			// Skip F1 cell
			if (j == f1_j && i == f1_i)
				continue;
			
			// Fetch cached displacement vector for current cell
			const vector<T, 2>& displacement = displacement_cache[i][j];
			
			// Find midpoint between the displacement vectors
			const vector<T, 2> midpoint = (f1_displacement + displacement) * T{0.5};
			
			// Calculate direction from the F1 cell to current cell
			const vector<T, 2> direction = math::normalize(displacement - f1_displacement);
			
			// Calculate square distance to the edge
			const T sqr_distance = math::dot(midpoint, direction);
			
			// Update minimum edge distance if closer than the nearest edge
			if (sqr_distance < edge_sqr_distance_edge)
				edge_sqr_distance_edge = sqr_distance;
		}
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
 * @tparam U Hash function return type.
 *
 * @param position Input position.
 * @param randomness Degree of randomness, on `[0, 1]`.
 * @param tiling Distance at which the Voronoi pattern should repeat. A value of `0` indicates no repetition.
 * @param hash Hash function.
 *
 * @return Tuple containing the square Euclidean distances, displacement vectors from the input position to the cell centers, and hash values indicating the cell IDs, for both the F1 and F2 cells.
 */
template <class T, class U>
std::tuple
<
	// F1 square distance to center
	T,
	
	// F1 position to center displacement
	vector<T, 2>,
	
	// F1 hash
	U,
	
	// F2 square distance to center
	T,
	
	// F2 position to center displacement
	vector<T, 2>,
	
	// F2 hash
	U
>
f1_f2
(
	const vector<T, 2>& position,
	T randomness,
	const vector<T, 2>& tiling,
	vector<U, 3> (*hash)(const vector<T, 2>&)
)
{
	// Calculate factor which scales hash value onto `[0, 1]`, modulated by desired randomness
	T hash_scale = (T{1} / static_cast<T>(std::numeric_limits<U>::max())) * randomness;
	
	// Get integer and fractional parts
    vector<T, 2> position_i = math::floor(position - T{1.5});
    vector<T, 2> position_f = position - position_i;
	
    // Find the F1 and F2 cells
	T f1_sqr_distance_center = f1_max_sqr_distance<T>;
	vector<T, 2> f1_displacement = {0, 0};
	U f1_hash = 0;
	T f2_sqr_distance_center = f1_max_sqr_distance<T>;
	vector<T, 2> f2_displacement = {0, 0};
	U f2_hash = 0;
	vector<T, 2> offset_i;
    for (int i = 0; i < 4; ++i)
	{
		offset_i[0] = static_cast<T>(i);
		
		for (int j = 0; j < 4; ++j)
		{
			offset_i[1] = static_cast<T>(j);
			
			// Calculate hash input position, tiling where specified
			vector<T, 2> hash_position = position_i + offset_i;
			if (tiling[0])
				hash_position[0] = std::fmod(hash_position[0], tiling[0]);
			if (tiling[1])
				hash_position[1] = std::fmod(hash_position[1], tiling[1]);
			
			// Calculate hash values for the hash position
			vector<U, 2> hash_i = vector<U, 2>(hash(hash_position));
			
			// Convert hash values to pseudorandom fractional offset
			vector<T, 2> offset_f = vector<T, 2>(hash_i) * hash_scale;
			
			// Calculate displacement from input position to cell center
			vector<T, 2> displacement = (offset_i + offset_f) - position_f;
			
			// Calculate square distance to the current cell center
			T sqr_distance = math::length_squared(displacement);
			
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
