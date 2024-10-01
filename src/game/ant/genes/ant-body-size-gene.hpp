// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_BODY_SIZE_GENE_HPP
#define ANTKEEPER_GAME_ANT_BODY_SIZE_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"

/// Ant body size phene.
/// 
/// @see https://www.antwiki.org/wiki/Morphological_Measurements
struct ant_body_size_phene
{
	/// Minimum mesosoma length (Weber's length), in centimeters.
	float min_mesosoma_length{0.0f};
	
	/// Maximum mesosoma length (Weber's length), in centimeters.
	float max_mesosoma_length{0.0f};
	
	/// Mean mesosoma length (Weber's length), in centimeters.
	float mean_mesosoma_length{0.0f};
};

/// Ant body size gene.
using ant_body_size_gene = ant_gene<ant_body_size_phene>;

template <>
inline constexpr ant_gene_type ant_body_size_gene::type() const noexcept
{
	return ant_gene_type::body_size;
}

#endif // ANTKEEPER_GAME_ANT_BODY_SIZE_GENE_HPP
