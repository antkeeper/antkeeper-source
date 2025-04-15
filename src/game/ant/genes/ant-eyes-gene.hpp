// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_EYES_GENE_HPP
#define ANTKEEPER_GAME_ANT_EYES_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
import engine.render.model;
import engine.utility.sized_types;
import <memory>;

using namespace engine;

/// Ant eyes phene.
/// @see https://www.antwiki.org/wiki/Morphological_Measurements
struct ant_eyes_phene
{
	/// Indicates whether eyes are present.
	bool present{false};
	
	/// Number of ommatidia.
	u32 ommatidia_count{0};
	
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
