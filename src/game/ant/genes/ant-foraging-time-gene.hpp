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

#ifndef ANTKEEPER_GAME_ANT_FORAGING_TIME_GENE_HPP
#define ANTKEEPER_GAME_ANT_FORAGING_TIME_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"

/**
 * Ant foraging time phene.
 */
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
