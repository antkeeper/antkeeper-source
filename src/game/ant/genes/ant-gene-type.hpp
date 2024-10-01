// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_GENE_TYPE_HPP
#define ANTKEEPER_GAME_ANT_GENE_TYPE_HPP

#include <cstdint>

/// Ant gene types.
enum class ant_gene_type: std::uint8_t
{
	antennae = 1,
	body_size,
	pupa,
	diet,
	egg,
	eyes,
	foraging_time,
	founding_mode,
	gaster,
	head,
	larva,
	legs,
	mandibles,
	mesosoma,
	nest_site,
	ocelli,
	pigmentation,
	pilosity,
	sculpturing,
	sting,
	waist,
	wings
};

#endif // ANTKEEPER_GAME_ANT_GENE_TYPE_HPP
