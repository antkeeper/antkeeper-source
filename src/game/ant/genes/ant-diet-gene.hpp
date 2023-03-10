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

#ifndef ANTKEEPER_GAME_ANT_DIET_GENE_HPP
#define ANTKEEPER_GAME_ANT_DIET_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"

/**
 * Ant diet phene.
 */
struct ant_diet_phene
{
	/// Preference for eating seeds.
	float seeds{0.0f};
	
	/// Preference for eating ant brood.
	float ant_brood{0.0f};
	
	/// Preference for eating arthropod eggs.
	float arthropod_eggs{0.0f};
	
	/// Preference for eating nectar.
	float nectar{0.0f};
	
	/// Preference for eating fungi.
	float fungi{0.0f};
	
	/// Preference for eating carrion.
	float carrion{0.0f};
};

/// Ant diet gene.
using ant_diet_gene = ant_gene<ant_diet_phene>;

template <>
inline constexpr ant_gene_type ant_diet_gene::type() const noexcept
{
	return ant_gene_type::diet;
}

#endif // ANTKEEPER_GAME_ANT_DIET_GENE_HPP
