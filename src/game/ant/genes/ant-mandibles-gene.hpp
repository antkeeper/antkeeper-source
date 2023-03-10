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

#ifndef ANTKEEPER_GAME_ANT_MANDIBLES_GENE_HPP
#define ANTKEEPER_GAME_ANT_MANDIBLES_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
#include <engine/render/model.hpp>
#include <memory>
#include <cstdint>

/**
 * Ant mandibles phene.
 *
 * @see https://www.antwiki.org/wiki/Character_States_for_Extant_Ant_Genera_of_the_Formicidae
 * @see https://www.antwiki.org/wiki/Morphological_and_Functional_Diversity_of_Ant_Mandibles
 * @see https://www.antwiki.org/wiki/Morphological_Measurements
 */
struct ant_mandibles_phene
{
	/// Mandible length at closure, in mesosomal lengths.
	float length{0.0f};
	
	/// Number of teeth and denticles on the masticatory (apical) margin of the mandible.
	std::uint8_t apical_dental_count{0};
	
	/// Number of teeth and denticles on the basal margin of the mandible.
	std::uint8_t basal_dental_count{0};
	
	/// 3D model of the mandibles.
	std::shared_ptr<render::model> model;
};

/// Ant mandibles gene.
using ant_mandibles_gene = ant_gene<ant_mandibles_phene>;

template <>
inline constexpr ant_gene_type ant_mandibles_gene::type() const noexcept
{
	return ant_gene_type::mandibles;
}

#endif // ANTKEEPER_GAME_ANT_MANDIBLES_GENE_HPP
