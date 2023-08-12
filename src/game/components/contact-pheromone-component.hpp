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

#ifndef ANTKEEPER_CONTACT_PHEROMONE_COMPONENT_HPP
#define ANTKEEPER_CONTACT_PHEROMONE_COMPONENT_HPP

#include <cstdint>

/**
 * Pheromones that coat an exoskeleton, providing short-range chemical communication.
 */
struct contact_pheromone_component
{
	/// Bit mask representing a cuticular hydrocarbon (CHC) profile.
	std::uint32_t profile{};
};

#endif // ANTKEEPER_CONTACT_PHEROMONE_COMPONENT_HPP
