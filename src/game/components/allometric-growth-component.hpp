// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ALLOMETRIC_GROWTH_COMPONENT_HPP
#define ANTKEEPER_GAME_ALLOMETRIC_GROWTH_COMPONENT_HPP

#include <engine/animation/bone.hpp>
#include <unordered_map>

/// Growth component with seperable rates for different body parts.
struct allometric_growth_component
{
	/// Growth rates of each bone.
	std::unordered_map<std::size_t, float> rates;
};

#endif // ANTKEEPER_GAME_ALLOMETRIC_GROWTH_COMPONENT_HPP
