// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_MESOSOMA_GENE_HPP
#define ANTKEEPER_GAME_ANT_MESOSOMA_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
#include <engine/render/model.hpp>
#include <memory>

/// Ant mesosoma phene.
/// @see https://www.antwiki.org/wiki/Morphological_Measurements
struct ant_mesosoma_phene
{
	/// Pronotum width, in mesosomal lengths.
	float pronotum_width{0.0f};
	
	/// Degree of spinescence on the pronotum.
	float pronotum_spinescence{0.0f};
	
	/// Degree of spinescence on the mesonotum.
	float mesonotum_spinescence{0.0f};
	
	/// Degree of spinescence on the propodeum.
	float propodeum_spinescence{0.0f};
	
	/// 3D model of the mesosoma.
	/// 
	/// The mesosoma model skeleton must contain the following bones:
	/// 
	/// * mesosoma
	/// * * head_socket
	/// * * procoxa_socket_l
	/// * * procoxa_socket_r
	/// * * mesocoxa_socket_l
	/// * * mesocoxa_socket_r
	/// * * metacoxa_socket_l
	/// * * metacoxa_socket_r
	/// * * petiole_socket
	/// 
	/// In alate phenes, the mesosoma model skeleton must additionally contain the following bones:
	/// 
	/// * forewing_socket_l
	/// * forewing_socket_r
	/// * hindwing_socket_l
	/// * hindwing_socket_r
	std::shared_ptr<render::model> model;
};

/// Ant mesosoma gene.
using ant_mesosoma_gene = ant_gene<ant_mesosoma_phene>;

template <>
inline constexpr ant_gene_type ant_mesosoma_gene::type() const noexcept
{
	return ant_gene_type::mesosoma;
}

#endif // ANTKEEPER_GAME_ANT_MESOSOMA_GENE_HPP
