// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ATMOSPHERE_COMPONENT_HPP
#define ANTKEEPER_GAME_ATMOSPHERE_COMPONENT_HPP

import engine.math.vector;

using namespace engine;

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
