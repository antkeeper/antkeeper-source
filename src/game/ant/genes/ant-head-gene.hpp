// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_HEAD_GENE_HPP
#define ANTKEEPER_GAME_ANT_HEAD_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
#include <engine/render/model.hpp>
#include <memory>

using namespace engine;

/// Ant head phene.
/// @see https://www.antwiki.org/wiki/Morphological_Measurements
/// @see https://antwiki.org/wiki/Phragmosis
struct ant_head_phene
{
	/// Head length, in mesosomal lengths.
	float length{0.0f};
	
	/// Head width, in mesosomal lengths.
	float width{0.0f};
	
	/// Degree of phragmosis.
	float phragmosis{0.0f};
	
	/// 3D model of the head.
	/// 
	/// The head model skeleton must contain the following bones:
	/// 
	/// * head
	/// * * mandible_socket_l
	/// * * mandible_socket_r
	/// * * antenna_socket_l
	/// * * antenna_socket_r
	/// * * eye_socket_l
	/// * * eye_socket_r
	/// * * ocellus_socket_l
	/// * * ocellus_socket_r
	/// * * ocellus_socket_m
	std::shared_ptr<render::model> model;
};

/// Ant head gene.
using ant_head_gene = ant_gene<ant_head_phene>;

template <>
inline constexpr ant_gene_type ant_head_gene::type() const noexcept
{
	return ant_gene_type::head;
}

#endif // ANTKEEPER_GAME_ANT_HEAD_GENE_HPP
