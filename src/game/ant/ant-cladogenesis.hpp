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

#ifndef ANTKEEPER_GAME_ANT_CLADOGENESIS_HPP
#define ANTKEEPER_GAME_ANT_CLADOGENESIS_HPP

#include "game/ant/ant-genome.hpp"
#include "game/ant/ant-gene-pool.hpp"
#include <memory>
#include <random>

/**
 * Generates a genome from a gene pool.
 *
 * @param pool Gene pool.
 * @param rng Random number generator.
 *
 * @return New genome.
 */
[[nodiscard]] std::unique_ptr<ant_genome> ant_cladogenesis(const ant_gene_pool& pool, std::random_device& rng);

#endif // ANTKEEPER_GAME_ANT_CLADOGENESIS_HPP
