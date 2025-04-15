// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.noise.voronoi;
export import engine.math.vector;
import engine.hash.make_uint;
import engine.hash.pcg;
import engine.utility.sized_types;
import engine.math.constants;
import engine.math.functions;
export import <array>;
export import <tuple>;
import <limits>;
import <utility>;

export namespace engine::noise
{
	/// Number of Voronoi cells to search.
	/// @tparam N Number of dimensions.
	/// @private
	template <usize N>
	inline constexpr usize voronoi_kernel_size = 4 << math::max(usize{0}, (2 * (N - 1)));

	/// Generates an kernel offset vector for a given index.
	/// @tparam T Real type.
	/// @tparam N Number of dimensions.
	/// @tparam I Index sequence.
	/// @param i Index of a kernel offset vector.
	/// @return Kernel offset vector.
	/// @private
	template <class T, usize N, usize... I>
	[[nodiscard]] constexpr math::vector<T, N> voronoi_kernel_offset(usize i, std::index_sequence<I...>)
	{
		return {static_cast<T>((I ? (i / (2 << math::max(usize{0}, 2 * I - 1))) : i) % 4)...};
	}

	/// Generates a Voronoi search kernel.
	/// @tparam T Real type.
	/// @tparam N Number of dimensions.
	/// @tparam I Index sequence.
	/// @return Voronoi search kernel.
	/// @private
	template <class T, usize N, usize... I>
	[[nodiscard]] constexpr std::array<math::vector<T, N>, voronoi_kernel_size<N>> voronoi_generate_kernel(std::index_sequence<I...>)
	{
		return {voronoi_kernel_offset<T, N>(I, std::make_index_sequence<N>{})...};
	}

	/// *n*-dimensional search kernel.
	/// @tparam T Real type.
	/// @tparam N Number of dimensions.
	/// @private
	template <class T, usize N>
	inline constexpr auto voronoi_kernel = voronoi_generate_kernel<T, N>(std::make_index_sequence<voronoi_kernel_size<N>>{});

	/// Finds the Voronoi cell (F1) containing the input position.
	/// @tparam T Real type.
	/// @tparam N Number of dimensions.
	/// @param position Input position.
	/// @param randomness Degree of randomness, on `[0, 1]`.
	/// @param tiling Distance at which the Voronoi pattern should repeat. A value of `0` indicates no repetition.
	/// @param hash Hash function.
	/// @return Tuple containing the square Euclidean distance from @p position to the F1 cell, the displacement vector from the input position to the F1 cell center, and a hash value indicating the ID of the F1 cell.
	template <class T, usize N>
	[[nodiscard]] std::tuple
	<
		// F1 square distance to center
		T,
	
		// F1 position to center displacement
		math::vector<T, N>,
	
