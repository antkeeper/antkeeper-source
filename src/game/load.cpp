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

#include "game/load.hpp"
#include "game/world.hpp"
#include "application.hpp"
#include "debug/logger.hpp"
#include "resources/json.hpp"
#include "resources/resource-manager.hpp"
#include "render/model.hpp"
#include "render/material.hpp"
#include "render/passes/sky-pass.hpp"
#include "render/passes/ground-pass.hpp"
#include "game/system/astronomy.hpp"
#include "game/system/terrain.hpp"
#include "math/random.hpp"
#include "math/noise/noise.hpp"
#include <fstream>
#include <iostream>
#include <stb/stb_image_write.h>
#include "resources/image.hpp"

#include <algorithm>
#include <execution>


namespace game {
namespace load {

void biome(game::context& ctx, const std::filesystem::path& path)
{
	ctx.logger->push_task("Loading biome from \"" + path.string() + "\"");
	
	image img;
	img.format(1, 1);
	img.resize(2048, 2048);
	
	float frequency = 10.0f;
	std::size_t octaves = 4;
	float lacunarity = 2.0f;
	float gain = 0.5f;	
	auto hash = static_cast<std::uint32_t(*)(const math::vector<float, 2>&)>(math::noise::hash::pcg3d_1);
	auto noise = static_cast<float(*)(const math::vector<float, 2>&, decltype(hash))>(math::noise::simplex);
	
	auto fbm = [&](const float2& x)
	{
		return math::noise::fbm
		(
			x,
			octaves,
			lacunarity,
			gain,
			noise,
			hash
		);
	};
	
	auto width = img.get_width();
	auto height = img.get_height();
	unsigned char* pixels = (unsigned char*)img.data();
	
	float scale_x = 1.0f / static_cast<float>(width - 1) * frequency;
	float scale_y = 1.0f / static_cast<float>(height - 1) * frequency;
	

	
	std::for_each
	(
		std::execution::par_unseq,
		img.begin<unsigned char>(),
		img.end<unsigned char>(),
		[pixels, width, height, scale_x, scale_y, &fbm](auto& pixel)
		{
			std::size_t i = &pixel - pixels;
			std::size_t y = i / width;
			std::size_t x = i % width;
			
			float2 position =
			{
				static_cast<float>(x) * scale_x,
				static_cast<float>(y) * scale_y
			};
			
			//float n = math::noise::simplex<float, 2>(position, &math::noise::hash::pcg3d_1);
			//float n = fbm(position);
			// auto [sqr_center_distance, displacement, id, sqr_edge_distance] = math::noise::voronoi::f1_edge<float, std::uint32_t>(position, 1.0f, &math::noise::hash::pcg3d_3);
			// float center_distance = std::sqrt(sqr_center_distance);
			// float edge_distance = std::sqrt(sqr_edge_distance);
			
			auto
			[
				f1_sqr_distance,
				f1_displacement,
				f1_id,
				f2_sqr_distance,
				f2_displacement,
				f2_id
			] = math::noise::voronoi::f1_f2<float, std::uint32_t>(position, 1.0f, {0.0f, 0.0f}, &math::noise::hash::pcg3d_3);
			
			float f1_distance = std::sqrt(f1_sqr_distance);
			float f2_distance = std::sqrt(f2_sqr_distance);
			
			pixel = static_cast<unsigned char>(std::min(255.0f, f1_distance * 255.0f));
			//pixel = static_cast<unsigned char>(id % 255);
		}
	);
	
	stbi_flip_vertically_on_write(1);
	stbi_write_png((ctx.config_path / "gallery" / "noise.png").string().c_str(), img.get_width(), img.get_height(), img.get_channel_count(), img.data(), img.get_width() * img.get_channel_count());
	
	
	try
	{
		json* data = ctx.resource_manager->load<json>(path);
		
		// Load location
		if (auto location = data->find("location"); location != data->end())
		{
			double elevation = 0.0;
			double latitude = 0.0;
			double longitude = 0.0;
			
			if (auto location_ele = location->find("elevation"); location_ele != location->end())
				elevation = location_ele->get<double>();
			else
				ctx.logger->warning("Biome elevation undefined");
			
			if (auto location_lat = location->find("latitude"); location_lat != location->end())
				latitude = math::radians<double>(location_lat->get<double>());
			else
				ctx.logger->warning("Biome latitude undefined");
			
			if (auto location_lon = location->find("longitude"); location_lon != location->end())
				longitude = math::radians<double>(location_lon->get<double>());
			else
				ctx.logger->warning("Biome longitude undefined");
			
			// Set location
			game::world::set_location(ctx, elevation, latitude, longitude);
		}
		else
		{
			ctx.logger->warning("Biome location undefined");
		}
		
		// Setup sky
		ctx.sky_pass->set_sky_model(ctx.resource_manager->load<render::model>("celestial-hemisphere.mdl"));
		
		// Load terrain
		if (auto terrain = data->find("terrain"); terrain != data->end())
		{
			if (auto material = terrain->find("material"); material != terrain->end())
			{
				render::material* terrain_material = ctx.resource_manager->load<render::material>(material->get<std::string>());
				ctx.terrain_system->set_patch_material(terrain_material);
			}
			else
			{
				ctx.logger->warning("Biome terrain material undefined");
			}
			
			if (auto material = terrain->find("horizon_material"); material != terrain->end())
			{
				render::model* terrestrial_hemisphere_model = ctx.resource_manager->load<render::model>("terrestrial-hemisphere.mdl");
				(*terrestrial_hemisphere_model->get_groups())[0]->set_material(ctx.resource_manager->load<render::material>(material->get<std::string>()));
				ctx.ground_pass->set_ground_model(terrestrial_hemisphere_model);
			}
			else
			{
				ctx.logger->warning("Biome terrain horizon material undefined");
			}
			
			// Terrain elevation function
			ctx.terrain_system->set_elevation_function
			(
				[](float x, float z) -> float
				{
					float angle = math::radians(30.0f);
					float c = std::cos(angle);
					float s = std::sin(angle);
					
					x = x * c - z * s;
					z = x * s + z * c;
					
					
					
					float frequency = 0.01f;
					std::size_t octaves = 4;
					float lacunarity = 3.0f;
					float gain = 0.5f;
					auto noise = static_cast<float(*)(const math::vector<float, 2>&, decltype(hash))>(math::noise::simplex);
					auto hash = static_cast<std::uint32_t(*)(const math::vector<float, 2>&)>(math::noise::hash::pcg3d_1);
					
					float2 position = float2{x, z} * frequency;
					
					/*
					float n = math::noise::fbm
					(
						position,
						octaves,
						lacunarity,
						gain,
						noise,
						hash
					);
					*/
					//float n = math::noise::voronoi::f1<float, std::uint32_t>(position, 1.0f, &math::noise::hash::pcg3d_3)[0];
					float n = 0.0f;
					return 10.0f * n;
				}
			);
			
			// Setup lighting
			double3 terrain_albedo = {0, 0, 0};
			if (terrain->contains("albedo"))
			{
				const auto& albedo_element = (*terrain)["albedo"];
				
				terrain_albedo[0] = albedo_element[0].get<double>();
				terrain_albedo[1]= albedo_element[1].get<double>();
				terrain_albedo[2] = albedo_element[2].get<double>();
				
			}
			ctx.astronomy_system->set_bounce_albedo(terrain_albedo);
		}
		else
		{
			ctx.logger->warning("Biome terrain undefined");
		}
	}
	catch (...)
	{
		ctx.logger->pop_task(EXIT_FAILURE);
	}
	ctx.logger->pop_task(EXIT_SUCCESS);
}

void colony(game::context& ctx, const std::filesystem::path& path)
{
	ctx.logger->push_task("Loading colony from \"" + path.string() + "\"");
	try
	{
		json* data = ctx.resource_manager->load<json>(path);
		
	}
	catch (...)
	{
		ctx.logger->pop_task(EXIT_FAILURE);
	}
	ctx.logger->pop_task(EXIT_SUCCESS);
}

} // namespace load
} // namespace game
