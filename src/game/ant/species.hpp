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

#ifndef ANTKEEPER_GAME_ANT_SPECIES_HPP
#define ANTKEEPER_GAME_ANT_SPECIES_HPP

#include "game/ant/caste.hpp"
#include "game/ant/trait/body-size.hpp"
#include "game/ant/trait/cocoon.hpp"
#include "game/ant/trait/diet.hpp"
#include "game/ant/trait/egg.hpp"
#include "game/ant/trait/foraging-time.hpp"
#include "game/ant/trait/larva.hpp"
#include "game/ant/trait/nest-site.hpp"
#include <optional>

namespace game {
namespace ant {

/**
 * Ant species description.
 */
struct species
{
	/// Body size description.
	const trait::body_size* body_size;
	
	/// Egg description.
	const trait::egg* egg;
	
	/// Larva description.
	const trait::larva* larva;
	
	/// Cocoon description.
	const trait::cocoon* cocoon;
	
	/// Queen caste description.
	std::optional<caste> queen_caste;
	
	/// Worker caste description.
	std::optional<caste> worker_caste;
	
	/// Soldier caste description.
	std::optional<caste> soldier_caste;
	
	/// Male caste description.
	std::optional<caste> male_caste;
	
	/// Diet description.
	const trait::diet* diet;
	
	/// Foraging time description.
	const trait::foraging_time* foraging_time;
	
	/// Nest site description.
	const trait::nest_site* nest_site;
};

} // namespace ant
} // namespace game

#endif // ANTKEEPER_GAME_ANT_SPECIES_HPP
