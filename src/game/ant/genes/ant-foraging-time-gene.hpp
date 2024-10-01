// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_FORAGING_TIME_GENE_HPP
#define ANTKEEPER_GAME_ANT_FORAGING_TIME_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"

/// Ant foraging time phene.
struct ant_foraging_time_phene
{
	/// Minimum solar altitude, in radians, at which foraging occurs.
	float min_solar_altitude{0.0f};
	
	/// Maximum solar alttiude, in radians, at which foraging occurs.
	float max_solar_altitude{0.0f};
};

/// Ant foraging time gene.
using ant_foraging_time_gene = ant_gene<ant_foraging_time_phene>;

template <>
inline constexpr ant_gene_type ant_foraging_time_gene::type() const noexcept
{
	return ant_gene_type::foraging_time;
}

#endif // ANTKEEPER_GAME_ANT_FORAGING_TIME_GENE_HPP
