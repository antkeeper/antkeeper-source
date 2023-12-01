// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_COORDINATES_HPP
#define ANTKEEPER_GEOM_COORDINATES_HPP

#include <engine/geom/primitives/point.hpp>
#include <bit>
#include <cstdint>
#include <cmath>

namespace geom {

/**
 * Voronoi regions of a triangle.
 */
enum class triangle_region: std::uint8_t
{
	/// Face ABC region.
	abc = 0b000,
	
	/// Edge AB region.
	ab  = 0b100,
	
	/// Edge BC region.
	bc  = 0b001,
	
	/// Edge CA region.
	ca  = 0b010,
	
	/// Vertex A region.
	a   = 0b110,
	
	/// Vertex B region.
	b   = 0b101,
	
	/// Vertex C region.
	c   = 0b011
};

/**
 * Checks whether a triangle voronoi region is a face region.
 *
 * @param region Triangle region.
 *
 * @return `true` if @p region is a face region, `false` otherwise.
 */
[[nodiscard]] inline constexpr bool is_face_region(triangle_region region) noexcept
{
	return !static_cast<bool>(region);
}

/**
 * Checks whether a triangle voronoi region is an edge region.
 *
 * @param region Triangle region.
 *
 * @return `true` if @p region is an edge region, `false` otherwise.
 */
[[nodiscard]] inline constexpr bool is_edge_region(triangle_region region) noexcept
{
	return std::has_single_bit(static_cast<std::uint8_t>(region));
}

/**
 * Checks whether a triangle voronoi region is a vertex region.
 *
 * @param region Triangle region.
 *
 * @return `true` if @p region is an vertex region, `false` otherwise.
 */
[[nodiscard]] inline constexpr bool is_vertex_region(triangle_region region) noexcept
{
	return static_cast<bool>(region) && !std::has_single_bit(static_cast<std::uint8_t>(region));
}

/**
 * Returns the edge index of an edge region.
 *
 * @param region Triangle edge region.
 *
 * @return Edge index.
 */
[[nodiscard]] inline constexpr std::uint8_t edge_index(triangle_region region) noexcept
{
	return static_cast<std::uint8_t>(region) & std::uint8_t{0b11};
}

/**
 * Returns the vertex index of a vertex region.
 *
 * @param region Triangle vertex region.
 *
 * @return Vertex index.
 */
[[nodiscard]] inline constexpr std::uint8_t vertex_index(triangle_region region) noexcept
{
	return std::uint8_t{3} - (static_cast<std::uint8_t>(region) >> 1);
}

/**
 * Classifies barycentric coordinates according to their Voronoi region.
 *
 * @tparam T Real type.
 *
 * @param p Barycentric coordinates of point to classify.
 *
 * @return Voronoi region of point @p p.
 */
template <class T>
[[nodiscard]] constexpr triangle_region barycentric_to_region(const point<T, 3>& p) noexcept
{
	std::uint8_t region = static_cast<std::uint8_t>(p.x() <= T{0});
	region |= static_cast<std::uint8_t>(p.y() <= T{0}) << std::uint8_t{1};
	region |= static_cast<std::uint8_t>(p.z() <= T{0}) << std::uint8_t{2};
	return static_cast<triangle_region>(region);
}

/**
 * Converts Cartesian coordinates to barycentric coordinates.
 *
 * @tparam T Real type.
 *
 * @param p Barycentric coordinates of point to convert.
 * @param a Cartesian coordinates of first point of triangle.
 * @param b Cartesian coordinates of second point of triangle.
 * @param c Cartesian coordinates of third point of triangle.
 *
 * @return Cartesian coordinates of point @p p.
 */
template <class T>
[[nodiscard]] inline constexpr point<T, 3> barycentric_to_cartesian(const point<T, 3>& p, const point<T, 3>& a, const point<T, 3>& b, const point<T, 3>& c) noexcept
{
	return a * p.x() + b * p.y() + c * p.z();
}

/**
 * Converts Cartesian coordinates to barycentric coordinates.
 *
 * @tparam T Real type.
 *
 * @param p Cartesian coordinates of point to convert.
 * @param a Cartesian coordinates of first point of triangle.
 * @param b Cartesian coordinates of second point of triangle.
 * @param c Cartesian coordinates of third point of triangle.
 *
 * @return Barycentric coordinates of point @p p.
 */
template <class T>
[[nodiscard]] constexpr point<T, 3> cartesian_to_barycentric(const point<T, 3>& p, const point<T, 3>& a, const point<T, 3>& b, const point<T, 3>& c) noexcept
{
	point<T, 3> uvw;
	
	// Cross product version:
	const auto ab = b - a;
	const auto ca = a - c;
	const auto ap = p - a;
	const auto n = math::cross(ab, ca);
	const auto d = math::sqr_length(n);
	const auto q = math::cross(n, ap);
	uvw.z() = math::dot(q, ab) / d;
	uvw.y() = math::dot(q, ca) / d;
	uvw.x() = T{1} - uvw.y() - uvw.z();
	
	// Dot product version:
	// const auto ab = b - a;
	// const auto ac = c - a;
	// const auto ap = p - a;
	// const auto ab_dot_ab = math::dot(ab, ab);
	// const auto ab_dot_ac = math::dot(ab, ac);
	// const auto ac_dot_ac = math::dot(ac, ac);
	// const auto ap_dot_ab = math::dot(ap, ab);
	// const auto ap_dot_ac = math::dot(ap, ac);
	// const auto d = ab_dot_ab * ac_dot_ac - ab_dot_ac * ab_dot_ac;
	// uvw.z() = (ab_dot_ab * ap_dot_ac - ab_dot_ac * ap_dot_ab) / d;
	// uvw.y() = (ac_dot_ac * ap_dot_ab - ab_dot_ac * ap_dot_ac) / d;
	// uvw.x() = T{1} - uvw.y() - uvw.z();
	
	return uvw;
}

/**
 * Converts Cartesian (rectangular) coordinates to spherical coordinates.
 *
 * @tparam T Real type.
 *
 * @param p Cartesian coordinates of point to convert.
 *
 * @return Spherical coordinates of point @p p, in the ISO order of radial distance, polar angle (radians), and azimuthal angle (radians).
 */
template <class T>
[[nodiscard]] point<T, 3> cartesian_to_spherical(const point<T, 3>& p)
{
	const T xx_yy = p.x() * p.x() + p.y() * p.y();
	
	return
	{
		std::sqrt(xx_yy + p.z() * p.z()),
		std::atan2(p.z(), std::sqrt(xx_yy)),
		std::atan2(p.y(), p.x())
	};
}

/**
 * Converts spherical coordinates to Cartesian (rectangular) coordinates.
 *
 * @tparam T Real type.
 *
 * @param p Spherical coordinates to convert, in the ISO order of radial distance, polar angle (radians), and azimuthal angle (radians).
 *
 * @return Cartesian coordinates of point @p p.
 */
template <class T>
[[nodiscard]] point<T, 3> spherical_to_cartesian(const point<T, 3>& p)
{
	const T x = p.x() * std::cos(p.y());
	
	return
	{
		x * std::cos(p.z()),
		x * std::sin(p.z()),
		p.x() * std::sin(p.y())
	};
}

} // namespace geom

#endif // ANTKEEPER_GEOM_COORDINATES_HPP
