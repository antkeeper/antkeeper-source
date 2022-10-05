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

#ifndef ANTKEEPER_GAME_COMPONENT_GENOME_HPP
#define ANTKEEPER_GAME_COMPONENT_GENOME_HPP

#include <string>
#include <vector>

namespace game {
namespace component {

/// Complete set of genetic material in an organism.
struct genome
{
	/**
	 * Number of complete sets of chromosomes in a cell.
	 *
	 * A value of `1` indicates the organism is haploid, `2` is diploid, `3` is triploid, etc.
	 */
	unsigned int ploidy;
	
	/**
	 * Set of DNA base sequences for every chromosomes in the genome.
	 *
	 * A DNA base sequence is a string of IUPAC DNA base symbols. Homologous chromosomes should be stored consecutively, such that in a diploid organism, a chromosome with an even index is homologous to the following chromosome.
	 */
	std::vector<std::string> chromosomes;
};

} // namespace component
} // namespace game

#endif // ANTKEEPER_GAME_COMPONENT_GENOME_HPP
