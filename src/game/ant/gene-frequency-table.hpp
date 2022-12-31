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

#ifndef ANTKEEPER_GAME_ANT_GENE_FREQUENCY_TABLE_HPP
#define ANTKEEPER_GAME_ANT_GENE_FREQUENCY_TABLE_HPP

#include <random>
#include <vector>

namespace game {
namespace ant {
	
/**
 * Gene frequency table.
 *
 * @tparam T Gene type.
 */
template <class T>
struct gene_frequency_table
{
	/// Gene array.
	std::vector<const T*> genes;
	
	/// Weight array
	std::vector<float> weights;
	
	/**
	 * Samples a gene from the frequency table.
	 *
	 * @tparam Generator Uniform random bit generator type.
	 *
	 * @param g Uniform random bit generator object.
	 *
	 * @return Randomly sampled gene.
	 */
	template <class Generator>
	const T* sample(Generator& g) const
	{
		if (genes.empty())
			return nullptr;
		
		std::discrete_distribution<std::size_t> distribution(weights.begin(), weights.end());
		return genes[distribution(g)];
	}
};

} // namespace ant
} // namespace game

#endif // ANTKEEPER_GAME_ANT_GENE_FREQUENCY_TABLE_HPP
