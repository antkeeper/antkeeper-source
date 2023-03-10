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

#ifndef ANTKEEPER_GAME_ANT_BODY_SIZE_GENE_HPP
#define ANTKEEPER_GAME_ANT_BODY_SIZE_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"

/**
 * Ant body size phene.
 *
 * @see https://www.antwiki.org/wiki/Morphological_Measurements
 */
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
