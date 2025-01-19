// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_BLACKBODY_COMPONENT_HPP
#define ANTKEEPER_GAME_BLACKBODY_COMPONENT_HPP

#include <engine/math/vector.hpp>

/// Blackbody radiator
struct blackbody_component
{
	/// Effective temperature, in Kelvin.
	double temperature{};
	
	/// Luminance of the blackbody.
	double luminance{};
	
	/// Color of the blackbody.
	math::dvec3 color{};
};

#endif // ANTKEEPER_GAME_BLACKBODY_COMPONENT_HPP
