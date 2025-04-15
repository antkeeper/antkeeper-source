// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.geom.brep.feature;
export import engine.geom.brep.vertex;
export import engine.geom.brep.edge;
export import engine.geom.brep.face;
export import <variant>;

export namespace engine::geom::brep
{
	/// B-rep feature, which can be either a vertex, edge, or face.
	using feature = std::variant<vertex*, edge*, face*>;
}