		// F1 hash
		hash::make_uint_t<T>
	>
	voronoi_f1
	(
		const math::vector<T, N>& position,
		T randomness = T{1},
		const math::vector<T, N>& tiling = math::zero<math::vector<T, N>>,
		math::vector<hash::make_uint_t<T>, N> (*hash)(const math::vector<T, N>&) = &hash::pcg<T, N>
	)
	{
		// Calculate factor which scales hash value onto `[0, 1]`, modulated by desired randomness
		T hash_scale = (T{1} / static_cast<T>(std::numeric_limits<hash::make_uint_t<T>>::max())) * randomness;
	
		// Get integer and fractional parts
		math::vector<T, N> position_i = math::floor(position - T{1.5});
		math::vector<T, N> position_f = position - position_i;
	
		// Find the F1 cell 
		T f1_sqr_distance = math::inf<T>;
		math::vector<T, N> f1_displacement;
		hash::make_uint_t<T> f1_hash;
		for (usize i = 0; i < voronoi_kernel_size<N>; ++i)
		{
			// Get kernel offset for current cell
			const math::vector<T, N>& offset_i = voronoi_kernel<T, N>[i];
		
			// Calculate hash input position, tiling where specified
			math::vector<T, N> hash_position = position_i + offset_i;
			for (usize j = 0; j < N; ++j)
			{
				if (tiling[j])
				{
					hash_position[j] = math::mod(hash_position[j], tiling[j]);
					if (hash_position[j] < T{0})
						hash_position[j] += tiling[j];
				}
			}
		
			// Calculate hash values for the hash position
			math::vector<hash::make_uint_t<T>, N> hash_i = hash(hash_position);
		
			// Convert hash values to pseudorandom fractional offset
			math::vector<T, N> offset_f = math::vector<T, N>(hash_i) * hash_scale;
		
			// Calculate displacement from input position to cell center
			math::vector<T, N> displacement = (offset_i + offset_f) - position_f;
		
			// Calculate square distance to the current cell center
			T sqr_distance = math::sqr_length(displacement);
		
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

	/// Finds the Voronoi cell (F1) containing the input position, along with the distance to the nearest edge.
	/// @tparam T Real type.
	/// @tparam N Number of dimensions.
	/// @param position Input position.
	/// @param randomness Degree of randomness, on `[0, 1]`.
	/// @param tiling Distance at which the Voronoi pattern should repeat. A value of `0` indicates no repetition.
	/// @param hash Hash function.
	/// @return Tuple containing the square Euclidean distance from @p position to the F1 cell center, the displacement vector from the input position to the F1 cell center, a hash value indicating the ID of the F1 cell, and the square Euclidean distance from @p position to the nearest edge.
	template <class T, usize N>
	[[nodiscard]] std::tuple
	<
		// F1 square distance to center
		T,
	
		// F1 position to center displacement
		math::vector<T, N>,
	
		// F1 hash
		hash::make_uint_t<T>,
	
		// Edge square distance
		T
	>
	voronoi_f1_edge
	(
		const math::vector<T, N>& position,
		T randomness = T{1},
		const math::vector<T, N>& tiling = math::zero<math::vector<T, N>>,
		math::vector<hash::make_uint_t<T>, N> (*hash)(const math::vector<T, N>&) = &hash::pcg<T, N>
	)
	{
		// Calculate factor which scales hash value onto `[0, 1]`, modulated by desired randomness
		T hash_scale = (T{1} / static_cast<T>(std::numeric_limits<hash::make_uint_t<T>>::max())) * randomness;
	
		// Get integer and fractional parts
		math::vector<T, N> position_i = math::floor(position - T{1.5});
		math::vector<T, N> position_f = position - position_i;
	
		// Find F1 cell
		T f1_sqr_distance_center = math::inf<T>;
		math::vector<T, N> displacement_cache[voronoi_kernel_size<N>];
		usize f1_i = 0;
		hash::make_uint_t<T> f1_hash;
		for (usize i = 0; i < voronoi_kernel_size<N>; ++i)
		{
			// Get kernel offset for current cell
			const math::vector<T, N>& offset_i = voronoi_kernel<T, N>[i];
		
			// Calculate hash input position, tiling where specified
			math::vector<T, N> hash_position = position_i + offset_i;
			for (usize j = 0; j < N; ++j)
			{
				if (tiling[j])
				{
					hash_position[j] = math::mod(hash_position[j], tiling[j]);
					if (hash_position[j] < T{0})
						hash_position[j] += tiling[j];
				}
			}
		
			// Calculate hash values for the hash position
			math::vector<hash::make_uint_t<T>, N> hash_i = hash(hash_position);
		
			// Convert hash values to pseudorandom fractional offset
			math::vector<T, N> offset_f = math::vector<T, N>(hash_i) * hash_scale;
		
			// Calculate and cache displacement from input position to cell center
			displacement_cache[i] = (offset_i + offset_f) - position_f;
		
			// Calculate square distance to the current cell center
			T sqr_distance = math::sqr_length(displacement_cache[i]);
		
			// Update F1 cell
			if (sqr_distance < f1_sqr_distance_center)
			{
				f1_sqr_distance_center = sqr_distance;
				f1_i = i;
				f1_hash = hash_i[0];
			}
		}
	
		// Get displacement vector from input position to the F1 cell center
		const math::vector<T, N>& f1_displacement = displacement_cache[f1_i];
	
	   // Find distance to the closest edge
		T edge_sqr_distance_edge = math::inf<T>;
		for (usize i = 0; i < voronoi_kernel_size<N>; ++i)
		{
			// Skip F1 cell
			if (i == f1_i)
				continue;
		
			// Fetch cached displacement vector for current cell
			const math::vector<T, N>& displacement = displacement_cache[i];
		
			// Find midpoint between the displacement vectors
			const math::vector<T, N> midpoint = (f1_displacement + displacement) * T{0.5};
		
			// Calculate direction from the F1 cell to current cell
			const math::vector<T, N> direction = normalize(displacement - f1_displacement);
		
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

	/// Finds the Voronoi cell (F1) containing the input position, as well as the nearest neighboring cell (F2).
	/// @tparam T Real type.
	/// @tparam N Number of dimensions.
	/// @param position Input position.
	/// @param randomness Degree of randomness, on `[0, 1]`.
	/// @param tiling Distance at which the Voronoi pattern should repeat. A value of `0` indicates no repetition.
	/// @param hash Hash function.
	/// @return Tuple containing the square Euclidean distances, displacement vectors from the input position to the cell centers, and hash values indicating the cell IDs, for both the F1 and F2 cells.
	template <class T, usize N>
	[[nodiscard]] std::tuple
	<
		// F1 square distance to center
		T,
	
		// F1 position to center displacement
		math::vector<T, N>,
	
		// F1 hash
		hash::make_uint_t<T>,
	
		// F2 square distance to center
		T,
	
		// F2 position to center displacement
		math::vector<T, N>,
	
		// F2 hash
		hash::make_uint_t<T>
	>
	voronoi_f1_f2
	(
		const math::vector<T, N>& position,
		T randomness = T{1},
		const math::vector<T, N>& tiling = math::zero<math::vector<T, N>>,
		math::vector<hash::make_uint_t<T>, N> (*hash)(const math::vector<T, N>&) = &hash::pcg<T, N>
	)
	{
		// Calculate factor which scales hash value onto `[0, 1]`, modulated by desired randomness
		T hash_scale = (T{1} / static_cast<T>(std::numeric_limits<hash::make_uint_t<T>>::max())) * randomness;
	
		// Get integer and fractional parts
		math::vector<T, N> position_i = math::floor(position - T{1.5});
		math::vector<T, N> position_f = position - position_i;
	
		// Find the F1 and F2 cells
		T f1_sqr_distance_center = math::inf<T>;
		math::vector<T, N> f1_displacement = {0, 0};
		hash::make_uint_t<T> f1_hash = 0;
		T f2_sqr_distance_center = math::inf<T>;
		math::vector<T, N> f2_displacement = {0, 0};
		hash::make_uint_t<T> f2_hash = 0;
		for (usize i = 0; i < voronoi_kernel_size<N>; ++i)
		{
			// Get kernel offset for current cell
			const math::vector<T, N>& offset_i = voronoi_kernel<T, N>[i];
		
			// Calculate hash input position, tiling where specified
			math::vector<T, N> hash_position = position_i + offset_i;
			for (usize j = 0; j < N; ++j)
			{
				if (tiling[j])
				{
					hash_position[j] = math::mod(hash_position[j], tiling[j]);
					if (hash_position[j] < T{0})
						hash_position[j] += tiling[j];
				}
			}
		
			// Calculate hash values for the hash position
			math::vector<hash::make_uint_t<T>, N> hash_i = hash(hash_position);
		
			// Convert hash values to pseudorandom fractional offset
			math::vector<T, N> offset_f = math::vector<T, N>(hash_i) * hash_scale;
		
			// Calculate displacement from input position to cell center
			math::vector<T, N> displacement = (offset_i + offset_f) - position_f;
		
			// Calculate square distance to the current cell center
			T sqr_distance = math::sqr_length(displacement);
		
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
}
