// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_PIGMENTATION_GENE_HPP
#define ANTKEEPER_GAME_ANT_PIGMENTATION_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
#include <engine/render/material.hpp>
#include <memory>

/// Ant pigmentation phene.
struct ant_pigmentation_phene
{
	/// Pigmentation material
	std::shared_ptr<render::material> material;
};

/// Ant pigmentation gene.
using ant_pigmentation_gene = ant_gene<ant_pigmentation_phene>;

template <>
inline constexpr ant_gene_type ant_pigmentation_gene::type() const noexcept
{
	return ant_gene_type::pigmentation;
}

#endif // ANTKEEPER_GAME_ANT_PIGMENTATION_GENE_HPP
