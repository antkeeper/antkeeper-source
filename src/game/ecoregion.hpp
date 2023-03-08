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

#ifndef ANTKEEPER_GAME_ECOREGION_HPP
#define ANTKEEPER_GAME_ECOREGION_HPP

#include <engine/utility/fundamental-types.hpp>
#include <engine/render/material.hpp>
#include "game/ant/ant-gene-pool.hpp"
#include <string>
#include <random>
#include <memory>

/**
 * 
 */
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
	float3 terrain_albedo;
	
	/// Horizon material.
	std::shared_ptr<render::material> horizon_material;
	
	/// Array of gene pools.
	std::vector<ant_gene_pool> gene_pools;
	
	/// Discrete probability distribution of gene pools.
	std::discrete_distribution<std::size_t> gene_pool_distribution;
};


#endif // ANTKEEPER_GAME_ECOREGION_HPP
