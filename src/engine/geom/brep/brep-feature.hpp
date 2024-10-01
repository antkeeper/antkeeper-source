// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_BREP_FEATURE_HPP
#define ANTKEEPER_GEOM_BREP_FEATURE_HPP

#include <engine/geom/brep/brep-vertex.hpp>
#include <engine/geom/brep/brep-edge.hpp>
#include <engine/geom/brep/brep-face.hpp>
#include <variant>

namespace geom {

/// B-rep feature, which can be either a vertex, edge, or face.
using brep_feature = std::variant<geom::brep_vertex*, geom::brep_edge*, geom::brep_face*>;

} // namespace geom

#endif // ANTKEEPER_GEOM_BREP_FEATURE_HPP
