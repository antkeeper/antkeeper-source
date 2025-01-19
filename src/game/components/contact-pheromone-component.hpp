// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_CONTACT_PHEROMONE_COMPONENT_HPP
#define ANTKEEPER_CONTACT_PHEROMONE_COMPONENT_HPP

#include <cstdint>

/// Pheromones that coat an exoskeleton, providing short-range chemical communication.
struct contact_pheromone_component
{
	/// Bit mask representing a cuticular hydrocarbon (CHC) profile.
	std::uint32_t profile{};
};

#endif // ANTKEEPER_CONTACT_PHEROMONE_COMPONENT_HPP
