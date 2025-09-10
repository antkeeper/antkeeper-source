// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_CELESTIAL_BODY_COMPONENT_HPP
#define ANTKEEPER_GAME_CELESTIAL_BODY_COMPONENT_HPP

#include <vector>

/// A simple celestial body.
struct celestial_body_component
{
	/// Mean radius of the body, in meters.
	double radius;
	
	/// Mass of the body, in kilograms.
	double mass;
	
	/// Polynomial coefficients, in descending order of degree, of the right ascension of the body's north pole, in radians, w.r.t. Julian centuries (36525 days) from epoch.
	std::vector<double> pole_ra;
	
	/// Polynomial coefficients, in descending order of degree, of the declination of the body's north pole, in radians, w.r.t. Julian centuries (36525 days) from epoch.
	std::vector<double> pole_dec;
	
	/// Polynomial coefficients, in descending order of degree, of the rotation state of the body's prime meridian, in radians, w.r.t. days from epoch.
	std::vector<double> prime_meridian;
	
	/*
	/// Polynomial coefficients, in descending order of degree, of the nutation and precession angles, in radians. Angles are calculated as `x + y * d`, where `d` is the days from epoch.
	std::vector<std::vector<double>> nutation_precession_angles;
	
	/// Nutation and precession amplitudes of the right ascension of the body's north pole, in radians.
	std::vector<double> nutation_precession_ra;
	
	/// Nutation and precession amplitudes of the declination of the body's north pole, in radians.
	std::vector<double> nutation_precession_dec;
	
	/// Nutation and precession amplitudes of the rotation state of the body's prime meridian, in radians.
	std::vector<double> nutation_precession_pm;
	*/
	
	/// Geometric albedo
	double albedo;
};

#endif // ANTKEEPER_GAME_CELESTIAL_BODY_COMPONENT_HPP
