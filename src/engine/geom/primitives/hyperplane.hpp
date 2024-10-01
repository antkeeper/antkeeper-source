// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_PRIMITIVES_HYPERPLANE_HPP
#define ANTKEEPER_GEOM_PRIMITIVES_HYPERPLANE_HPP

#include <engine/math/vector.hpp>

namespace geom {
namespace primitives {

/// *n*-dimensional plane.
/// @tparam T Real type.
/// @tparam N Number of dimensions.
template <class T, std::size_t N>
struct hyperplane
{
	/// Vector type.
	using vector_type = math::vector<T, N>;
	
	/// Hyperplane normal.
	vector_type normal;
	
	/// Hyperplane constant.
	T constant;
	
	/// Constructs a hyperplane.
	constexpr hyperplane() noexcept = default;
	
	/// Constructs a hyperplane given a normal and constant.
	/// @param normal Plane normal.
	/// @param constant Plane constant.
	inline constexpr hyperplane(const vector_type& normal, float constant) noexcept:
		normal{normal},
		constant{constant}
	{}
	
	/// Constructs a hyperplane given a normal and an offset point.
	/// @param normal Plane normal.
	/// @param offset Offset from the origin.
	inline constexpr hyperplane(const vector_type& normal, const vector_type& offset) noexcept:
		normal{normal},
		constant{-math::dot(normal, offset)}
	{}
	
	/// Calculates the signed distance from the hyperplane to a point.
	/// @param point Input point.
	/// @return Signed distance from the hyperplane to @p point.
	[[nodiscard]] inline constexpr T distance(const vector_type& point) const noexcept
	{
		return math::dot(normal, point) + constant;
	}
};

} // namespace primitives

using namespace primitives;

} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVES_HYPERPLANE_HPP
