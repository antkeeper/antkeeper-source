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

#ifndef ANTKEEPER_GAME_ANT_SPECIES_HPP
#define ANTKEEPER_GAME_ANT_SPECIES_HPP

#include "game/ant/ant-caste-type.hpp"
#include "game/ant/ant-phenome.hpp"
#include <engine/render/model.hpp>
#include <unordered_map>
#include <memory>

struct ant_species
{
	/// Caste-specific phenomes
	std::unordered_map<ant_caste_type, ant_phenome> phenomes;
	
	/// Caste-specific models
	std::unordered_map<caste, std::shared_ptr<render::model>> models;
};

#endif // ANTKEEPER_GAME_ANT_SPECIES_HPP
