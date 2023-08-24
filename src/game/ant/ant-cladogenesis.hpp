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
