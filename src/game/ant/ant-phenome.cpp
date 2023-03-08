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

#include "game/ant/ant-phenome.hpp"

ant_phenome::ant_phenome(const ant_genome& genome, ant_caste caste)
{
	if (genome.antennae)
		if (auto it = genome.antennae->phenes.find(caste); it != genome.antennae->phenes.end())
			antennae = &it->second;
	if (genome.body_size)
		if (auto it = genome.body_size->phenes.find(caste); it != genome.body_size->phenes.end())
			body_size = &it->second;
	if (genome.cocoon)
		cocoon = &genome.cocoon->phene;
	if (genome.diet)
		diet = &genome.diet->phene;
	if (genome.egg)
		egg = &genome.egg->phene;
	if (genome.eyes)
		if (auto it = genome.eyes->phenes.find(caste); it != genome.eyes->phenes.end())
			eyes = &it->second;
	if (genome.foraging_time)
		foraging_time = &genome.foraging_time->phene;
	if (genome.founding_mode)
		founding_mode = &genome.founding_mode->phene;
	if (genome.gaster)
		if (auto it = genome.gaster->phenes.find(caste); it != genome.gaster->phenes.end())
			gaster = &it->second;
	if (genome.head)
		if (auto it = genome.head->phenes.find(caste); it != genome.head->phenes.end())
			head = &it->second;
	if (genome.larva)
		larva = &genome.larva->phene;
	if (genome.legs)
		if (auto it = genome.legs->phenes.find(caste); it != genome.legs->phenes.end())
			legs = &it->second;
	if (genome.mandibles)
		if (auto it = genome.mandibles->phenes.find(caste); it != genome.mandibles->phenes.end())
			mandibles = &it->second;
	if (genome.mesosoma)
		if (auto it = genome.mesosoma->phenes.find(caste); it != genome.mesosoma->phenes.end())
			mesosoma = &it->second;
	if (genome.nest_site)
		nest_site = &genome.nest_site->phene;
	if (genome.ocelli)
		if (auto it = genome.ocelli->phenes.find(caste); it != genome.ocelli->phenes.end())
			ocelli = &it->second;
	if (genome.pigmentation)
		if (auto it = genome.pigmentation->phenes.find(caste); it != genome.pigmentation->phenes.end())
			pigmentation = &it->second;
	if (genome.pilosity)
		if (auto it = genome.pilosity->phenes.find(caste); it != genome.pilosity->phenes.end())
			pilosity = &it->second;
	if (genome.sculpturing)
		if (auto it = genome.sculpturing->phenes.find(caste); it != genome.sculpturing->phenes.end())
			sculpturing = &it->second;
	if (genome.sting)
		if (auto it = genome.sting->phenes.find(caste); it != genome.sting->phenes.end())
			sting = &it->second;
	if (genome.waist)
		if (auto it = genome.waist->phenes.find(caste); it != genome.waist->phenes.end())
			waist = &it->second;
	if (genome.wings)
		if (auto it = genome.wings->phenes.find(caste); it != genome.wings->phenes.end())
			wings = &it->second;
}
