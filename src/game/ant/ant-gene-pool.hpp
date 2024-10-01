// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_GENE_POOL_HPP
#define ANTKEEPER_GAME_ANT_GENE_POOL_HPP

#include "game/ant/ant-gene-frequency-table.hpp"
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

/// Pool of ant genes from which ant genomes can be generated.
struct ant_gene_pool
{
	/// Gene pool name.
	std::string name;
	
	ant_gene_frequency_table<ant_antennae_gene> antennae;
	ant_gene_frequency_table<ant_body_size_gene> body_size;
	ant_gene_frequency_table<ant_pupa_gene> pupa;
	ant_gene_frequency_table<ant_diet_gene> diet;
	ant_gene_frequency_table<ant_egg_gene> egg;
	ant_gene_frequency_table<ant_eyes_gene> eyes;
	ant_gene_frequency_table<ant_foraging_time_gene> foraging_time;
	ant_gene_frequency_table<ant_founding_mode_gene> founding_mode;
	ant_gene_frequency_table<ant_gaster_gene> gaster;
	ant_gene_frequency_table<ant_head_gene> head;
	ant_gene_frequency_table<ant_larva_gene> larva;
	ant_gene_frequency_table<ant_legs_gene> legs;
	ant_gene_frequency_table<ant_mandibles_gene> mandibles;
	ant_gene_frequency_table<ant_mesosoma_gene> mesosoma;
	ant_gene_frequency_table<ant_nest_site_gene> nest_site;
	ant_gene_frequency_table<ant_ocelli_gene> ocelli;
	ant_gene_frequency_table<ant_pigmentation_gene> pigmentation;
	ant_gene_frequency_table<ant_pilosity_gene> pilosity;
	ant_gene_frequency_table<ant_sculpturing_gene> sculpturing;
	ant_gene_frequency_table<ant_sting_gene> sting;
	ant_gene_frequency_table<ant_waist_gene> waist;
	ant_gene_frequency_table<ant_wings_gene> wings;
};

#endif // ANTKEEPER_GAME_ANT_GENE_POOL_HPP
