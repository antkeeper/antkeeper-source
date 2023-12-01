// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_PRIMITIVES_LINE_HPP
#define ANTKEEPER_GEOM_PRIMITIVES_LINE_HPP

#include <engine/geom/primitives/hyperplane.hpp>

namespace geom {
namespace primitives {

/**
 * 2-dimensional hyperplane.
 *
 * @tparam T Real type.
 */
template <class T>
using line = hyperplane<T, 2>;

} // namespace primitives
} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVES_LINE_HPP
