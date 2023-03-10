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

#ifndef ANTKEEPER_GAME_ANT_COCOON_GENE_HPP
#define ANTKEEPER_GAME_ANT_COCOON_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
#include <engine/render/model.hpp>
#include <memory>

/**
 * Ant cocoon phene.
 */
struct ant_cocoon_phene
{
	/// Indicates whether a cocoon is formed by the larvae or not.
	bool present{false};
	
	/// 3D model of the cocoon, if present.
	std::shared_ptr<render::model> model;
};

/// Ant cocoon gene.
using ant_cocoon_gene = ant_gene<ant_cocoon_phene>;

template <>
inline constexpr ant_gene_type ant_cocoon_gene::type() const noexcept
{
	return ant_gene_type::cocoon;
}

#endif // ANTKEEPER_GAME_ANT_COCOON_GENE_HPP
