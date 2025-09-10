// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/geom/brep/vertex.hpp>
#include <engine/geom/brep/edge.hpp>
#include <engine/geom/brep/face.hpp>
#include <variant>

namespace engine::geom::brep
{
	/// B-rep feature, which can be either a vertex, edge, or face.
	using feature = std::variant<vertex*, edge*, face*>;
}
