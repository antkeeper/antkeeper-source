// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.noise.fbm;
export import engine.math.vector;
import engine.noise.simplex;
import engine.hash.make_uint;
import engine.hash.pcg;
import engine.utility.sized_types;

export namespace engine::noise
{
	/// Fractional Brownian motion (fBm).
	/// @tparam T Real type.
	/// @tparam N Number of dimensions.
	/// @param position Input position.
	/// @param octaves Number of octaves.
	/// @param lacunarity Frequency multiplier.
	/// @param gain Amplitude multiplier.
	/// @param noise Noise function.
	/// @param hash Hash function.
	template <class T, usize N>
	[[nodiscard]] T fbm
	(
		math::vector<T, N> position,
		usize octaves,
		T lacunarity,
		T gain,
		T (*noise)(const math::vector<T, N>&, math::vector<hash::make_uint_t<T>, N> (*)(const math::vector<T, N>&)) = &simplex<T, N>,
		math::vector<hash::make_uint_t<T>, N> (*hash)(const math::vector<T, N>&) = &hash::pcg<T, N>
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
}
