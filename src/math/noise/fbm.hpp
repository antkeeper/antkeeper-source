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

#ifndef ANTKEEPER_MATH_NOISE_FBM_HPP
#define ANTKEEPER_MATH_NOISE_FBM_HPP

#include "math/vector.hpp"
#include <cstdint>

namespace math {
namespace noise {

/**
 * Fractional Brownian motion (fBm).
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 * @tparam U Hash function return type.
 *
 * @param x n-dimensional input value.
 * @param octaves Number of octaves.
 * @param lacunarity Frequency multiplier.
 * @param gain Amplitude multiplier.
 * @param noise Noise function.
 * @param hash Hash function.
 *
 */
template <class T, std::size_t N, class U>
T fbm
(
	vector<T, N> x,
	std::size_t octaves,
	T lacunarity,
	T gain,
	T (*noise)(const vector<T, N>&, U (*)(const vector<T, N>&)),
	U (*hash)(const vector<T, N>&)
)
{
    T amplitude{1};
    T value{0};
	
	while (octaves--)
	{
        value += noise(x, hash) * amplitude;
        x *= lacunarity;
        amplitude *= gain;
	}
	
    return value;
}

} // namespace noise
} // namespace math

#endif // ANTKEEPER_MATH_NOISE_FBM_HPP
