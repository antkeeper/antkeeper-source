// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_GASTER_GENE_HPP
#define ANTKEEPER_GAME_ANT_GASTER_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
#include <engine/render/model.hpp>
#include <memory>

using namespace engine;

/// Ant gaster phene.
/// @see https://antwiki.org/wiki/Phragmosis
struct ant_gaster_phene
{
	/// Degree of phragmosis.
	float phragmosis{0.0f};
	
	/// 3D model of the gaster.
	/// 
	/// The gaster model skeleton must contain the following bones:
	/// 
	/// * gaster
	/// * * sting_socket
	std::shared_ptr<render::model> model;
};

/// Ant gaster gene.
using ant_gaster_gene = ant_gene<ant_gaster_phene>;

template <>
inline constexpr ant_gene_type ant_gaster_gene::type() const noexcept
{
	return ant_gene_type::gaster;
}

#endif // ANTKEEPER_GAME_ANT_GASTER_GENE_HPP
