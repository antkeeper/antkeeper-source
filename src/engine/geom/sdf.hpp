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

#ifndef ANTKEEPER_GEOM_SDF_HPP
#define ANTKEEPER_GEOM_SDF_HPP

#include <engine/utility/fundamental-types.hpp>
#include <algorithm>

namespace geom {

/// Signed distance functions.
namespace sdf {

inline float3 translate(const float3& sample, const float3& offset)
{
	return sample - offset;
}

inline float sphere(const float3& p, float r)
{
	return math::length(p) - r;
}

inline float cylinder(const float3& p, float r, float h)
{
	float dx = std::abs(math::length(math::swizzle<0, 2>(p))) - r;
	float dy = std::abs(p[1]) - h;
	return std::min<float>(std::max<float>(dx, dy), 0.0f) + math::length(float2{std::max<float>(dx, 0.0f), std::max<float>(dy, 0.0f)});
}

inline float op_union(float a, float b)
{
	return std::min<float>(a, b);
}

inline float op_difference(float a, float b)
{
	return std::max<float>(-a, b);
}

inline float op_intersection(float a, float b)
{
	return std::max<float>(a, b);
}

inline float op_round(float d, float r)
{
    return d - r;
}

} // namespace sdf
} // namespace geom

#endif // ANTKEEPER_GEOM_SDF_HPP

