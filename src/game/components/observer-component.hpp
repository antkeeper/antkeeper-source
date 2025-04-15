// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_OBSERVER_COMPONENT_HPP
#define ANTKEEPER_GAME_OBSERVER_COMPONENT_HPP

import engine.entity.id;

using namespace engine;

struct observer_component
{
	/// Entity ID of a celestial body to which the observer position is relative.
	entity::id reference_body_eid;
	
	/// Elevation of the observer, in radians.
	double elevation;
	
	/// Latitude of the observer, in radians.
	double latitude;
	
	/// Longitude of the observer, in radians.
	double longitude;
};

#endif // ANTKEEPER_GAME_OBSERVER_COMPONENT_HPP
