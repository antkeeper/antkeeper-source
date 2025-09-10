// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ECOREGION_HPP
#define ANTKEEPER_GAME_ECOREGION_HPP

#include <random>
#include "game/ant/ant-gene-pool.hpp"
#include <engine/math/vector.hpp>
#include <engine/render/material.hpp>
#include <engine/utility/sized-types.hpp>
#include <memory>
#include <string>

using namespace engine;

struct ecoregion
{
	/// Ecoregion name.
	std::string name;
	
	/// Elevation, in meters.
	float elevation;
	
	/// Latitude, in radians.
	float latitude;
	
	/// Longitude, in radians.
	float longitude;
	
	/// Terrain material.
	std::shared_ptr<render::material> terrain_material;
	
	/// Terrain albedo.
	math::fvec3 terrain_albedo;
	
	/// Array of gene pools.
	std::vector<ant_gene_pool> gene_pools;
	
	/// Discrete probability distribution of gene pools.
	std::discrete_distribution<usize> gene_pool_distribution;
};

#endif // ANTKEEPER_GAME_ECOREGION_HPP
