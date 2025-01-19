// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_EGG_COMPONENT_HPP
#define ANTKEEPER_GAME_EGG_COMPONENT_HPP

/// Egg incubation parameters.
struct egg_component
{
	/// Duration of the incubation period, in days.
	float incubation_period{};
	
	/// Current incubation phase, on `[0, 1]`.
	float incubation_phase{};
};

#endif // ANTKEEPER_GAME_EGG_COMPONENT_HPP
