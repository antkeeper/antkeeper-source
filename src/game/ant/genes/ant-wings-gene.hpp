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

#ifndef ANTKEEPER_GAME_ANT_WINGS_GENE_HPP
#define ANTKEEPER_GAME_ANT_WINGS_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
#include <engine/render/model.hpp>
#include <memory>

/**
 * Ant wings phene.
 */
struct ant_wings_phene
{
	/// Wings presence.
	bool present{false};
	
	/// Forewing length, in mesosomal lengths.
	float forewing_length{0.0f};
	
	/// Forewing width, in mesosomal lengths.
	float forewing_width{0.0f};
	
	/// Degree of forewing venation. A value of `1.0` indicates a highly-developed venation pattern, while `0.0` indicates a complete absence of visible venation.
	float forewing_venation{0.0f};
	
	/// Hindwing length, in mesosomal lengths.
	float hindwing_length{0.0f};
	
	/// Hindwing width, in mesosomal lengths.
	float hindwing_width{0.0f};
	
	/// Degree of hindwing venation. A value of `1.0` indicates a highly-developed venation pattern, while `0.0` indicates a complete absence of visible venation.
	float hindwing_venation{0.0f};
	
	/// 3D model of the wings.
	std::shared_ptr<render::model> model;
};

/// Ant wings gene.
using ant_wings_gene = ant_gene<ant_wings_phene>;

template <>
inline constexpr ant_gene_type ant_wings_gene::type() const noexcept
{
	return ant_gene_type::wings;
}

#endif // ANTKEEPER_GAME_ANT_WINGS_GENE_HPP
