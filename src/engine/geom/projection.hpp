// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_PROJECTION_HPP
#define ANTKEEPER_GEOM_PROJECTION_HPP

#include <engine/math/vector.hpp>

namespace geom {

template <class T>
math::vec3<T> project_on_plane(const math::vec3<T>& v, const math::vec3<T>& p, const math::vec3<T>& n)
{
	return v - n * math::dot(v - p, n);
}

} // namespace geom

#endif // ANTKEEPER_GEOM_PROJECTION_HPP
