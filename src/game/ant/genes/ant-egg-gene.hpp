// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_EGG_GENE_HPP
#define ANTKEEPER_GAME_ANT_EGG_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
#include <engine/render/model.hpp>
#include <memory>

/// Ant egg phene.
struct ant_egg_phene
{
	/// Duration required for the embryo to develop into a larva, in days.
	float incubation_period{};
	
	/// Duration required for the contained larva to hatch, in days.
	float eclosion_period{};
	
	/// 3D model of the egg.
	std::shared_ptr<render::model> model;
};

/// Ant egg gene.
using ant_egg_gene = ant_gene<ant_egg_phene>;

template <>
inline constexpr ant_gene_type ant_egg_gene::type() const noexcept
{
	return ant_gene_type::egg;
}

#endif // ANTKEEPER_GAME_ANT_EGG_GENE_HPP
