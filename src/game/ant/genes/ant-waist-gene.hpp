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

#ifndef ANTKEEPER_GAME_ANT_WAIST_GENE_HPP
#define ANTKEEPER_GAME_ANT_WAIST_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
#include <engine/render/model.hpp>
#include <memory>

/**
 * Trait that describes the waist (petiole plus postpetiole) of an ant.
 *
 * @see https://www.antwiki.org/wiki/Morphological_Measurements
 */
struct ant_waist_phene
{
	//// Petiole presence.
	bool petiole_present{false};
	
	/// Petiole length, in mesosomal lengths.
	float petiole_length{0.0f};
	
	/// Petiole width, in mesosomal lengths.
	float petiole_width{0.0f};
	
	/// Petiole height, in mesosomal lengths.
	float petiole_height{0.0f};
	
	/// Degree of petiole spinescence.
	float petiole_spinescence{0.0f};
	
	/// Postpetiole presence.
	bool postpetiole_present{false};
	
	/// Postpetiole length, in mesosomal lengths.
	float postpetiole_length{0.0f};
	
	/// Postpetiole width, in mesosomal lengths.
	float postpetiole_width{0.0f};
	
	/// Postpetiole height, in mesosomal lengths.
	float postpetiole_height{0.0f};
	
	/// Degree of postpetiole spinescence
	float postpetiole_spinescence{0.0f};
	
	/// 3D model of the waist.
	std::shared_ptr<render::model> model;
};

/// Ant waist gene.
using ant_waist_gene = ant_gene<ant_waist_phene>;

template <>
inline constexpr ant_gene_type ant_waist_gene::type() const noexcept
{
	return ant_gene_type::waist;
}

#endif // ANTKEEPER_GAME_ANT_WAIST_GENE_HPP
