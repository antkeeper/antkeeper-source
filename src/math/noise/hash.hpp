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

#ifndef ANTKEEPER_MATH_NOISE_HASH_HPP
#define ANTKEEPER_MATH_NOISE_HASH_HPP

#include "math/vector.hpp"
#include <cstdint>

namespace math {
namespace noise {

/// Hash functions.
namespace hash {

/**
 * PCG3D hash.
 *
 * @see Mark Jarzynski and Marc Olano, Hash Functions for GPU Rendering, Journal of Computer Graphics Techniques (JCGT), vol. 9, no. 3, 21-38, 2020.
 */
/// @{
template <class T>
math::vector<std::uint32_t, 3> pcg3d_3(const math::vector<T, 3>& x)
{
	math::vector<std::uint32_t, 3> u = math::vector<std::uint32_t, 3>(x) * 1664525U + 1013904223U;
	
	u[0] += u[1] * u[2];
	u[1] += u[2] * u[0];
	u[2] += u[0] * u[1];
	
	u[0] ^= u[0] >> 16U;
	u[1] ^= u[1] >> 16U;
	u[2] ^= u[2] >> 16U;
	
	u[0] += u[1] * u[2];
	u[1] += u[2] * u[0];
	u[2] += u[0] * u[1];
	
	return u;
}

template <class T>
math::vector<std::uint32_t, 3> pcg3d_3(const math::vector<T, 2>& x)
{
	return pcg3d_3<T>({x[0], x[1], T{1}});
}

template <class T>
math::vector<std::uint32_t, 3> pcg3d_3(const math::vector<T, 1>& x)
{
	return pcg3d_3<T>({x[0], T{1}, T{1}});
}

template <class T>
math::vector<std::uint32_t, 3> pcg3d_3(T x)
{
	return pcg3d_3<T>({x, T{1}, T{1}});
}

template <class T>
std::uint32_t pcg3d_1(const math::vector<T, 3>& x)
{
	return pcg3d_3<T>(x)[0];
}

template <class T>
std::uint32_t pcg3d_1(const math::vector<T, 2>& x)
{
	return pcg3d_3<T>({x[0], x[1], T{1}})[0];
}

template <class T>
std::uint32_t pcg3d_1(const math::vector<T, 1>& x)
{
	return pcg3d_3<T>({x[0], T{1}, T{1}})[0];
}

template <class T>
std::uint32_t pcg3d_1(T x)
{
	return pcg3d_3<T>({x, T{1}, T{1}})[0];
}
/// @}

/**
 * PCG4D hash.
 *
 * @see Mark Jarzynski and Marc Olano, Hash Functions for GPU Rendering, Journal of Computer Graphics Techniques (JCGT), vol. 9, no. 3, 21-38, 2020.
 */
/// @{
template <class T>
math::vector<std::uint32_t, 4> pcg4d_4(const math::vector<T, 4>& x)
{
	math::vector<std::uint32_t, 4> u = math::vector<std::uint32_t, 4>(x) * 1664525U + 1013904223U;
	
	u[0] += u[1] * u[3];
	u[1] += u[2] * u[0];
	u[2] += u[0] * u[1];
	u[3] += u[1] * u[2];
	
	u[0] ^= u[0] >> 16U;
	u[1] ^= u[1] >> 16U;
	u[2] ^= u[2] >> 16U;
	u[3] ^= u[3] >> 16U;
	
	u[0] += u[1] * u[3];
	u[1] += u[2] * u[0];
	u[2] += u[0] * u[1];
	u[3] += u[1] * u[2];
	
	return u;
}

template <class T>
std::uint32_t pcg4d_1(const math::vector<T, 4>& x)
{
	return pcg4d_4<T>(x)[0];
}
/// @}

} // namespace hash

} // namespace noise
} // namespace math

#endif // ANTKEEPER_MATH_NOISE_HASH_HPP
