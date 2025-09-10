// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/vector.hpp>
#include <engine/utility/sized-types.hpp>

namespace engine::geom::inline primitives
{
	/// Half of a line proceeding from an initial point.
	/// @tparam T Real type.
	/// @tparam N Number of dimensions.
	template <class T, usize N>
	struct ray
	{
		/// Vector type.
		using vector_type = math::vector<T, N>;

		/// Ray origin position.
		vector_type origin;

		/// Ray direction vector.
		vector_type direction;

		/// Extrapolates from the ray origin along the ray direction vector.
		/// @param distance Signed extrapolation distance.
		/// @return Extrapolated coordinates.
		[[nodiscard]] inline constexpr vector_type extrapolate(T distance) const noexcept
		{
			return origin + direction * distance;
		}

		/// Calculates the square distance from the ray to a point.
		/// @param point Input point.
		/// @return Square distance from the ray to @p point.
		[[nodiscard]] inline constexpr T sqr_distance(const vector_type& point) const noexcept
		{
			return math::sqr_distance(point, closest_point(point));
		}

		/// Calculates the distance from the ray to a point.
		/// @param point Input point.
		/// @return Distance from the ray to @p point.
		[[nodiscard]] inline T distance(const vector_type& point) const noexcept
		{
			const auto d = sqr_distance(point);
			return d ? math::sqrt(d) : d;
		}
	};
}
