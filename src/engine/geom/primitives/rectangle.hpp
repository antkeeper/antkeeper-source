// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_PRIMITIVES_RECTANGLE_HPP
#define ANTKEEPER_GEOM_PRIMITIVES_RECTANGLE_HPP

#include <engine/geom/primitives/hyperrectangle.hpp>

namespace geom {
namespace primitives {

/// 2-dimensional hyperrectangle.
/// @tparam T Real type.
template <class T>
using rectangle = hyperrectangle<T, 2>;

} // namespace primitives
} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVES_RECTANGLE_HPP
