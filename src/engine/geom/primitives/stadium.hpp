// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_PRIMITIVES_STADIUM_HPP
#define ANTKEEPER_GEOM_PRIMITIVES_STADIUM_HPP

#include <engine/geom/primitives/hypercapsule.hpp>

namespace geom {
namespace primitives {

/**
 * 2-dimensional capsule.
 *
 * @tparam T Real type.
 */
template <class T>
using stadium = hypercapsule<T, 2>;

} // namespace primitives
} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVES_STADIUM_HPP
