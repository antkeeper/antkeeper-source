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

#ifndef ANTKEEPER_GAME_ANT_GENE_POLYPHENIC_GENE_HPP
#define ANTKEEPER_GAME_ANT_GENE_POLYPHENIC_GENE_HPP

#include "game/ant/caste.hpp"
#include <string>
#include <unordered_map>

namespace game {
namespace ant {
namespace gene {

/**
 * Gene with caste-specific phenes.
 *
 * @tparam T Phene type.
 *
 * @see https://en.wikipedia.org/wiki/Polyphenism
 */
template <class T>
struct polyphenic_gene
{
	/// Gene name.
	std::string name;
	
	/// Caste-specific phene definitions.
	std::unordered_map<caste, T> phenes;
};

} // namespace gene
} // namespace ant
} // namespace game

#endif // ANTKEEPER_GAME_ANT_GENE_POLYPHENIC_GENE_HPP