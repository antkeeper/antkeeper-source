// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_NOISE_FBM_HPP
#define ANTKEEPER_NOISE_FBM_HPP

#include <engine/noise/simplex.hpp>
#include <engine/hash/make-uint.hpp>
#include <engine/hash/pcg.hpp>
#include <engine/math/vector.hpp>
#include <cstddef>

// export module noise.fbm;
// import noise.simplex;
// import hash.make_uint;
// import hash.pcg;
// import math.vector;
// import <cstddef>;

namespace noise {

/// Fractional Brownian motion (fBm).
/// @tparam T Real type.
/// @tparam N Number of dimensions.
/// @param position Input position.
/// @param octaves Number of octaves.
/// @param lacunarity Frequency multiplier.
/// @param gain Amplitude multiplier.
/// @param noise Noise function.
/// @param hash Hash function.
template <class T, std::size_t N>
[[nodiscard]] T fbm
(
	math::vector<T, N> position,
	std::size_t octaves,
	T lacunarity,
	T gain,
	T (*noise)(const math::vector<T, N>&, math::vector<::hash::make_uint_t<T>, N> (*)(const math::vector<T, N>&)) = &simplex<T, N>,
	math::vector<::hash::make_uint_t<T>, N> (*hash)(const math::vector<T, N>&) = &::hash::pcg<T, N>
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

#endif // ANTKEEPER_NOISE_FBM_HPP
