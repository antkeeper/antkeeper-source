// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_PICKING_COMPONENT_HPP
#define ANTKEEPER_GAME_PICKING_COMPONENT_HPP

import engine.geom.primitives.sphere;
import engine.utility.sized_types;

using namespace engine;

struct picking_component
{
	/// Picking sphere.
	geom::sphere<float> sphere;
	
	/// Picking flags.
	u32 flags;
};

#endif // ANTKEEPER_GAME_PICKING_COMPONENT_HPP
