// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_GENOME_HPP
#define ANTKEEPER_GAME_ANT_GENOME_HPP

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

/**
 * Complete set of ant genes.
 */
struct ant_genome
{
	std::shared_ptr<ant_antennae_gene> antennae;
	std::shared_ptr<ant_body_size_gene> body_size;
	std::shared_ptr<ant_pupa_gene> pupa;
	std::shared_ptr<ant_diet_gene> diet;
	std::shared_ptr<ant_egg_gene> egg;
	std::shared_ptr<ant_eyes_gene> eyes;
	std::shared_ptr<ant_foraging_time_gene> foraging_time;
	std::shared_ptr<ant_founding_mode_gene> founding_mode;
	std::shared_ptr<ant_gaster_gene> gaster;
	std::shared_ptr<ant_head_gene> head;
	std::shared_ptr<ant_larva_gene> larva;
	std::shared_ptr<ant_legs_gene> legs;
	std::shared_ptr<ant_mandibles_gene> mandibles;
	std::shared_ptr<ant_mesosoma_gene> mesosoma;
	std::shared_ptr<ant_nest_site_gene> nest_site;
	std::shared_ptr<ant_ocelli_gene> ocelli;
	std::shared_ptr<ant_pigmentation_gene> pigmentation;
	std::shared_ptr<ant_pilosity_gene> pilosity;
	std::shared_ptr<ant_sculpturing_gene> sculpturing;
	std::shared_ptr<ant_sting_gene> sting;
	std::shared_ptr<ant_waist_gene> waist;
	std::shared_ptr<ant_wings_gene> wings;
};

#endif // ANTKEEPER_GAME_ANT_GENOME_HPP
