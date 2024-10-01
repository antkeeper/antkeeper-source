// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_PRIMITIVES_HYPERCAPSULE_HPP
#define ANTKEEPER_GEOM_PRIMITIVES_HYPERCAPSULE_HPP

#include <engine/geom/primitives/line-segment.hpp>
#include <engine/math/vector.hpp>

namespace geom {
namespace primitives {

/// *n*-dimensional capsule.
/// @tparam T Real type.
/// @tparam N Number of dimensions.
template <class T, std::size_t N>
struct hypercapsule
{
	/// Vector type.
	using vector_type = math::vector<T, N>;
	
	/// Line segment type.
	using segment_type = geom::line_segment<T, N>;
	
	/// Medial line segment.
	segment_type segment;
	
	/// Radius of the hemi-hyperspheres.
	T radius;
	
	/// Tests whether a point is contained within this hypercapsule.
	/// @param point Point to test for containment.
	/// @return `true` if the point is contained within this hypercapsule, `false` otherwise.
	[[nodiscard]] inline constexpr bool contains(const vector_type& point) const noexcept
	{
		return segment.sqr_distance(point) <= radius * radius;
	}
	
	/// Calculates the signed distance from the hypercapsule to a point.
	/// @param point Input point.
	/// @return Signed distance from the hypercapsule to @p point.
	[[nodiscard]] inline T distance(const vector_type& point) const noexcept
	{
		const T d = segment.sqr_distance(point);
		return (d ? std::sqrt(d) : d) - radius;
	}
};

} // namespace primitives

using namespace primitives;

} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVES_HYPERCAPSULE_HPP
