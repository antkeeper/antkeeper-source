// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_WORLD_HPP
#define ANTKEEPER_GAME_WORLD_HPP

#include "game/ecoregion.hpp"

class game;

/// World creation and manipulation functions.
namespace world
{
	/// Creates the cosmos.
	void cosmogenesis(::game& ctx);

	/// Creates the observer.
	void create_observer(::game& ctx);

	/// Sets the location of the observer.
	/// @param ctx Game context.
	/// @param elevation Elevation, in meters.
	/// @param latitude Latitude, in radians.
	/// @param longitude Longitude, in radians.
	void set_location(::game& ctx, double elevation, double latitude, double longitude);

	/// Sets the current time.
	/// @param ctx Game context.
	/// @param t UT1 time, in days.
	void set_time(::game& ctx, double t);

	/// Sets the current time.
	/// @param ctx Game context.
	/// @param year Astronomical year numbering. 1 BC is `0`, 2 BC is `-1`.
	/// @param month Month number on `[1, 12]`.
	/// @param day Day number on `[1, 31]`.
	/// @param hour Hour number on `[0, 23]`.
	/// @param minute Minute number on `[0, 59]`.
	/// @param second Fractional second on `[0.0, 60.0)`.
	void set_time(::game& ctx, int year, int month, int day, int hour, int minute, double second);

	/// Enters a ecoregion.
	/// @param ctx Game context.
	/// @param ecoregion Ecoregion to enter.
	void enter_ecoregion(::game& ctx, const ecoregion& ecoregion);
}

#endif // ANTKEEPER_GAME_WORLD_HPP
