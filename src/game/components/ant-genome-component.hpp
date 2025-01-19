// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_GENOME_COMPONENT_HPP
#define ANTKEEPER_GAME_ANT_GENOME_COMPONENT_HPP

#include "game/ant/ant-genome.hpp"
#include <memory>

struct ant_genome_component
{
	/// Shared pointer to the ant genome.
	std::shared_ptr<ant_genome> genome;
};

#endif // ANTKEEPER_GAME_ANT_GENOME_COMPONENT_HPP
