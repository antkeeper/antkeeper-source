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

#ifndef ANTKEEPER_GAME_ANT_GENE_HPP
#define ANTKEEPER_GAME_ANT_GENE_HPP

#include "game/ant/ant-caste-type.hpp"
#include "game/ant/genes/ant-gene-type.hpp"
#include <vector>
#include <unordered_map>
#include <engine/utility/hash/fnv1a.hpp>

/**
 * Abstract base class for ant genes.
 */
struct ant_gene_base
{
	/// Returns the ant gene type.
	[[nodiscard]] virtual constexpr ant_gene_type type() const noexcept = 0;
	
	/// 32-bit FNV-1a hash of gene name.
	hash::fnv1a32_t name;
};

/**
 * Ant gene with caste-specific phenes.
 *
 * @tparam T Ant phene type.
 *
 * @see https://en.wikipedia.org/wiki/Polyphenism
 */
template <class T>
struct ant_gene: public ant_gene_base
{
	[[nodiscard]] constexpr ant_gene_type type() const noexcept override;
	
	/// Phene definitions
	std::vector<T> phenes;
	
	/// Map of ant caste types to phenes.
	std::unordered_map<ant_caste_type, T*> phene_map;
};

#endif // ANTKEEPER_GAME_ANT_GENE_HPP
