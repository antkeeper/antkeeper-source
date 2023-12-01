// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/ant/ant-phenome.hpp"

ant_phenome::ant_phenome(const ant_genome& genome, ant_caste_type caste)
{
	if (genome.antennae)
		if (auto i = genome.antennae->phene_map.find(caste); i != genome.antennae->phene_map.end())
			antennae = i->second;
	if (genome.body_size)
		if (auto i = genome.body_size->phene_map.find(caste); i != genome.body_size->phene_map.end())
			body_size = i->second;
	if (genome.pupa)
		if (auto i = genome.pupa->phene_map.find(caste); i != genome.pupa->phene_map.end())
			pupa = i->second;
	if (genome.diet)
		if (auto i = genome.diet->phene_map.find(caste); i != genome.diet->phene_map.end())
			diet = i->second;
	if (genome.egg)
		if (auto i = genome.egg->phene_map.find(caste); i != genome.egg->phene_map.end())
			egg = i->second;
	if (genome.eyes)
		if (auto i = genome.eyes->phene_map.find(caste); i != genome.eyes->phene_map.end())
			eyes = i->second;
	if (genome.foraging_time)
		if (auto i = genome.foraging_time->phene_map.find(caste); i != genome.foraging_time->phene_map.end())
			foraging_time = i->second;
	if (genome.founding_mode)
		if (auto i = genome.founding_mode->phene_map.find(caste); i != genome.founding_mode->phene_map.end())
			founding_mode = i->second;
	if (genome.gaster)
		if (auto i = genome.gaster->phene_map.find(caste); i != genome.gaster->phene_map.end())
			gaster = i->second;
	if (genome.head)
		if (auto i = genome.head->phene_map.find(caste); i != genome.head->phene_map.end())
			head = i->second;
	if (genome.larva)
		if (auto i = genome.larva->phene_map.find(caste); i != genome.larva->phene_map.end())
			larva = i->second;
	if (genome.legs)
		if (auto i = genome.legs->phene_map.find(caste); i != genome.legs->phene_map.end())
			legs = i->second;
	if (genome.mandibles)
		if (auto i = genome.mandibles->phene_map.find(caste); i != genome.mandibles->phene_map.end())
			mandibles = i->second;
	if (genome.mesosoma)
		if (auto i = genome.mesosoma->phene_map.find(caste); i != genome.mesosoma->phene_map.end())
			mesosoma = i->second;
	if (genome.nest_site)
		if (auto i = genome.nest_site->phene_map.find(caste); i != genome.nest_site->phene_map.end())
			nest_site = i->second;
	if (genome.ocelli)
		if (auto i = genome.ocelli->phene_map.find(caste); i != genome.ocelli->phene_map.end())
			ocelli = i->second;
	if (genome.pigmentation)
		if (auto i = genome.pigmentation->phene_map.find(caste); i != genome.pigmentation->phene_map.end())
			pigmentation = i->second;
	if (genome.pilosity)
		if (auto i = genome.pilosity->phene_map.find(caste); i != genome.pilosity->phene_map.end())
			pilosity = i->second;
	if (genome.sculpturing)
		if (auto i = genome.sculpturing->phene_map.find(caste); i != genome.sculpturing->phene_map.end())
			sculpturing = i->second;
	if (genome.sting)
		if (auto i = genome.sting->phene_map.find(caste); i != genome.sting->phene_map.end())
			sting = i->second;
	if (genome.waist)
		if (auto i = genome.waist->phene_map.find(caste); i != genome.waist->phene_map.end())
			waist = i->second;
	if (genome.wings)
		if (auto i = genome.wings->phene_map.find(caste); i != genome.wings->phene_map.end())
			wings = i->second;
}
