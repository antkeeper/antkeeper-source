// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/vector.hpp>
#include <engine/math/functions.hpp>

/// Signed distance functions.
namespace engine::geom::sdf
{
	/// Translates a point.
	/// @param sample Point to translate.
	/// @param offset Translation offset.
	/// @return Translated point.
	[[nodiscard]] inline math::fvec3 translate(const math::fvec3& sample, const math::fvec3& offset) noexcept
	{
		return sample - offset;
	}

	/// Sphere signed distance function.
	/// @param p Sphere center.
	/// @param r Sphere radius.
	/// @return Signed distance to the sphere.
	[[nodiscard]] inline float sphere(const math::fvec3& p, float r)
	{
		return math::length(p) - r;
	}

	/// Cylinder signed distance function.
	/// @param p Cylinder center.
	/// @param r Cylinder radius.
	/// @param h Cylinder height.
	/// @return Signed distance to the cylinder.
	[[nodiscard]] inline float cylinder(const math::fvec3& p, float r, float h)
	{
		float dx = math::abs(math::length(math::swizzle<0, 2>(p))) - r;
		float dy = math::abs(p[1]) - h;
		return math::min(math::max(dx, dy), 0.0f) + math::length(math::fvec2{math::max(dx, 0.0f), math::max(dy, 0.0f)});
	}

	/// Signed distance union operation.
	/// @param a First signed distance.
	/// @param b Second signed distance.
	/// @return Signed distance to the union of the two shapes.
	[[nodiscard]] inline float op_union(float a, float b) noexcept
	{
		return math::min(a, b);
	}

	/// Signed distance difference operation.
	/// @param a First signed distance.
	/// @param b Second signed distance.
	/// @return Signed distance to the difference of the two shapes.
	[[nodiscard]] inline float op_difference(float a, float b) noexcept
	{
		return math::max(-a, b);
	}

	/// Signed distance intersection operation.
	/// @param a First signed distance.
	///	@param b Second signed distance.
	/// @return Signed distance to the intersection of the two shapes.
	[[nodiscard]] inline float op_intersection(float a, float b) noexcept
	{
		return math::max(a, b);
	}

	/// Signed distance round operation.
	/// @param d Signed distance.
	/// @param r Radius of the round operation.
	/// @return Signed distance to the rounded shape.
	[[nodiscard]] inline float op_round(float d, float r) noexcept
	{
		return d - r;
	}
}
