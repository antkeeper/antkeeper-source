// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_NEST_SITE_GENE_HPP
#define ANTKEEPER_GAME_ANT_NEST_SITE_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
import engine.utility.sized_types;

/// Ant nest site phene.
enum class ant_nest_site_phene: u8
{
	hypogeic,
	arboreal
};

/// Ant nest site gene.
using ant_nest_site_gene = ant_gene<ant_nest_site_phene>;

template <>
inline constexpr ant_gene_type ant_nest_site_gene::type() const noexcept
{
	return ant_gene_type::nest_site;
}

#endif // ANTKEEPER_GAME_ANT_NEST_SITE_GENE_HPP
