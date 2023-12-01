// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_PRIMITIVES_PLANE_HPP
#define ANTKEEPER_GEOM_PRIMITIVES_PLANE_HPP

#include <engine/geom/primitives/hyperplane.hpp>

namespace geom {
namespace primitives {

/**
 * 3-dimensional hyperplane.
 *
 * @tparam T Real type.
 */
template <class T>
using plane = hyperplane<T, 3>;

} // namespace primitives
} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVES_PLANE_HPP
