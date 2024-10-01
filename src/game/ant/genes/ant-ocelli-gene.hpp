// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_PHENE_OCELLI_GENE_HPP
#define ANTKEEPER_GAME_ANT_PHENE_OCELLI_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
#include <engine/render/model.hpp>
#include <memory>

/// Ant ocelli phene.
struct ant_ocelli_phene
{
	/// Lateral ocelli present.
	bool lateral_ocelli_present{false};
	
	/// Lateral ocelli width, in mesosomal lengths.
	float lateral_ocelli_width{0.0f};
	
	/// Lateral ocelli height, in mesosomal lengths.
	float lateral_ocelli_height{0.0f};
	
	/// Median ocellus present.
	bool median_ocellus_present{false};
	
	/// Median ocellus width, in mesosomal lengths.
	float median_ocellus_width{0.0f};
	
	/// Median ocellus height, in mesosomal lengths.
	float median_ocellus_height{0.0f};
	
	/// 3D model of the ocelli, if present.
	std::shared_ptr<render::model> model;
};

/// Ant ocelli gene.
using ant_ocelli_gene = ant_gene<ant_ocelli_phene>;

template <>
inline constexpr ant_gene_type ant_ocelli_gene::type() const noexcept
{
	return ant_gene_type::ocelli;
}

#endif // ANTKEEPER_GAME_ANT_PHENE_OCELLI_GENE_HPP
