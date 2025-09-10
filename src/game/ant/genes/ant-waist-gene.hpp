// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_WAIST_GENE_HPP
#define ANTKEEPER_GAME_ANT_WAIST_GENE_HPP

#include "game/ant/genes/ant-gene.hpp"
#include <engine/render/model.hpp>
#include <memory>

using namespace engine;

/// Trait that describes the waist (petiole plus postpetiole) of an ant.
/// @see https://www.antwiki.org/wiki/Morphological_Measurements
struct ant_waist_phene
{
	/// Waist presence.
	bool present{false};
	
	/// Petiole length, in mesosomal lengths.
	float petiole_length{0.0f};
	
	/// Petiole width, in mesosomal lengths.
	float petiole_width{0.0f};
	
	/// Petiole height, in mesosomal lengths.
	float petiole_height{0.0f};
	
	/// Degree of petiole spinescence.
	float petiole_spinescence{0.0f};
	
	/// Postpetiole presence.
	bool postpetiole_present{false};
	
	/// Postpetiole length, in mesosomal lengths.
	float postpetiole_length{0.0f};
	
	/// Postpetiole width, in mesosomal lengths.
	float postpetiole_width{0.0f};
	
	/// Postpetiole height, in mesosomal lengths.
	float postpetiole_height{0.0f};
	
	/// Degree of postpetiole spinescence
	float postpetiole_spinescence{0.0f};
	
	/// 3D model of the waist.
	/// 
	/// If the waist is present, the waist model skeleton can have one of two forms: single-segmented or double-segmented.
	/// 
	/// If the postpetiole is absent, the waist is single-segmented and its skeleton must contain the following bones:
	/// 
	/// - petiole
	///     - gaster_socket
	/// 
	/// If the postpetiole is present, the waist is double-segmented and its skeleton must contains the following bones:
	/// 
	/// - petiole
	///     - postpetiole
	///         - gaster_socket
	std::shared_ptr<render::model> model;
};

/// Ant waist gene.
using ant_waist_gene = ant_gene<ant_waist_phene>;

template <>
inline constexpr ant_gene_type ant_waist_gene::type() const noexcept
{
	return ant_gene_type::waist;
}

#endif // ANTKEEPER_GAME_ANT_WAIST_GENE_HPP
