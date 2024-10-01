// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_PRIMITIVES_CAPSULE_HPP
#define ANTKEEPER_GEOM_PRIMITIVES_CAPSULE_HPP

#include <engine/geom/primitives/hypercapsule.hpp>

namespace geom {
namespace primitives {

/// 3-dimensional capsule.
/// @tparam T Real type.
template <class T>
using capsule = hypercapsule<T, 3>;

} // namespace primitives
} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVES_CAPSULE_HPP
