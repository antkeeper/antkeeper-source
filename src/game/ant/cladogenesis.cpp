/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "game/ant/cladogenesis.hpp"

namespace game {
namespace ant {

genome* cladogenesis(const gene_pool& pool, std::random_device& rng)
{
	// Allocate genome
	ant::genome* genome = new ant::genome();
	
	// Randomly sample genes
	genome->antennae = pool.antennae.sample(rng);
	genome->body_size = pool.body_size.sample(rng);
	genome->cocoon = pool.cocoon.sample(rng);
	genome->diet = pool.diet.sample(rng);
	genome->egg = pool.egg.sample(rng);
	genome->eyes = pool.eyes.sample(rng);
	genome->foraging_time = pool.foraging_time.sample(rng);
	genome->founding_mode = pool.founding_mode.sample(rng);
	genome->gaster = pool.gaster.sample(rng);
	genome->head = pool.head.sample(rng);
	genome->larva = pool.larva.sample(rng);
	genome->legs = pool.legs.sample(rng);
	genome->mandibles = pool.mandibles.sample(rng);
	genome->mesosoma = pool.mesosoma.sample(rng);
	genome->nest_site = pool.nest_site.sample(rng);
	genome->ocelli = pool.ocelli.sample(rng);
	genome->pigmentation = pool.pigmentation.sample(rng);
	genome->pilosity = pool.pilosity.sample(rng);
	genome->sculpturing = pool.sculpturing.sample(rng);
	genome->sting = pool.sting.sample(rng);
	genome->waist = pool.waist.sample(rng);
	genome->wings = pool.wings.sample(rng);
	
	return genome;
}

} // namespace ant
} // namespace game
