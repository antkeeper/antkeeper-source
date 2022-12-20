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

#ifndef ANTKEEPER_GAME_ANT_CASTE_HPP
#define ANTKEEPER_GAME_ANT_CASTE_HPP

#include "game/ant/caste-type.hpp"
#include "game/ant/trait/antennae.hpp"
#include "game/ant/trait/eyes.hpp"
#include "game/ant/trait/gaster.hpp"
#include "game/ant/trait/head.hpp"
#include "game/ant/trait/legs.hpp"
#include "game/ant/trait/mandibles.hpp"
#include "game/ant/trait/mesosoma.hpp"
#include "game/ant/trait/ocelli.hpp"
#include "game/ant/trait/pigmentation.hpp"
#include "game/ant/trait/sculpturing.hpp"
#include "game/ant/trait/size-variation.hpp"
#include "game/ant/trait/sting.hpp"
#include "game/ant/trait/waist.hpp"
#include "game/ant/trait/wings.hpp"
#include <optional>

namespace game {
namespace ant {

/**
 * Ant caste description.
 */
struct caste
{
	/// Caste type.
	caste_type type;
	
	/// Antennae description.
	const trait::antennae* antennae;
	
	/// Eyes description.
	const trait::eyes* eyes;
	
	/// Gaster description.
	const trait::gaster* gaster;
	
	/// Head description.
	const trait::head* head;
	
	/// Legs description.
	const trait::legs* legs;
	
	/// Mandibles description.
	const trait::mandibles* mandibles;
	
	/// Mesosoma description.
	const trait::mesosoma* mesosoma;
	
	/// Ocelli description.
	const trait::ocelli* ocelli;
	
	/// Pigmentation description.
	const trait::pigmentation* pigmentation;
	
	/// Sculpturing description.
	const trait::sculpturing* sculpturing;
	
	/// Size variation description.
	const trait::size_variation* size_variation;
	
	/// Sting description.
	const trait::sting* sting;
	
	/// Waist description.
	const trait::waist* waist;
	
	/// Wings description.
	const trait::wings* wings;
};

} // namespace ant
} // namespace game

#endif // ANTKEEPER_GAME_ANT_CASTE_HPP
