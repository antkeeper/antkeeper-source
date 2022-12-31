/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANTKEEPER_GAME_COMPONENT_BLACKBODY_HPP
#define ANTKEEPER_GAME_COMPONENT_BLACKBODY_HPP

namespace game {
namespace component {

/// Blackbody radiator
struct blackbody
{
	/// Effective temperature, in Kelvin.
	double temperature;
	
	/// (Dependent) RGB spectral luminance, in cd/m^2.
	double3 luminance;
};

} // namespace component
} // namespace game

#endif // ANTKEEPER_GAME_COMPONENT_BLACKBODY_HPP
