// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.geom.primitives.hypercapsule;
export import engine.geom.primitives.line_segment;
export import engine.math.vector;
import engine.math.functions;
import engine.utility.sized_types;

export namespace engine::geom::inline primitives
{
	/// *n*-dimensional capsule.
	/// @tparam T Real type.
	/// @tparam N Number of dimensions.
	template <class T, usize N>
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
			return (d ? math::sqrt(d) : d) - radius;
		}
	};
}
