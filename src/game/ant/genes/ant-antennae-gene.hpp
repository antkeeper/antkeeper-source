// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_ANTENNAE_GENE_HPP
#define ANTKEEPER_GAME_ANT_ANTENNAE_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
import engine.render.model;
import engine.utility.sized_types;
import <memory>;

using namespace engine;

/// Ant antennae phene.
/// 
/// @see https://www.antwiki.org/wiki/Character_States_for_Extant_Ant_Genera_of_the_Formicidae
/// @see https://www.antwiki.org/wiki/Morphological_Measurements
struct ant_antennae_phene
{
	/// Total number of antennal segments per antenna.
	u8 total_antennomere_count{0};
	
	/// Number of antennal segments that constitute a club.
	u8 club_antennomere_count{0};
	
	/// 3D model of the antennae.
	/// 
	/// The antennae model skeleton must contain the following bones:
	/// 
	/// * antennomere1_l
	/// * * antennomere2_l
	/// * * * ...
	/// * * * * antennomere`<n>`_l
	/// * antennomere1_r
	/// * * antennomere2_r
	/// * * * ...
	/// * * * * antennomere`<n>`_r
	/// 
	/// Where `<n>` is the total number of antennal segments per antenna. 
	std::shared_ptr<render::model> model;
};

/// Ant antennae gene.
using ant_antennae_gene = ant_gene<ant_antennae_phene>;

template <>
inline constexpr ant_gene_type ant_antennae_gene::type() const noexcept
{
	return ant_gene_type::antennae;
}

#endif // ANTKEEPER_GAME_ANT_ANTENNAE_GENE_HPP
