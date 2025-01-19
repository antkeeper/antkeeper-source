// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_WINGS_GENE_HPP
#define ANTKEEPER_GAME_ANT_WINGS_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
#include <engine/render/model.hpp>
#include <memory>

/// Ant wings phene.
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
