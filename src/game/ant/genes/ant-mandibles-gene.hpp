// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
