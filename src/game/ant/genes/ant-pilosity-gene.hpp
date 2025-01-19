// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_PILOSITY_GENE_HPP
#define ANTKEEPER_GAME_ANT_PILOSITY_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"

/// Ant pilosity phene.
struct ant_pilosity_phene
{
	/// Hair density.
	float density{0.0f};
};

/// Ant pilosity gene.
using ant_pilosity_gene = ant_gene<ant_pilosity_phene>;

template <>
inline constexpr ant_gene_type ant_pilosity_gene::type() const noexcept
{
	return ant_gene_type::pilosity;
}

#endif // ANTKEEPER_GAME_ANT_PILOSITY_GENE_HPP
