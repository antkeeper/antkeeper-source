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

#ifndef ANTKEEPER_FUNDAMENTAL_TYPES_HPP
#define ANTKEEPER_FUNDAMENTAL_TYPES_HPP

#include <engine/math/vector.hpp>
#include <engine/math/matrix.hpp>

/// 2D vector of bools
using bool2 = math::vector<bool, 2>;

/// 3D vector of bools
using bool3 = math::vector<bool, 3>;

/// 4D vector of bools
using bool4 = math::vector<bool, 4>;

/// 2D vector of chars
using char2 = math::vector<char, 2>;

/// 3D vector of chars
using char3 = math::vector<char, 3>;

/// 4D vector of chars
using char4 = math::vector<char, 4>;

/// 2D vector of unsigned chars
using uchar2 = math::vector<unsigned char, 2>;

/// 3D vector of unsigned chars
using uchar3 = math::vector<unsigned char, 3>;

/// 4D vector of unsigned chars
using uchar4 = math::vector<unsigned char, 4>;

/// 2D vector of shorts
using short2 = math::vector<short, 2>;

/// 3D vector of shorts
using short3 = math::vector<short, 3>;

/// 4D vector of shorts
using short4 = math::vector<short, 4>;

/// 2D vector of unsigned shorts
using ushort2 = math::vector<unsigned short, 2>;

/// 3D vector of unsigned shorts
using ushort3 = math::vector<unsigned short, 3>;

/// 4D vector of unsigned shorts
using ushort4 = math::vector<unsigned short, 4>;

/// 2D vector of ints
using int2 = math::vector<int, 2>;

/// 3D vector of ints
using int3 = math::vector<int, 3>;

/// 4D vector of ints
using int4 = math::vector<int, 4>;

/// 2D vector of unsigned ints
using uint2 = math::vector<unsigned int, 2>;

/// 3D vector of unsigned ints
using uint3 = math::vector<unsigned int, 3>;

/// 4D vector of unsigned ints
using uint4 = math::vector<unsigned int, 4>;

/// 2D vector of longs
using long2 = math::vector<long, 2>;

/// 3D vector of longs
using long3 = math::vector<long, 3>;

/// 4D vector of longs
using long4 = math::vector<long, 4>;

/// 2D vector of unsigned longs
using ulong2 = math::vector<unsigned long, 2>;

/// 3D vector of unsigned longs
using ulong3 = math::vector<unsigned long, 3>;

/// 4D vector of unsigned longs
using ulong4 = math::vector<unsigned long, 4>;

/// 2D vector of floats
using float2 = math::vector<float, 2>;

/// 3D vector of floats
using float3 = math::vector<float, 3>;

/// 4D vector of floats
using float4 = math::vector<float, 4>;

/// 2D vector of doubles
using double2 = math::vector<double, 2>;

/// 3D vector of doubles
using double3 = math::vector<double, 3>;

/// 4D vector of doubles
using double4 = math::vector<double, 4>;

/// 2x2 matrix of bools
using bool2x2 = math::matrix<bool, 2, 2>;

/// 3x3 matrix of bools
using bool3x3 = math::matrix<bool, 3, 3>;

/// 4x4 matrix of bools
using bool4x4 = math::matrix<bool, 4, 4>;

/// 2x2 matrix of chars
using char2x2 = math::matrix<char, 2, 2>;

/// 3x3 matrix of chars
using char3x3 = math::matrix<char, 3, 3>;

/// 4x4 matrix of chars
using char4x4 = math::matrix<char, 4, 4>;

/// 2x2 matrix of unsigned chars
using uchar2x2 = math::matrix<unsigned char, 2, 2>;

/// 3x3 matrix of unsigned chars
using uchar3x3 = math::matrix<unsigned char, 3, 3>;

/// 4x4 matrix of unsigned chars
using uchar4x4 = math::matrix<unsigned char, 4, 4>;

/// 2x2 matrix of shorts
using short2x2 = math::matrix<short, 2, 2>;

/// 3x3 matrix of shorts
using short3x3 = math::matrix<short, 3, 3>;

/// 4x4 matrix of shorts
using short4x4 = math::matrix<short, 4, 4>;

/// 2x2 matrix of unsigned shorts
using ushort2x2 = math::matrix<unsigned short, 2, 2>;

/// 3x3 matrix of unsigned shorts
using ushort3x3 = math::matrix<unsigned short, 3, 3>;

/// 4x4 matrix of unsigned shorts
using ushort4x4 = math::matrix<unsigned short, 4, 4>;

/// 2x2 matrix of ints
using int2x2 = math::matrix<int, 2, 2>;

/// 3x3 matrix of ints
using int3x3 = math::matrix<int, 3, 3>;

/// 4x4 matrix of ints
using int4x4 = math::matrix<int, 4, 4>;

/// 2x2 matrix of unsigned ints
using uint2x2 = math::matrix<unsigned int, 2, 2>;

/// 3x3 matrix of unsigned ints
using uint3x3 = math::matrix<unsigned int, 3, 3>;

/// 4x4 matrix of unsigned ints
using uint4x4 = math::matrix<unsigned int, 4, 4>;

/// 2x2 matrix of longs
using long2x2 = math::matrix<long, 2, 2>;

/// 3x3 matrix of longs
using long3x3 = math::matrix<long, 3, 3>;

/// 4x4 matrix of longs
using long4x4 = math::matrix<long, 4, 4>;

/// 2x2 matrix of unsigned longs
using ulong2x2 = math::matrix<unsigned long, 2, 2>;

/// 3x3 matrix of unsigned longs
using ulong3x3 = math::matrix<unsigned long, 3, 3>;

/// 4x4 matrix of unsigned longs
using ulong4x4 = math::matrix<unsigned long, 4, 4>;

/// 2x2 matrix of floats
using float2x2 = math::matrix<float, 2, 2>;

/// 3x3 matrix of floats
using float3x3 = math::matrix<float, 3, 3>;

/// 4x4 matrix of floats
using float4x4 = math::matrix<float, 4, 4>;

/// 2x2 matrix of doubles
using double2x2 = math::matrix<double, 2, 2>;

/// 3x3 matrix of doubles
using double3x3 = math::matrix<double, 3, 3>;

/// 4x4 matrix of doubles
using double4x4 = math::matrix<double, 4, 4>;

#endif // ANTKEEPER_FUNDAMENTAL_TYPES_HPP
