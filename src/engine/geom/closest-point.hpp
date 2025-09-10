// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/geom/primitives/primitives.hpp>
#include <engine/geom/coordinates.hpp>
#include <engine/utility/sized-types.hpp>
#include <engine/math/functions.hpp>
#include <tuple>

namespace engine::geom
{
	/// Calculates the closest point on a ray to a point.
	/// @tparam T Real type.
	/// @tparam N Number of dimensions.
	/// @param a Ray a.
	/// @param b Point b.
	/// @return Closest point on ray a to point b.
	template <class T, usize N>
	[[nodiscard]] constexpr point<T, N> closest_point(const ray<T, N>& a, const point<T, N>& b) noexcept
	{
		return a.extrapolate(math::max(T{0}, math::dot(b - a.origin, a.direction)));
	}

	/// Calculates the closest point on a line segment to a point.
	/// @tparam T Real type.
	/// @tparam N Number of dimensions.
	/// @param ab Line segment ab.
	/// @param c Point c.
	/// @return Closest point on line segment ab to point c.
	template <class T, usize N>
	[[nodiscard]] constexpr point<T, N> closest_point(const line_segment<T, N>& ab, const point<T, N>& c) noexcept
	{
		const auto direction_ab = ab.b - ab.a;

		const auto distance_ab = math::dot(c - ab.a, direction_ab);
		if (distance_ab <= T{0})
		{
			return ab.a;
		}
		else
		{
			const auto sqr_length_ab = sqr_length(direction_ab);
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

	/// Calculates the closest points on two line segments.
	/// @tparam T Real type.
	/// @tparam N Number of dimensions.
	/// @param ab Line segment ab.
	/// @param cd Line segment cd.
	/// @return Tuple containing the closest point on segment ab to segment cd, followed by the closest point on segment cd to segment ab.
	/// @see Ericson, C. (2004). Real-time collision detection. Crc Press.
	template <class T, usize N>
	[[nodiscard]] constexpr std::tuple<point<T, N>, point<T, N>> closest_point(const line_segment<T, N>& ab, const line_segment<T, N>& cd) noexcept
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
					cd.a + direction_cd * math::clamp(cd_dot_ca / sqr_length_cd, T{0}, T{1})
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
					ab.a + direction_ab * math::clamp(-ab_dot_ca / sqr_length_ab, T{0}, T{1}),
					cd.a
				};
			}
			else
			{
				const auto ab_dot_cd = math::dot(direction_ab, direction_cd);

				const auto den = sqr_length_ab * sqr_length_cd - ab_dot_cd * ab_dot_cd;

				auto distance_ab = (den) ? math::clamp((ab_dot_cd * cd_dot_ca - ab_dot_ca * sqr_length_cd) / den, T{0}, T{1}) : T{0};
				auto distance_cd = (ab_dot_cd * distance_ab + cd_dot_ca) / sqr_length_cd;

				if (distance_cd < T{0})
				{
					return
					{
						ab.a + direction_ab * math::clamp(-ab_dot_ca / sqr_length_ab, T{0}, T{1}),
						cd.a
					};
				}
				else if (distance_cd > T{1})
				{
					return
					{
						ab.a + direction_ab * math::clamp((ab_dot_cd - ab_dot_ca) / sqr_length_ab, T{0}, T{1}),
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

	/// Calculates the closest point on a hyperplane to a point.
	/// @tparam T Real type.
	/// @tparam N Number of dimensions.
	/// @param a Hyperplane a.
	/// @param b Point b.
	/// @return Closest point on hyperplane a to point b.
	template <class T, usize N>
	[[nodiscard]] constexpr point<T, N> closest_point(const hyperplane<T, N>& a, const point<T, N>& b) noexcept
	{
		return b - a.normal * (math::dot(a.normal, b) + a.constant);
	}

	/// Calculates the closest point on a triangle to a point.
	/// @tparam T Real type.
	/// @param a First point of triangle.
	/// @param b Second point of triangle.
	/// @param c Third point of triangle.
	/// @param p Point.
	/// @return Closest point on the triangle to point @p p, followed by the Voronoi region of the point.
	/// @see Ericson, C. (2004). Real-time collision detection. Crc Press.
	template <class T>
	[[nodiscard]] constexpr std::tuple<point<T, 3>, triangle_region> closest_point(const point<T, 3>& a, const point<T, 3>& b, const point<T, 3>& c, const point<T, 3>& p) noexcept
	{
		const auto ab = b - a;
		const auto ac = c - a;
		const auto ap = p - a;
		const auto ap_dot_ab = math::dot(ap, ab);
		const auto ap_dot_ac = math::dot(ap, ac);
		if (ap_dot_ab <= T{0} && ap_dot_ac <= T{0})
		{
			return {a, triangle_region::a};
		}

		const auto bc = c - b;
		const auto bp = p - b;
		const auto bp_dot_ba = math::dot(bp, a - b);
		const auto bp_dot_bc = math::dot(bp, bc);
		if (bp_dot_ba <= T{0} && bp_dot_bc <= T{0})
		{
			return {b, triangle_region::b};
		}

		const auto cp = p - c;
		const auto cp_dot_ca = math::dot(cp, a - c);
		const auto cp_dot_cb = math::dot(cp, b - c);
		if (cp_dot_ca <= T{0} && cp_dot_cb <= T{0})
		{
			return {c, triangle_region::c};
		}

		const auto n = math::cross(ab, ac);
		const auto pa = a - p;
		const auto pb = b - p;
		const auto vc = math::dot(n, math::cross(pa, pb));
		if (vc <= T{0} && ap_dot_ab >= T{0} && bp_dot_ba >= T{0})
		{
			return {a + ap_dot_ab / (ap_dot_ab + bp_dot_ba) * ab, triangle_region::ab};
		}

		const auto pc = c - p;
		const auto va = math::dot(n, math::cross(pb, pc));
		if (va <= T{0} && bp_dot_bc >= T{0} && cp_dot_cb >= T{0})
		{
			return {b + bp_dot_bc / (bp_dot_bc + cp_dot_cb) * bc, triangle_region::bc};
		}

		const auto vb = math::dot(n, math::cross(pc, pa));
		if (vb <= T{0} && ap_dot_ac >= T{0} && cp_dot_ca >= T{0})
		{
			return {a + ap_dot_ac / (ap_dot_ac + cp_dot_ca) * ac, triangle_region::ca};
		}

		const auto u = va / (va + vb + vc);
		const auto v = vb / (va + vb + vc);
		const auto w = T{1} - u - v;

		return {a * u + b * v + c * w, triangle_region::abc};
	}

	/// Calculates the closest point on a triangle to a point.
	/// @tparam T Real type.
	/// @param tri Triangle.
	/// @param p Point.
	/// @return Closest point on the triangle to point @p p, followed by the Voronoi region of the point.
	/// @see Ericson, C. (2004). Real-time collision detection. Crc Press.
	template <class T>
	[[nodiscard]] inline constexpr std::tuple<point<T, 3>, triangle_region> closest_point(const triangle<T, 3>& tri, const point<T, 3>& p) noexcept
	{
		return closest_point(tri.a, tri.b, tri.c, p);
	}

	/// Calculates the closest point on or in a hypersphere to a point.
	/// @tparam T Real type.
	/// @tparam N Number of dimensions.
	/// @param a Hypersphere a.
	/// @param b Point b.
	/// @return Closest point on or in hypersphere a to point b.
	template <class T, usize N>
	[[nodiscard]] point<T, N> closest_point(const hypersphere<T, N>& a, const point<T, N>& b)
	{
		const auto ab = b - a.center;
		const auto d = math::sqr_length(ab);
		return d > math::sqr(a.radius) ? a.center + ab * (a.radius / math::sqrt(d)) : b;
	}

	/// Calculates the closest point on or in a hypercapsule to a point.
	/// @tparam T Real type.
	/// @tparam N Number of dimensions.
	/// @param a Hypercapsule a.
	/// @param b Point b.
	/// @return Closest point on or in hypercapsule a to point b.
	template <class T, usize N>
	[[nodiscard]] point<T, N> closest_point(const hypercapsule<T, N>& a, const point<T, N>& b)
	{
		const auto c = closest_point(a.segment, b);
		const auto cb = b - c;
		const auto d = math::sqr_length(cb);
		return d > math::sqr(a.radius) ? c + cb * (a.radius / math::sqrt(d)) : b;
	}

	/// Calculates the closest point on or in a hyperrectangle to a point.
	/// @tparam T Real type.
	/// @tparam N Number of dimensions.
	/// @param a Hyperrectangle a.
	/// @param b Point b.
	/// @return Closest point on or in hyperrectangle a to point b.
	template <class T, usize N>
	[[nodiscard]] constexpr point<T, N> closest_point(const hyperrectangle<T, N>& a, const point<T, N>& b) noexcept
	{
		return math::clamp(b, a.min, a.max);
	}
}
