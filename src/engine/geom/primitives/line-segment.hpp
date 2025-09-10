// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/vector.hpp>
#include <engine/math/functions.hpp>
#include <engine/utility/sized-types.hpp>

namespace engine::geom::inline primitives
{
	/// *n*-dimensional line segment.
	/// @tparam T Real type.
	/// @tparam N Number of dimensions.
	template <class T, usize N>
	struct line_segment
	{
		/// Vector type.
		using vector_type = math::vector<T, N>;

		/// First endpoint.
		vector_type a;

		/// Second endpoint.
		vector_type b;

		/// Calculates the square length of the line segment.
		/// @return Square length of the line segment.
		[[nodiscard]] inline T sqr_length() const noexcept
		{
			return math::sqr_distance(a, b);
		}

		/// Calculates the length of the line segment.
		/// @return Length of the line segment.
		[[nodiscard]] inline T length() const noexcept
		{
			const T d = sqr_length();
			return d ? math::sqrt(d) : d;
		}

		/// Calculates the square distance from the line segment to a point.
		/// @param point Input point.
		/// @return Square distance from the line segment to @p point.
		[[nodiscard]] T sqr_distance(const vector_type& point) const noexcept
		{
			const vector_type ab = b - a;
			const vector_type ap = point - a;

			const T t = math::dot(ap, ab);
			if (t <= T{0})
			{
				return math::sqr_length(ap);
			}

			const T ab_sqr_length = math::sqr_length(ab);
			if (t >= ab_sqr_length)
			{
				return math::sqr_length(point - b);
			}

			return math::sqr_length(ap) - (t * t) / ab_sqr_length;
		}

		/// Calculates the distance from the line segment to a point.
		/// @param point Input point.
		/// @return Distance from the line segment to @p point.
		[[nodiscard]] inline T distance(const vector_type& point) const
		{
			const T d = sqr_distance(point);
			return d ? math::sqrt(d) : d;
		}
	};
}
