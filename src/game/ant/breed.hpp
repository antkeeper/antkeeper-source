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

#ifndef ANTKEEPER_GAME_ANT_BREED_HPP
#define ANTKEEPER_GAME_ANT_BREED_HPP

#include "game/ant/trait/antennae.hpp"
#include "game/ant/trait/cocoon.hpp"
#include "game/ant/trait/diet.hpp"
#include "game/ant/trait/egg.hpp"
#include "game/ant/trait/eyes.hpp"
#include "game/ant/trait/foraging-time.hpp"
#include "game/ant/trait/forewings.hpp"
#include "game/ant/trait/gaster.hpp"
#include "game/ant/trait/head.hpp"
#include "game/ant/trait/hindwings.hpp"
#include "game/ant/trait/larva.hpp"
#include "game/ant/trait/legs.hpp"
#include "game/ant/trait/mandibles.hpp"
#include "game/ant/trait/mesosoma.hpp"
#include "game/ant/trait/nest.hpp"
#include "game/ant/trait/ocelli.hpp"
#include "game/ant/trait/pigmentation.hpp"
#include "game/ant/trait/pilosity.hpp"
#include "game/ant/trait/sculpturing.hpp"
#include "game/ant/trait/sting.hpp"
#include "game/ant/trait/waist.hpp"

namespace game {
namespace ant {

/**
 * Set of all traits that describes an ant breed.
 */
struct breed
{
	// Morphological traits
	trait::antennae* antennae;
	trait::eyes* eyes;
	trait::forewings* forewings;
	trait::gaster* gaster;
	trait::waist* waist;
	trait::head* head;
	trait::hindwings* hindwings;
	trait::legs* legs;
	trait::mandibles* mandibles;
	trait::mesosoma* mesosoma;
	trait::ocelli* ocelli;
	trait::sting* sting;
	trait::sculpturing* sculpturing;
	trait::pigmentation* pigmentation;
	trait::pilosity* pilosity;
	trait::egg* egg;
	trait::larva* larva;
	trait::cocoon* cocoon;
	
	// Behavioral traits
	trait::diet* diet;
	trait::foraging_time* foraging_time;
	trait::nest* nest;
};

} // namespace ant
} // namespace game

#endif // ANTKEEPER_GAME_ANT_BREED_HPP
