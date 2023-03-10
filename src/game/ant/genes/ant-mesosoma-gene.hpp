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

#ifndef ANTKEEPER_GAME_ANT_MESOSOMA_GENE_HPP
#define ANTKEEPER_GAME_ANT_MESOSOMA_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
#include <engine/render/model.hpp>
#include <memory>

/**
 * Ant mesosoma phene.
 *
 * @see https://www.antwiki.org/wiki/Morphological_Measurements
 */
struct ant_mesosoma_phene
{
	/// Pronotum width, in mesosomal lengths.
	float pronotum_width{0.0f};
	
	/// Degree of spinescence on the pronotum.
	float pronotum_spinescence{0.0f};
	
	/// Degree of spinescence on the mesonotum.
	float mesonotum_spinescence{0.0f};
	
	/// Degree of spinescence on the propodeum.
	float propodeum_spinescence{0.0f};
	
	/// 3D model of the mesosoma.
	std::shared_ptr<render::model> model;
};

/// Ant mesosoma gene.
using ant_mesosoma_gene = ant_gene<ant_mesosoma_phene>;

template <>
inline constexpr ant_gene_type ant_mesosoma_gene::type() const noexcept
{
	return ant_gene_type::mesosoma;
}

#endif // ANTKEEPER_GAME_ANT_MESOSOMA_GENE_HPP
