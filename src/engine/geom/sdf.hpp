// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_SDF_HPP
#define ANTKEEPER_GEOM_SDF_HPP

#include <engine/math/vector.hpp>
#include <algorithm>

namespace geom {

/// Signed distance functions.
namespace sdf {

inline math::fvec3 translate(const math::fvec3& sample, const math::fvec3& offset)
{
	return sample - offset;
}

inline float sphere(const math::fvec3& p, float r)
{
	return math::length(p) - r;
}

inline float cylinder(const math::fvec3& p, float r, float h)
{
	float dx = std::abs(math::length(math::swizzle<0, 2>(p))) - r;
	float dy = std::abs(p[1]) - h;
	return std::min<float>(std::max<float>(dx, dy), 0.0f) + math::length(math::fvec2{std::max<float>(dx, 0.0f), std::max<float>(dy, 0.0f)});
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

