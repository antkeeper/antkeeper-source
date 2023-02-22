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

#ifndef ANTKEEPER_GAME_ANT_PHENOME_HPP
#define ANTKEEPER_GAME_ANT_PHENOME_HPP

#include "game/ant/phene/antennae.hpp"
#include "game/ant/phene/body-size.hpp"
#include "game/ant/phene/cocoon.hpp"
#include "game/ant/phene/diet.hpp"
#include "game/ant/phene/egg.hpp"
#include "game/ant/phene/eyes.hpp"
#include "game/ant/phene/foraging-time.hpp"
#include "game/ant/phene/founding-mode.hpp"
#include "game/ant/phene/gaster.hpp"
#include "game/ant/phene/head.hpp"
#include "game/ant/phene/larva.hpp"
#include "game/ant/phene/legs.hpp"
#include "game/ant/phene/mandibles.hpp"
#include "game/ant/phene/mesosoma.hpp"
#include "game/ant/phene/nest-site.hpp"
#include "game/ant/phene/ocelli.hpp"
#include "game/ant/phene/pigmentation.hpp"
#include "game/ant/phene/pilosity.hpp"
#include "game/ant/phene/sculpturing.hpp"
#include "game/ant/phene/sting.hpp"
#include "game/ant/phene/waist.hpp"
#include "game/ant/phene/wings.hpp"
#include "game/ant/genome.hpp"
#include "game/ant/caste.hpp"

namespace ant {

/**
 * Complete set of ant phenes.
 */
struct phenome
{
	/**
	 * Constructs a phenome for a given caste.
	 *
	 * @param genome Ant genome.
	 * @param caste Ant caste.
	 */
	phenome(const genome& genome, caste caste);
	
	/// Constructs an empty phenome.
	phenome();
	
	const phene::antennae* antennae;
	const phene::body_size* body_size;
	const phene::cocoon* cocoon;
	const phene::diet* diet;
	const phene::egg* egg;
	const phene::eyes* eyes;
	const phene::foraging_time* foraging_time;
	const phene::founding_mode* founding_mode;
	const phene::gaster* gaster;
	const phene::head* head;
	const phene::larva* larva;
	const phene::legs* legs;
	const phene::mandibles* mandibles;
	const phene::mesosoma* mesosoma;
	const phene::nest_site* nest_site;
	const phene::ocelli* ocelli;
	const phene::pigmentation* pigmentation;
	const phene::pilosity* pilosity;
	const phene::sculpturing* sculpturing;
	const phene::sting* sting;
	const phene::waist* waist;
	const phene::wings* wings;
};

} // namespace ant

#endif // ANTKEEPER_GAME_ANT_PHENOME_HPP
