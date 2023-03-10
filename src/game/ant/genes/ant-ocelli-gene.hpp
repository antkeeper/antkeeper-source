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

#include "game/ant/genes/ant-gene.hpp"
#include <engine/render/model.hpp>
#include <memory>

/**
 * Ant ocelli phene.
 */
struct ant_ocelli_phene
{
	/// Lateral ocelli present.
	bool lateral_ocelli_present{false};
	
	/// Lateral ocelli width, in mesosomal lengths.
	float lateral_ocelli_width{0.0f};
	
	/// Lateral ocelli height, in mesosomal lengths.
	float lateral_ocelli_height{0.0f};
	
	/// 3D model of the lateral ocelli, if present.
	std::shared_ptr<render::model> lateral_ocelli_model;
	
	/// Median ocellus present.
	bool median_ocellus_present{false};
	
	/// Median ocellus width, in mesosomal lengths.
	float median_ocellus_width{0.0f};
	
	/// Median ocellus height, in mesosomal lengths.
	float median_ocellus_height{0.0f};
	
	/// 3D model of the median ocellus, if present.
	std::shared_ptr<render::model> median_ocellus_model;
};

/// Ant ocelli gene.
using ant_ocelli_gene = ant_gene<ant_ocelli_phene>;

template <>
inline constexpr ant_gene_type ant_ocelli_gene::type() const noexcept
{
	return ant_gene_type::ocelli;
}

#endif // ANTKEEPER_GAME_ANT_PHENE_OCELLI_GENE_HPP
