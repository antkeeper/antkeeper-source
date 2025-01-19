// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_FOUNDING_MODE_GENE_HPP
#define ANTKEEPER_GAME_ANT_FOUNDING_MODE_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
#include <cstdint>

/// Colony founding mode phene.
/// @see https://www.antwiki.org/wiki/Colony_Foundation
enum class ant_founding_mode_phene: std::uint8_t
{
	/// Foraging outside the claustral chamber is required.
	semi_claustral,
	
	/// Foraging outside the claustral chamber is unnecessary.
	claustral,
	
	/// Parent colony splits, with a queen taking a portion of the workforce with her.
	fission
};

/// Ant founding mode gene.
using ant_founding_mode_gene = ant_gene<ant_founding_mode_phene>;

template <>
inline constexpr ant_gene_type ant_founding_mode_gene::type() const noexcept
{
	return ant_gene_type::founding_mode;
}

#endif // ANTKEEPER_GAME_ANT_FOUNDING_MODE_GENE_HPP
