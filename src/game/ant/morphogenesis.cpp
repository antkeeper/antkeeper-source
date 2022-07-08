/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "game/ant/morphogenesis.hpp"
#include "render/material.hpp"

namespace game {
namespace ant {

static render::model* generate_queen(const ant::breed& breed);
static render::model* generate_worker(const ant::breed& breed);
static render::model* generate_soldier(const ant::breed& breed);
static render::model* generate_male(const ant::breed& breed);
static render::material* build_material(const ant::breed& breed);
static render::model* build_model
(
	render::material* material,
	render::model* antennae,
	render::model* eyes,
	render::model* forewings,
	render::model* gaster,
	render::model* head,
	render::model* hindwings,
	render::model* legs,
	render::model* mandibles,
	render::model* mesosoma,
	render::model* ocelli,
	render::model* sting,
	render::model* waist
);

render::model* morphogenesis(const ant::breed& breed, ant::caste caste)
{
	switch (caste)
	{
		case ant::caste::queen:
			return generate_queen(breed);
		case ant::caste::worker:
			return generate_worker(breed);
		case ant::caste::soldier:
			return generate_soldier(breed);
		case ant::caste::male:
			return generate_male(breed);
	}
	
	return nullptr;
}

render::model* generate_queen(const ant::breed& breed)
{
	return nullptr;
}

render::model* generate_worker(const ant::breed& breed)
{
	// Get material parameters
	
	
	// Build material
	render::material* material = build_material(breed);
	
	// Get worker body part models
	render::model* antennae_model = breed.antennae->model;
	render::model* eyes_model = breed.eyes->model;
	render::model* gaster_model = breed.gaster->model;
	render::model* head_model = breed.head->model;
	render::model* legs_model = breed.legs->model;
	render::model* mandibles_model = breed.mandibles->model;
	render::model* mesosoma_model = breed.mesosoma->model;
	render::model* sting_model = breed.sting->model;
	render::model* waist_model = breed.waist->model;
	
	// Build worker model
	render::model* model = build_model
	(
		material,
		antennae_model,
		eyes_model,
		nullptr,
		gaster_model,
		head_model,
		nullptr,
		legs_model,
		mandibles_model,
		mesosoma_model,
		nullptr,
		sting_model,
		waist_model
	);
	
	return model;
}


render::model* generate_soldier(const ant::breed& breed)
{
	return nullptr;
}

render::model* generate_male(const ant::breed& breed)
{
	return nullptr;
}

render::material* build_material(const ant::breed& breed)
{
	return nullptr;
}

render::model* build_model
(
	render::material* material,
	render::model* antennae,
	render::model* eyes,
	render::model* forewings,
	render::model* gaster,
	render::model* head,
	render::model* hindwings,
	render::model* legs,
	render::model* mandibles,
	render::model* mesosoma,
	render::model* ocelli,
	render::model* sting,
	render::model* waist
)
{
	return nullptr;
}


} // namespace ant
} // namespace game
