// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_LARVA_GENE_HPP
#define ANTKEEPER_GAME_ANT_LARVA_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
#include <engine/render/model.hpp>
#include <engine/utility/sized-types.hpp>
#include <memory>

using namespace engine;

/// Ant larva phene.
struct ant_larva_phene
{
	/// Duration required for the larva to develop into a pupa, in days.
	float development_period{};
	
	/// Duration required for the larva to spin a cocoon, in days.
	float spinning_period{};
	
	/// Number of larval instars before pupation.
	u8 instar_count{};
	
	/// Scale of the first larva instar, relative to the final larval instar.
	float first_instar_scale{};
	
	/// 3D model of the larva.
	std::shared_ptr<render::model> model;
};

/// Ant larva gene.
using ant_larva_gene = ant_gene<ant_larva_phene>;

template <>
inline constexpr ant_gene_type ant_larva_gene::type() const noexcept
{
	return ant_gene_type::larva;
}

#endif // ANTKEEPER_GAME_ANT_LARVA_GENE_HPP
