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

#ifndef ANTKEEPER_MATH_NOISE_FBM_HPP
#define ANTKEEPER_MATH_NOISE_FBM_HPP

#include "math/hash/make-uint.hpp"
#include "math/hash/pcg.hpp"
#include "math/noise/simplex.hpp"
#include "math/vector.hpp"
#include <cstdint>

namespace math {
namespace noise {

/**
 * Fractional Brownian motion (fBm).
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 *
 * @param position Input position.
 * @param octaves Number of octaves.
 * @param lacunarity Frequency multiplier.
 * @param gain Amplitude multiplier.
 * @param noise Noise function.
 * @param hash Hash function.
 *
 */
template <class T, std::size_t N>
T fbm
(
	vector<T, N> position,
	std::size_t octaves,
	T lacunarity,
	T gain,
	T (*noise)(const vector<T, N>&, vector<hash::make_uint_t<T>, N> (*)(const vector<T, N>&)) = &simplex<T, N>,
	vector<hash::make_uint_t<T>, N> (*hash)(const vector<T, N>&) = &hash::pcg<T, N>
)
{
    T amplitude{1};
    T value{0};
	
	while (octaves--)
	{
        value += noise(position, hash) * amplitude;
        position *= lacunarity;
        amplitude *= gain;
	}
	
    return value;
}

} // namespace noise
} // namespace math

#endif // ANTKEEPER_MATH_NOISE_FBM_HPP
