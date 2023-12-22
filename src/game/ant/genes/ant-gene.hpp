// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_GENE_HPP
#define ANTKEEPER_GAME_ANT_GENE_HPP

#include "game/ant/ant-caste-type.hpp"
#include "game/ant/genes/ant-gene-type.hpp"
#include <vector>
#include <unordered_map>
#include <engine/hash/fnv1a.hpp>

/**
 * Abstract base class for ant genes.
 */
struct ant_gene_base
{
	/** Destructs an ant gene base. */
	virtual ~ant_gene_base() = default;
	
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
	/** Destructs an ant gene. */
	virtual ~ant_gene() = default;
	
	[[nodiscard]] constexpr ant_gene_type type() const noexcept override;
	
	/// Phene definitions
	std::vector<T> phenes;
	
	/// Map of ant caste types to phenes.
	std::unordered_map<ant_caste_type, T*> phene_map;
};

#endif // ANTKEEPER_GAME_ANT_GENE_HPP
