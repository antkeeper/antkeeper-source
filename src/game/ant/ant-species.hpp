// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_SPECIES_HPP
#define ANTKEEPER_GAME_ANT_SPECIES_HPP

#include "game/ant/ant-caste-type.hpp"
#include "game/ant/ant-phenome.hpp"
#include <engine/render/model.hpp>
#include <unordered_map>
#include <memory>

using namespace engine;

struct ant_species
{
	/// Caste-specific phenomes
	std::unordered_map<ant_caste_type, ant_phenome> phenomes;
	
	/// Caste-specific models
	std::unordered_map<ant_caste_type, std::shared_ptr<render::model>> models;
};

#endif // ANTKEEPER_GAME_ANT_SPECIES_HPP
