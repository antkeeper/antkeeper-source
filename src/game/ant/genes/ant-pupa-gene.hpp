// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_PUPA_GENE_HPP
#define ANTKEEPER_GAME_ANT_PUPA_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
#include <engine/render/model.hpp>
#include <memory>

/**
 * Ant pupa phene.
 */
struct ant_pupa_phene
{
	/// Duration required for the pupa to develop into an adult, in days.
	float development_period{};
	
	/// Duration required for the callow to emerge from the cocoon or become mobile, in days.
	float eclosion_period{};
	
	/// Indicates whether a cocoon is formed by the larvae or not.
	bool cocoon_present{false};
	
	/// 3D model of the cocoon, if present.
	std::shared_ptr<render::model> cocoon_model;
};

/// Ant pupa gene.
using ant_pupa_gene = ant_gene<ant_pupa_phene>;

template <>
inline constexpr ant_gene_type ant_pupa_gene::type() const noexcept
{
	return ant_gene_type::pupa;
}

#endif // ANTKEEPER_GAME_ANT_PUPA_GENE_HPP
