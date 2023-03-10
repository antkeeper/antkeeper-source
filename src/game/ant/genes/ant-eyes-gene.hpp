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

#ifndef ANTKEEPER_GAME_ANT_EYES_GENE_HPP
#define ANTKEEPER_GAME_ANT_EYES_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
#include <engine/render/model.hpp>
#include <cstdint>
#include <memory>

/**
 * Ant eyes phene.
 *
 * @see https://www.antwiki.org/wiki/Morphological_Measurements
 */
struct ant_eyes_phene
{
	/// Indicates whether eyes are present.
	bool present{false};
	
	/// Number of ommatidia.
	std::uint32_t ommatidia_count{0};
	
	/// Eye length, in mesosomal lengths.
	float length{0.0f};
	
	/// Eye width, in mesosomal lengths.
	float width{0.0f};
	
	/// Eye height, in mesosomal lengths.
	float height{0.0f};
	
	/// 3D model of the eyes, if present.
	std::shared_ptr<render::model> model;
};

/// Ant eyes gene.
using ant_eyes_gene = ant_gene<ant_eyes_phene>;

template <>
inline constexpr ant_gene_type ant_eyes_gene::type() const noexcept
{
	return ant_gene_type::eyes;
}

#endif // ANTKEEPER_GAME_ANT_EYES_GENE_HPP
