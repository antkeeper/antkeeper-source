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

#ifndef ANTKEEPER_GAME_ANT_GENOME_HPP
#define ANTKEEPER_GAME_ANT_GENOME_HPP

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

namespace ant {

/**
 * Complete set of ant genes.
 */
struct genome
{
	/// Constructs an empty genome.
	genome();
	
	const gene::antennae* antennae;
	const gene::body_size* body_size;
	const gene::cocoon* cocoon;
	const gene::diet* diet;
	const gene::egg* egg;
	const gene::eyes* eyes;
	const gene::foraging_time* foraging_time;
	const gene::founding_mode* founding_mode;
	const gene::gaster* gaster;
	const gene::head* head;
	const gene::larva* larva;
	const gene::legs* legs;
	const gene::mandibles* mandibles;
	const gene::mesosoma* mesosoma;
	const gene::nest_site* nest_site;
	const gene::ocelli* ocelli;
	const gene::pigmentation* pigmentation;
	const gene::pilosity* pilosity;
	const gene::sculpturing* sculpturing;
	const gene::sting* sting;
	const gene::waist* waist;
	const gene::wings* wings;
};

} // namespace ant

#endif // ANTKEEPER_GAME_ANT_GENOME_HPP
