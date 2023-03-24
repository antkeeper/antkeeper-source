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

#ifndef ANTKEEPER_GEOM_CLOSEST_POINT_HPP
#define ANTKEEPER_GEOM_CLOSEST_POINT_HPP

#include <engine/geom/primitives/hypercapsule.hpp>
#include <engine/geom/primitives/hyperplane.hpp>
#include <engine/geom/primitives/hyperrectangle.hpp>
#include <engine/geom/primitives/hypersphere.hpp>
#include <engine/geom/primitives/line-segment.hpp>
#include <engine/geom/primitives/point.hpp>
#include <engine/geom/primitives/ray.hpp>
#include <algorithm>
#include <cmath>
#include <tuple>

namespace geom {

/**
 * Calculates the closest point on a ray to a point.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 *
 * @param a Ray a.
 * @param b Point b.
 *
 * @return Closest point on ray a to point b.
 */
template <class T, std::size_t N>
[[nodiscard]] point<T, N> closest_point(const ray<T, N>& a, const point<T, N>& b) noexcept
{
	return a.extrapolate(std::max<T>(T{0}, math::dot(b - a.origin, a.direction)));
}

/**
 * Calculates the closest point on a line segment to a point.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 *
 * @param ab Line segment ab.
 * @param c Point c.
 *
 * @return Closest point on line segment ab to point c.
 */
template <class T, std::size_t N>
[[nodiscard]] point<T, N> closest_point(const line_segment<T, N>& ab, const point<T, N>& c) noexcept
{
	const auto direction_ab = ab.b - ab.a;
	
	const auto distance_ab = math::dot(c - ab.a, direction_ab);
	if (distance_ab <= T{0})
	{
		return ab.a;
	}
	else
	{
		const auto sqr_length_ab = math::sqr_length(direction_ab);
		if (distance_ab >= sqr_length_ab)
		{
			return ab.b;
		}
		else
		{
			return ab.a + direction_ab * (distance_ab / sqr_length_ab);
		}
	}
}

/**
 * Calculates the closest points on two line segments.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 *
 * @param ab Line segment ab.
 * @param cd Line segment cd.
 *
 * @return Tuple containing the closest point on segment ab to segment cd, followed by the closest point on segment cd to segment ab.
 */
/// @{
template <class T, std::size_t N>
[[nodiscard]] std::tuple<point<T, N>, point<T, N>> closest_point(const line_segment<T, N>& ab, const line_segment<T, N>& cd) noexcept
{
	const auto direction_ab = ab.b - ab.a;
	const auto direction_cd = cd.b - cd.a;
	const auto direction_ca = ab.a - cd.a;
	
	const auto sqr_length_ab = math::sqr_length(direction_ab);
	const auto sqr_length_cd = math::sqr_length(direction_cd);
	const auto cd_dot_ca = math::dot(direction_cd, direction_ca);
	
	if (sqr_length_ab <= T{0})
	{
		if (sqr_length_cd <= T{0})
		{
			// Both segments are points
			return {ab.a, cd.a};
		}
		else
		{
			// Segment ab is a point
			return
			{
				ab.a,
				cd.a + direction_cd * std::min<T>(std::max<T>(cd_dot_ca / sqr_length_cd, T{0}), T{1})
			};
		}
	}
	else
	{
		const auto ab_dot_ca = math::dot(direction_ab, direction_ca);
		
		if (sqr_length_cd <= T{0})
		{
			// Segment cd is a point
			return
			{
				ab.a + direction_ab * std::min<T>(std::max<T>(-ab_dot_ca / sqr_length_ab, T{0}), T{1}),
				cd.a
			};
		}
		else
		{
			const auto ab_dot_cd = math::dot(direction_ab, direction_cd);
			
			const auto den = sqr_length_ab * sqr_length_cd - ab_dot_cd * ab_dot_cd;
			
			auto distance_ab = (den) ? std::min<T>(std::max<T>((ab_dot_cd * cd_dot_ca - ab_dot_ca * sqr_length_cd) / den, T{0}), T{1}) : T{0};
			auto distance_cd = (ab_dot_cd * distance_ab + cd_dot_ca) / sqr_length_cd;
			
			if (distance_cd < T{0})
			{
				return
				{
					ab.a + direction_ab * std::min<T>(std::max<T>(-ab_dot_ca / sqr_length_ab, T{0}), T{1}),
					cd.a
				};
			}
			else if (distance_cd > T{1})
			{
				return
				{
					ab.a + direction_ab * std::min<T>(std::max<T>((ab_dot_cd - ab_dot_ca) / sqr_length_ab, T{0}), T{1}),
					cd.b
				};
			}
			
			return
			{
				ab.a + direction_ab * distance_ab,
				cd.a + direction_cd * distance_cd
			};
		}
	}
}

/**
 * Calculates the closest point on a hyperplane to a point.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 *
 * @param a Hyperplane a.
 * @param b Point b.
 *
 * @return Closest point on hyperplane a to point b.
 */
template <class T, std::size_t N>
[[nodiscard]] point<T, N> closest_point(const hyperplane<T, N>& a, const point<T, N>& b) noexcept
{
	return b - a.normal * (math::dot(a.normal, b) + a.constant);
}

/**
 * Calculates the closest point on a hypersphere to a point.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 *
 * @param a Hypersphere a.
 * @param b Point b.
 *
 * @return Closest point on hypersphere a to point b.
 */
template <class T, std::size_t N>
[[nodiscard]] point<T, N> closest_point(const hypersphere<T, N>& a, const point<T, N>& b)
{
	const auto ab = b - a.center;
	const auto d = math::sqr_length(ab);
	return a.center + ab * (d ? a.radius / std::sqrt(d) : d);
}

/**
 * Calculates the closest point on a hypercapsule to a point.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 *
 * @param a Hypercapsule a.
 * @param b Point b.
 *
 * @return Closest point on hypercapsule a to point b.
 */
template <class T, std::size_t N>
[[nodiscard]] point<T, N> closest_point(const hypercapsule<T, N>& a, const point<T, N>& b)
{
	const auto c = closest_point(a.segment, b);
	const auto cb = b - c;
	const auto d = math::sqr_length(cb);
	return c + cb * (d ? a.radius / std::sqrt(d) : d);
}

/**
 * Calculates the closest point on a hyperrectangle to a point.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 *
 * @param a Hyperrectangle a.
 * @param b Point b.
 *
 * @return Closest point on hyperrectangle a to point b.
 */
template <class T, std::size_t N>
[[nodiscard]] point<T, N> closest_point(const hyperrectangle<T, N>& a, const point<T, N>& b) noexcept
{
	const auto c = a.center();
	const auto p = b - c;
	const auto d = math::abs(p) - a.extents();
	const auto m = std::min<T>(math::max(d), T{0});
	
	point<T, N> r;
	for (std::size_t i = 0; i < N; ++i)
	{
		r[i] = c[i] + std::copysign(d[i] >= m ? d[i] : T{0}, p[i]);
	}
	
	return r;
}

} // namespace geom

#endif // ANTKEEPER_GEOM_CLOSEST_POINT_HPP
