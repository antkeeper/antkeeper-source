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

#ifndef ANTKEEPER_GAME_ATMOSPHERE_COMPONENT_HPP
#define ANTKEEPER_GAME_ATMOSPHERE_COMPONENT_HPP

#include <engine/math/vector.hpp>


/// Atmosphere
struct atmosphere_component
{
	/// Elevation of the upper limit of the atmosphere, in meters.
	double upper_limit;
	
	/// Index of refraction of air at sea level.
	double index_of_refraction;
	
	/// Molar concentration of Rayleigh particles at sea level, in mol/m-3.
	double rayleigh_concentration;
	
	/// Scale height of the exponential distribution of Rayleigh particles, in meters.
	double rayleigh_scale_height;
	
	/// (Dependent) Rayleigh scattering coefficients.
	math::dvec3 rayleigh_scattering;
	
	/// Molar concentration of Mie particles at sea level, in mol/m-3.
	double mie_concentration;
	
	/// Scale height of the exponential distribution of Mie particles, in meters.
	double mie_scale_height;
	
	/// Mie phase function anisotropy factor.
	double mie_anisotropy;
	
	/// Mie single-scattering albedo.
	double mie_albedo;
	
	/// (Dependent) Mie scattering coefficient.
	double mie_scattering;
	
	/// (Dependent) Mie extinction coefficient.
	double mie_extinction;
	
	/// Concentration of ozone in the atmosphere, unitless.
	double ozone_concentration;
	
	/// Elevation of the lower limit of the triangular distribution of ozone particles, in meters.
	double ozone_lower_limit;
	
	/// Elevation of the upper limit of the triangular distribution of ozone particles, in meters.
	double ozone_upper_limit;
	
	/// Elevation of the mode of the triangular distribution of ozone particles, in meters.
	double ozone_mode;
	
	/// (Dependent) Ozone absorption coefficients.
	math::dvec3 ozone_absorption;
	
	/// Airglow luminance, in cd/m^2.
	math::dvec3 airglow_luminance;
};


#endif // ANTKEEPER_GAME_ATMOSPHERE_COMPONENT_HPP
