/*
 * Copyright (C) 2020  Christopher J. Howard
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

#include "resource-loader.hpp"
#include "resource-manager.hpp"
#include "game/biome.hpp"
#include "math/angles.hpp"
#include <nlohmann/json.hpp>
#include <physfs.h>

template <typename T>
static bool load_value(T* value, const nlohmann::json& json, const std::string& name)
{
	if (auto element = json.find(name); element != json.end())
	{
		*value = element.value().get<T>();
		return true;
	}
	
	return false;
}

template <typename T>
static bool load_array(T* value, std::size_t size, const nlohmann::json& json, const std::string& name)
{
	if (auto element = json.find(name); element != json.end())
	{
		std::size_t i = 0;
		for (auto it = element.value().cbegin(); (it != element.value().cend()) && (i < size); ++it)
		{
			*(value++) = it.value().get<float>();
			++i;
		}
		
		return true;
	}
	
	return false;
}

template <>
biome* resource_loader<biome>::load(resource_manager* resource_manager, PHYSFS_File* file)
{
	// Read file into buffer
	std::size_t size = static_cast<int>(PHYSFS_fileLength(file));
	std::string buffer;
	buffer.resize(size);
	PHYSFS_readBytes(file, &buffer[0], size);
	
	// Parse json from file buffer
	nlohmann::json json = nlohmann::json::parse(buffer);
	
	biome* biome = new ::biome();
	
	load_value(&biome->name, json, "name");
	
	if (auto terrain = json.find("terrain"); terrain != json.end())
	{
		std::string material_filename;
		if (load_value(&material_filename, terrain.value(), "material"))
		{
			biome->terrain_material = resource_manager->load<::material>(material_filename);
		}	
	}
	
	if (auto weather = json.find("weather"); weather != json.end())
	{
		load_array(&biome->ambient_color.x, 3, weather.value(), "ambient_color");
		load_value(&biome->ambient_intensity, weather.value(), "ambient_intensity");
		
		
		load_value(&biome->sun_azimuth, weather.value(), "sun_azimuth");
		biome->sun_azimuth = math::radians(biome->sun_azimuth);
		load_value(&biome->sun_elevation, weather.value(), "sun_elevation");
		biome->sun_elevation = math::radians(biome->sun_elevation);
		load_array(&biome->sun_color.x, 3, weather.value(), "sun_color");
		load_value(&biome->sun_intensity, weather.value(), "sun_intensity");
		load_value(&biome->sun_angular_radius, weather.value(), "sun_angular_radius");
		biome->sun_angular_radius = math::radians(biome->sun_angular_radius);
		
		load_array(&biome->horizon_color.x, 3, weather.value(), "horizon_color");
		load_array(&biome->zenith_color.x, 3, weather.value(), "zenith_color");
		
		load_value(&biome->wind_speed, weather.value(), "wind_speed");
		load_value(&biome->wind_direction, weather.value(), "wind_direction");
		biome->wind_direction = math::radians(biome->wind_direction);
		
		std::string sky_palette_filename;
		if (load_value(&sky_palette_filename, weather.value(), "sky_palette"))
		{
			biome->sky_palette = resource_manager->load<image>(sky_palette_filename);
		}
		
		std::string shadow_palette_filename;
		if (load_value(&shadow_palette_filename, weather.value(), "shadow_palette"))
		{
			biome->shadow_palette = resource_manager->load<image>(shadow_palette_filename);
		}
	}

	return biome;
}

