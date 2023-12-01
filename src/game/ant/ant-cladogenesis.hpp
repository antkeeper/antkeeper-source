// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_CLADOGENESIS_HPP
#define ANTKEEPER_GAME_ANT_CLADOGENESIS_HPP

#include "game/ant/ant-genome.hpp"
#include "game/ant/ant-gene-pool.hpp"
#include <memory>
#include <random>

/**
 * Generates an ant genome from a gene pool.
 *
 * @tparam URBG Uniform random bit generator type.
 *
 * @param pool Ant gene pool.
 * @param urbg Uniform random bit generator object.
 *
 * @return New genome.
 */
template <class URBG>
[[nodiscard]] std::unique_ptr<ant_genome> ant_cladogenesis(const ant_gene_pool& pool, URBG& urbg)
{
	// Allocate genome
	std::unique_ptr<ant_genome> genome = std::make_unique<ant_genome>();
	
	// Randomly sample genes
	genome->antennae = pool.antennae.sample(urbg);
	genome->body_size = pool.body_size.sample(urbg);
	genome->pupa = pool.pupa.sample(urbg);
	genome->diet = pool.diet.sample(urbg);
	genome->egg = pool.egg.sample(urbg);
	genome->eyes = pool.eyes.sample(urbg);
	genome->foraging_time = pool.foraging_time.sample(urbg);
	genome->founding_mode = pool.founding_mode.sample(urbg);
	genome->gaster = pool.gaster.sample(urbg);
	genome->head = pool.head.sample(urbg);
	genome->larva = pool.larva.sample(urbg);
	genome->legs = pool.legs.sample(urbg);
	genome->mandibles = pool.mandibles.sample(urbg);
	genome->mesosoma = pool.mesosoma.sample(urbg);
	genome->nest_site = pool.nest_site.sample(urbg);
	genome->ocelli = pool.ocelli.sample(urbg);
	genome->pigmentation = pool.pigmentation.sample(urbg);
	genome->pilosity = pool.pilosity.sample(urbg);
	genome->sculpturing = pool.sculpturing.sample(urbg);
	genome->sting = pool.sting.sample(urbg);
	genome->waist = pool.waist.sample(urbg);
	genome->wings = pool.wings.sample(urbg);
	
	return genome;
}

#endif // ANTKEEPER_GAME_ANT_CLADOGENESIS_HPP
