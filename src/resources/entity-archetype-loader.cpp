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
#include "string-table.hpp"
#include "renderer/model.hpp"
#include "game/components/behavior-component.hpp"
#include "game/components/collision-component.hpp"
#include "game/components/terrain-component.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/model-component.hpp"
#include "game/components/nest-component.hpp"
#include "entity/archetype.hpp"
#include "game/behavior/ebt.hpp"
#include <sstream>
#include <stdexcept>

using namespace ecs;

static bool load_behavior_component(archetype& archetype, resource_manager& resource_manager, const std::vector<std::string>& parameters)
{
	if (parameters.size() != 2)
	{
		throw std::runtime_error("load_behavior_component(): Invalid parameter count.");
	}

	std::string filename = parameters[1];
	behavior_component component;
	component.behavior_tree = resource_manager.load<ebt::node>(filename);
	if (!component.behavior_tree)
	{
		std::string message = std::string("load_behavior_component(): Failed to load behavior tree \"") + filename + std::string("\"");
		throw std::runtime_error(message);
	}

	archetype.set<behavior_component>(component);

	return true;
}

static bool load_collision_component(archetype& archetype, resource_manager& resource_manager, const std::vector<std::string>& parameters)
{
	if (parameters.size() != 2)
	{
		throw std::runtime_error("load_collision_component(): Invalid parameter count.");
	}

	std::string filename = parameters[1];
	collision_component component;
	component.mesh = resource_manager.load<mesh>(filename);
	if (!component.mesh)
	{
		std::string message = std::string("load_collision_component(): Failed to load model \"") + filename + std::string("\"");
		throw std::runtime_error(message);
	}

	archetype.set<collision_component>(component);

	return true;
}

static bool load_model_component(archetype& archetype, resource_manager& resource_manager, const std::vector<std::string>& parameters)
{
	if (parameters.size() != 2)
	{
		throw std::runtime_error("load_model_component(): Invalid parameter count.");
	}

	std::string filename = parameters[1];
	model_component component;
	component.model = resource_manager.load<model>(filename);
	component.instance_count = 0;
	component.layers = 1;
	if (!component.model)
	{
		std::string message = std::string("load_model_component(): Failed to load model \"") + filename + std::string("\"");
		throw std::runtime_error(message);
	}

	archetype.set<model_component>(component);

	return true;
}

static bool load_nest_component(archetype& archetype, const std::vector<std::string>& parameters)
{
	nest_component component;
	archetype.set<nest_component>(component);

	return true;
}

static bool load_terrain_component(archetype& archetype, const std::vector<std::string>& parameters)
{
	if (parameters.size() != 4)
	{
		throw std::runtime_error("load_terrain_component(): Invalid parameter count.");
	}

	terrain_component component;
	component.subdivisions = std::stoi(parameters[1]);
	component.x = std::stoi(parameters[2]);
	component.z = std::stoi(parameters[3]);
	archetype.set<terrain_component>(component);

	return true;
}

static bool load_transform_component(archetype& archetype, const std::vector<std::string>& parameters)
{
	if (parameters.size() != 11)
	{
		throw std::runtime_error("load_transform_component(): Invalid parameter count.");
	}

	std::stringstream stream;
	for (std::size_t i = 1; i < parameters.size(); ++i)
	{
		stream << parameters[i];
		if (i < parameters.size() - 1)
			stream << ' ';
	}

	transform_component component;
	stream >> component.transform.translation.x;
	stream >> component.transform.translation.y;
	stream >> component.transform.translation.z;
	stream >> component.transform.rotation.w;
	stream >> component.transform.rotation.x;
	stream >> component.transform.rotation.y;
	stream >> component.transform.rotation.z;
	stream >> component.transform.scale.x;
	stream >> component.transform.scale.y;
	stream >> component.transform.scale.z;
	component.warp = true;

	archetype.set<transform_component>(component);

	return true;
}

static bool load_component(archetype& archetype, resource_manager& resource_manager, const std::vector<std::string>& parameters)
{
	if (parameters[0] == "behavior") return load_behavior_component(archetype, resource_manager, parameters);
	if (parameters[0] == "collision") return load_collision_component(archetype, resource_manager, parameters);
	if (parameters[0] == "model") return load_model_component(archetype, resource_manager, parameters);
	if (parameters[0] == "nest") return load_nest_component(archetype, parameters);
	if (parameters[0] == "terrain") return load_terrain_component(archetype, parameters);
	if (parameters[0] == "transform") return load_transform_component(archetype, parameters);

	std::string message = std::string("load_component(): Unknown component type \"") + parameters[0] + std::string("\"");
	throw std::runtime_error(message);
}

template <>
archetype* resource_loader<archetype>::load(resource_manager* resource_manager, PHYSFS_File* file)
{
	ecs::archetype* archetype = new ecs::archetype(resource_manager->get_archetype_registry());

	// Load string table from input stream
	string_table* table = resource_loader<string_table>::load(resource_manager, file);

	// Ensure table is not empty.
	if (!table || table->empty())
	{
		delete table;
		return nullptr;
	}

	// Load components from table rows
	for (const string_table_row& row: *table)
	{
		// Skip empty rows and comments
		if (row.empty() || row[0].empty() || row[0][0] == '#')
		{
			continue;
		}


		load_component(*archetype, *resource_manager, row);
	}
	
	return archetype;
}

