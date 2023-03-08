/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANTKEEPER_GAME_ANT_GENOME_HPP
#define ANTKEEPER_GAME_ANT_GENOME_HPP

#include "game/ant/genes/ant-antennae-gene.hpp"
#include "game/ant/genes/ant-body-size-gene.hpp"
#include "game/ant/genes/ant-cocoon-gene.hpp"
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
	std::shared_ptr<ant_cocoon_gene> cocoon;
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
