// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_PRIMITIVES_SPHERE_HPP
#define ANTKEEPER_GEOM_PRIMITIVES_SPHERE_HPP

#include <engine/geom/primitives/hypersphere.hpp>

namespace geom {
namespace primitives {

/// 3-dimensional hypersphere.
/// @tparam T Real type.
template <class T>
using sphere = hypersphere<T, 3>;

} // namespace primitives
} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVES_SPHERE_HPP
