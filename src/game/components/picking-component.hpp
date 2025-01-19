// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_PICKING_COMPONENT_HPP
#define ANTKEEPER_GAME_PICKING_COMPONENT_HPP

#include <engine/geom/primitives/sphere.hpp>
#include <cstdint>

struct picking_component
{
	/// Picking sphere.
	geom::sphere<float> sphere;
	
	/// Picking flags.
	std::uint32_t flags;
};

#endif // ANTKEEPER_GAME_PICKING_COMPONENT_HPP
