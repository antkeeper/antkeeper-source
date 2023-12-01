// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_SCULPTURING_GENE_HPP
#define ANTKEEPER_GAME_ANT_SCULPTURING_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
#include <engine/gl/texture.hpp>
#include <memory>

/**
 * Ant surface sculpturing phene.
 */
struct ant_sculpturing_phene
{
	/// Surface roughness.
	float roughness{0.0f};
	
	/// Surface culpturing normal map.
	std::shared_ptr<gl::texture_2d> normal_map;
};

/// Ant sculpturing gene.
using ant_sculpturing_gene = ant_gene<ant_sculpturing_phene>;

template <>
inline constexpr ant_gene_type ant_sculpturing_gene::type() const noexcept
{
	return ant_gene_type::sculpturing;
}

#endif // ANTKEEPER_GAME_ANT_SCULPTURING_GENE_HPP
