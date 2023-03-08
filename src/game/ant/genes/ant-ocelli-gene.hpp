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

#ifndef ANTKEEPER_GAME_ANT_PHENE_OCELLI_GENE_HPP
#define ANTKEEPER_GAME_ANT_PHENE_OCELLI_GENE_HPP

#include "game/ant/genes/polyphenic-ant-gene.hpp"
#include <engine/render/model.hpp>
#include <memory>

/**
 * Ant ocelli phene.
 */
struct ant_ocelli_phene
{
	/// 3D model of the lateral ocelli, if present.
	std::shared_ptr<render::model> lateral_ocelli_model;
	
	/// 3D model of the median ocellus, if present.
	std::shared_ptr<render::model> median_ocellus_model;
	
	/// Lateral ocelli present.
	bool lateral_ocelli_present;
	
	/// Median ocellus present.
	bool median_ocellus_present;
	
	/// Ocellus width, in mesosomal lengths.
	float width;
	
	/// Ocellus height, in mesosomal lengths.
	float height;
};

/// Ant ocelli gene.
using ant_ocelli_gene = polyphenic_ant_gene<ant_ocelli_phene>;

#endif // ANTKEEPER_GAME_ANT_PHENE_OCELLI_GENE_HPP
