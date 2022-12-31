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

#ifndef ANTKEEPER_GAME_ANT_PHENE_MANDIBLES_HPP
#define ANTKEEPER_GAME_ANT_PHENE_MANDIBLES_HPP

#include "render/model.hpp"

namespace game {
namespace ant {
namespace phene {

/**
 * Ant mandibles phene.
 *
 * @see https://www.antwiki.org/wiki/Character_States_for_Extant_Ant_Genera_of_the_Formicidae
 * @see https://www.antwiki.org/wiki/Morphological_and_Functional_Diversity_of_Ant_Mandibles
 * @see https://www.antwiki.org/wiki/Morphological_Measurements
 */
struct mandibles
{
	/// 3D model of the mandibles.
	render::model* model;
	
	/// Mandible length at closure, in mesosomal lengths.
	float length;
	
	/// Number of teeth and denticles on the masticatory (apical) margin of the mandible.
	int apical_dental_count;
	
	/// Number of teeth and denticles on the basal margin of the mandible.
	int basal_dental_count;
};

} // namespace phene
} // namespace ant
} // namespace game

#endif // ANTKEEPER_GAME_ANT_PHENE_MANDIBLES_HPP
