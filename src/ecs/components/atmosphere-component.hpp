/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_ECS_ATMOSPHERE_COMPONENT_HPP
#define ANTKEEPER_ECS_ATMOSPHERE_COMPONENT_HPP

#include "utility/fundamental-types.hpp"

namespace ecs {

/// Atmosphere
struct atmosphere_component
{
	/// Altitude of the outer atmosphere, in meters.
	double exosphere_altitude;
	
	/// Rayleigh scale height, in meters.
	double rayleigh_scale_height;
	
	/// Mie scale height, in meters.
	double mie_scale_height;
	
	/// (Dependent) Rayleigh scattering coefficients
	double3 rayleigh_scattering_coefficients;
	
	/// (Dependent) Mie scattering coefficients
	double3 mie_scattering_coefficients;
};

} // namespace ecs

#endif // ANTKEEPER_ECS_ATMOSPHERE_COMPONENT_HPP
