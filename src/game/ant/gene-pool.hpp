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

#ifndef ANTKEEPER_GAME_ANT_GENE_POOL_HPP
#define ANTKEEPER_GAME_ANT_GENE_POOL_HPP

#include "game/ant/gene-frequency-table.hpp"
#include "game/ant/gene/antennae.hpp"
#include "game/ant/gene/body-size.hpp"
#include "game/ant/gene/cocoon.hpp"
#include "game/ant/gene/diet.hpp"
#include "game/ant/gene/egg.hpp"
#include "game/ant/gene/eyes.hpp"
#include "game/ant/gene/foraging-time.hpp"
#include "game/ant/gene/founding-mode.hpp"
#include "game/ant/gene/gaster.hpp"
#include "game/ant/gene/head.hpp"
#include "game/ant/gene/larva.hpp"
#include "game/ant/gene/legs.hpp"
#include "game/ant/gene/mandibles.hpp"
#include "game/ant/gene/mesosoma.hpp"
#include "game/ant/gene/nest-site.hpp"
#include "game/ant/gene/ocelli.hpp"
#include "game/ant/gene/pigmentation.hpp"
#include "game/ant/gene/pilosity.hpp"
#include "game/ant/gene/sculpturing.hpp"
#include "game/ant/gene/sting.hpp"
#include "game/ant/gene/waist.hpp"
#include "game/ant/gene/wings.hpp"

namespace game {
namespace ant {

/**
 * Pool of genes from which ant genomes can be generated.
 */
struct gene_pool
{
	/// Gene pool name.
	std::string name;
	
	gene_frequency_table<gene::antennae> antennae;
	gene_frequency_table<gene::body_size> body_size;
	gene_frequency_table<gene::cocoon> cocoon;
	gene_frequency_table<gene::diet> diet;
	gene_frequency_table<gene::egg> egg;
	gene_frequency_table<gene::eyes> eyes;
	gene_frequency_table<gene::foraging_time> foraging_time;
	gene_frequency_table<gene::founding_mode> founding_mode;
	gene_frequency_table<gene::gaster> gaster;
	gene_frequency_table<gene::head> head;
	gene_frequency_table<gene::larva> larva;
	gene_frequency_table<gene::legs> legs;
	gene_frequency_table<gene::mandibles> mandibles;
	gene_frequency_table<gene::mesosoma> mesosoma;
	gene_frequency_table<gene::nest_site> nest_site;
	gene_frequency_table<gene::ocelli> ocelli;
	gene_frequency_table<gene::pigmentation> pigmentation;
	gene_frequency_table<gene::pilosity> pilosity;
	gene_frequency_table<gene::sculpturing> sculpturing;
	gene_frequency_table<gene::sting> sting;
	gene_frequency_table<gene::waist> waist;
	gene_frequency_table<gene::wings> wings;
};

} // namespace ant
} // namespace game

#endif // ANTKEEPER_GAME_ANT_GENE_POOL_HPP
