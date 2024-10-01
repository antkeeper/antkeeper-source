// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_PRIMITIVES_TRIANGLE_HPP
#define ANTKEEPER_GEOM_PRIMITIVES_TRIANGLE_HPP

#include <engine/math/vector.hpp>

namespace geom {
namespace primitives {

/// *n*-dimensional triangle.
/// @tparam T Real type.
/// @tparam N Number of dimensions.
template <class T, std::size_t N>
struct triangle
{
	/// Vector type.
	using vector_type = math::vector<T, N>;
	
	/// First point.
	vector_type a;
	
	/// Second point.
	vector_type b;
	
	/// Third point.
	vector_type c;
};

} // namespace primitives

using namespace primitives;

} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVES_TRIANGLE_HPP
