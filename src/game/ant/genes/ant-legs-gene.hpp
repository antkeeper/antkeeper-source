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

#ifndef ANTKEEPER_GAME_ANT_LEGS_GENE_HPP
#define ANTKEEPER_GAME_ANT_LEGS_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
#include <engine/render/model.hpp>
#include <memory>

/**
 * Ant legs phene.
 *
 * @see https://www.antwiki.org/wiki/Morphological_Measurements
 */
struct ant_legs_phene
{
	/// Distance from the mesosoma to the ground when standing, in mesosomal lengths.
	float standing_height{};
	
	/// Creeping speed, in mesosomal lengths per second.
	float creeping_speed{};
	
	/// Walking speed, in mesosomal lengths per second.
	float walking_speed{};
	
	/// Running speed, in mesosomal lengths per second.
	float running_speed{};
	
	/// Distance covered in a single gait cycle, in mesosomal lengths.
	float stride_length{};
	
	/// Maximum angular frequency when turning, in radians per second.
	float max_angular_frequency{};
	
	/// Grip factor.
	float grip{};
	
	/**
	 * 3D model of the legs.
	 *
	 * The legs model skeleton must contain the following bone chains:
	 *
	 * * procoxa_l
	 * * * profemur_l
	 * * * * protibia_l
	 * * * * * protarsomere1_l
	 * * * * * * protarsomere2_l
	 * * * * * * * protarsomere3_l
	 * * * * * * * * protarsomere4_l
	 * * * * * * * * * protarsomere5_l
	 * * procoxa_r
	 * * * profemur_r
	 * * * * protibia_r
	 * * * * * protarsomere1_r
	 * * * * * * protarsomere2_r
	 * * * * * * * protarsomere3_r
	 * * * * * * * * protarsomere4_r
	 * * * * * * * * * protarsomere5_r
	 * * mesocoxa_l
	 * * * mesofemur_l
	 * * * * mesotibia_l
	 * * * * * mesotarsomere1_l
	 * * * * * * mesotarsomere2_l
	 * * * * * * * mesotarsomere3_l
	 * * * * * * * * mesotarsomere4_l
	 * * * * * * * * * mesotarsomere5_l
	 * * mesocoxa_r
	 * * * mesofemur_r
	 * * * * mesotibia_r
	 * * * * * mesotarsomere1_r
	 * * * * * * mesotarsomere2_r
	 * * * * * * * mesotarsomere3_r
	 * * * * * * * * mesotarsomere4_r
	 * * * * * * * * * mesotarsomere5_r
	 * * metacoxa_l
	 * * * metafemur_l
	 * * * * metatibia_l
	 * * * * * metatarsomere1_l
	 * * * * * * metatarsomere2_l
	 * * * * * * * metatarsomere3_l
	 * * * * * * * * metatarsomere4_l
	 * * * * * * * * * metatarsomere5_l
	 * * metacoxa_r
	 * * * metafemur_r
	 * * * * metatibia_r
	 * * * * * metatarsomere1_r
	 * * * * * * metatarsomere2_r
	 * * * * * * * metatarsomere3_r
	 * * * * * * * * metatarsomere4_r
	 * * * * * * * * * metatarsomere5_r
	 */
	std::shared_ptr<render::model> model;
};

/// Ant legs gene.
using ant_legs_gene = ant_gene<ant_legs_phene>;

template <>
inline constexpr ant_gene_type ant_legs_gene::type() const noexcept
{
	return ant_gene_type::legs;
}

#endif // ANTKEEPER_GAME_ANT_LEGS_GENE_HPP
