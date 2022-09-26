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

#ifndef ANTKEEPER_ENTITY_COMPONENT_ATMOSPHERE_HPP
#define ANTKEEPER_ENTITY_COMPONENT_ATMOSPHERE_HPP

#include "utility/fundamental-types.hpp"

namespace entity {
namespace component {

/// Atmosphere
struct atmosphere
{
	/// Altitude of the outer atmosphere, in meters.
	double exosphere_altitude;
	
	/// Atmospheric index of refraction at sea level.
	double index_of_refraction;
	
	/// Molecular density of Rayleigh particles at sea level.
	double rayleigh_density;
	
	/// Molecular density of Mie particles at sea level.
	double mie_density;
	
	/// Rayleigh scale height, in meters.
	double rayleigh_scale_height;
	
	/// Mie scale height, in meters.
	double mie_scale_height;
	
	/// Mie phase function anisotropy factor.
	double mie_anisotropy;
	
	/// Airglow, in lux.
	double airglow;
	
	/// Molar concentration of air at sea level, in mol/m-3.
	double air_concentration;
	
	/// Concentration of ozone in the atmosphere, unitless.
	double ozone_concentration;
	
	/// Elevation of the lower limit of the ozone triangular distribution, in meters.
	double ozone_lower_limit;
	
	/// Elevation of the upper limit of the ozone triangular distribution, in meters.
	double ozone_upper_limit;
	
	/// Elevation of the mode of the ozone triangular distribution, in meters.
	double ozone_mode;
	
	/// (Dependent) Rayleigh scattering coefficients.
	double3 rayleigh_scattering;
	
	/// (Dependent) Mie scattering coefficients.
	double3 mie_scattering;
	
	/// (Dependent) Ozone absorption coefficients.
	double3 ozone_absorption;
};

} // namespace component
} // namespace entity

#endif // ANTKEEPER_ENTITY_COMPONENT_ATMOSPHERE_HPP
