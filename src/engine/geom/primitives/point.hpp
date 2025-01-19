// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_PRIMITIVES_POINT_HPP
#define ANTKEEPER_GEOM_PRIMITIVES_POINT_HPP

#include <engine/math/vector.hpp>

namespace geom {
namespace primitives {

/// *n*-dimensional point.
/// @tparam T Real type.
/// @tparam N Number of dimensions.
template <class T, std::size_t N>
using point = math::vector<T, N>;

} // namespace primitives

using namespace primitives;

} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVES_POINT_HPP
