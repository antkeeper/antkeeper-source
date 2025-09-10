// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_GENE_FREQUENCY_TABLE_HPP
#define ANTKEEPER_GAME_ANT_GENE_FREQUENCY_TABLE_HPP

#include <random>
#include <engine/utility/sized-types.hpp>
#include <vector>
#include <memory>

using namespace engine;
	
/// Ant gene frequency table.
/// @tparam T Gene type.
template <class T>
struct ant_gene_frequency_table
{
	/// Gene array.
	std::vector<std::shared_ptr<T>> genes;
	
	/// Weight array
	std::vector<float> weights;
	
	/// Samples a gene from the frequency table.
	/// @tparam URBG Uniform random bit generator type.
	/// @param urbg Uniform random bit generator object.
	/// @return Randomly sampled gene.
	template <class URBG>
	[[nodiscard]] std::shared_ptr<T> sample(URBG& urbg) const
	{
		if (genes.empty())
		{
			return nullptr;
		}
		
		std::discrete_distribution<usize> distribution(weights.begin(), weights.end());
		
		return genes[distribution(urbg)];
	}
};

#endif // ANTKEEPER_GAME_ANT_GENE_FREQUENCY_TABLE_HPP
