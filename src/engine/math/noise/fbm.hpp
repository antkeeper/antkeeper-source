// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_MATH_NOISE_FBM_HPP
#define ANTKEEPER_MATH_NOISE_FBM_HPP

#include <engine/math/hash/make-uint.hpp>
#include <engine/math/hash/pcg.hpp>
#include <engine/math/noise/simplex.hpp>
#include <engine/math/vector.hpp>
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
[[nodiscard]] T fbm
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
