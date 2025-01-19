// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_SDF_HPP
#define ANTKEEPER_GEOM_SDF_HPP

#include <engine/math/vector.hpp>
#include <algorithm>

namespace geom {

/// Signed distance functions.
namespace sdf {

/// Translates a point.
/// @param sample Point to translate.
/// @param offset Translation offset.
/// @return Translated point.
inline math::fvec3 translate(const math::fvec3& sample, const math::fvec3& offset)
{
	return sample - offset;
}

/// Sphere signed distance function.
/// @param p Sphere center.
/// @param r Sphere radius.
/// @return Signed distance to the sphere.
inline float sphere(const math::fvec3& p, float r)
{
	return math::length(p) - r;
}

/// Cylinder signed distance function.
/// @param p Cylinder center.
/// @param r Cylinder radius.
/// @param h Cylinder height.
/// @return Signed distance to the cylinder.
inline float cylinder(const math::fvec3& p, float r, float h)
{
	float dx = std::abs(math::length(math::swizzle<0, 2>(p))) - r;
	float dy = std::abs(p[1]) - h;
	return std::min<float>(std::max<float>(dx, dy), 0.0f) + math::length(math::fvec2{std::max<float>(dx, 0.0f), std::max<float>(dy, 0.0f)});
}

/// Signed distance union operation.
/// @param a First signed distance.
/// @param b Second signed distance.
/// @return Signed distance to the union of the two shapes.
inline float op_union(float a, float b)
{
	return std::min<float>(a, b);
}

/// Signed distance difference operation.
/// @param a First signed distance.
/// @param b Second signed distance.
/// @return Signed distance to the difference of the two shapes.
inline float op_difference(float a, float b)
{
	return std::max<float>(-a, b);
}

/// Signed distance intersection operation.
/// @param a First signed distance.
///	@param b Second signed distance.
/// @return Signed distance to the intersection of the two shapes.
inline float op_intersection(float a, float b)
{
	return std::max<float>(a, b);
}

/// Signed distance round operation.
/// @param d Signed distance.
/// @param r Radius of the round operation.
/// @return Signed distance to the rounded shape.
inline float op_round(float d, float r)
{
    return d - r;
}

} // namespace sdf
} // namespace geom

#endif // ANTKEEPER_GEOM_SDF_HPP
