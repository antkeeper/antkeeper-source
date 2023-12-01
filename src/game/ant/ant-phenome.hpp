// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_PHENOME_HPP
#define ANTKEEPER_GAME_ANT_PHENOME_HPP

#include "game/ant/genes/ant-antennae-gene.hpp"
#include "game/ant/genes/ant-body-size-gene.hpp"
#include "game/ant/genes/ant-pupa-gene.hpp"
#include "game/ant/genes/ant-diet-gene.hpp"
#include "game/ant/genes/ant-egg-gene.hpp"
#include "game/ant/genes/ant-eyes-gene.hpp"
#include "game/ant/genes/ant-foraging-time-gene.hpp"
#include "game/ant/genes/ant-founding-mode-gene.hpp"
#include "game/ant/genes/ant-gaster-gene.hpp"
#include "game/ant/genes/ant-head-gene.hpp"
#include "game/ant/genes/ant-larva-gene.hpp"
#include "game/ant/genes/ant-legs-gene.hpp"
#include "game/ant/genes/ant-mandibles-gene.hpp"
#include "game/ant/genes/ant-mesosoma-gene.hpp"
#include "game/ant/genes/ant-nest-site-gene.hpp"
#include "game/ant/genes/ant-ocelli-gene.hpp"
#include "game/ant/genes/ant-pigmentation-gene.hpp"
#include "game/ant/genes/ant-pilosity-gene.hpp"
#include "game/ant/genes/ant-sculpturing-gene.hpp"
#include "game/ant/genes/ant-sting-gene.hpp"
#include "game/ant/genes/ant-waist-gene.hpp"
#include "game/ant/genes/ant-wings-gene.hpp"
#include "game/ant/ant-genome.hpp"
#include "game/ant/ant-caste-type.hpp"

/**
 * Complete set of ant phenes.
 */
struct ant_phenome
{
	/**
	 * Constructs an ant phenome for a given caste.
	 *
	 * @param genome Ant genome.
	 * @param caste Ant caste.
	 */
	ant_phenome(const ant_genome& genome, ant_caste_type caste);
	
	/// Constructs an empty ant phenome.
	ant_phenome() = default;
	
	const ant_antennae_phene* antennae{nullptr};
	const ant_body_size_phene* body_size{nullptr};
	const ant_pupa_phene* pupa{nullptr};
	const ant_diet_phene* diet{nullptr};
	const ant_egg_phene* egg{nullptr};
	const ant_eyes_phene* eyes{nullptr};
	const ant_foraging_time_phene* foraging_time{nullptr};
	const ant_founding_mode_phene* founding_mode{nullptr};
	const ant_gaster_phene* gaster{nullptr};
	const ant_head_phene* head{nullptr};
	const ant_larva_phene* larva{nullptr};
	const ant_legs_phene* legs{nullptr};
	const ant_mandibles_phene* mandibles{nullptr};
	const ant_mesosoma_phene* mesosoma{nullptr};
	const ant_nest_site_phene* nest_site{nullptr};
	const ant_ocelli_phene* ocelli{nullptr};
	const ant_pigmentation_phene* pigmentation{nullptr};
	const ant_pilosity_phene* pilosity{nullptr};
	const ant_sculpturing_phene* sculpturing{nullptr};
	const ant_sting_phene* sting{nullptr};
	const ant_waist_phene* waist{nullptr};
	const ant_wings_phene* wings{nullptr};
};

#endif // ANTKEEPER_GAME_ANT_PHENOME_HPP
