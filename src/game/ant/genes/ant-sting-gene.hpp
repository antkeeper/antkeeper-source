// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_STING_GENE_HPP
#define ANTKEEPER_GAME_ANT_STING_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
import engine.render.model;
import <memory>;

using namespace engine;

/// Trait that describes the sting of an ant.
struct ant_sting_phene
{
	/// Indicates whether a sting present or not.
	bool present{false};
	
	/// 3D model of the sting.
	std::shared_ptr<render::model> model;
};

/// Ant sting gene.
using ant_sting_gene = ant_gene<ant_sting_phene>;

template <>
inline constexpr ant_gene_type ant_sting_gene::type() const noexcept
{
	return ant_gene_type::sting;
}

#endif // ANTKEEPER_GAME_ANT_STING_GENE_HPP
