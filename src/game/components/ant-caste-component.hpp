// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_CASTE_COMPONENT_HPP
#define ANTKEEPER_GAME_ANT_CASTE_COMPONENT_HPP

#include "game/ant/ant-caste-type.hpp"
#include "game/ant/ant-phenome.hpp"
#include <memory>

struct ant_caste_component
{
	/// Caste type.
	ant_caste_type type{};
	
	// Subcaste type.
	//ant_subcaste subtype;
	
	/// Shared pointer to the caste phenome.
	std::shared_ptr<ant_phenome> phenome;
};

#endif // ANTKEEPER_GAME_ANT_CASTE_COMPONENT_HPP
