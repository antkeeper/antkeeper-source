// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_DIET_GENE_HPP
#define ANTKEEPER_GAME_ANT_DIET_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"

/// Ant diet phene.
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
