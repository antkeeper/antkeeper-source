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

#include "resource-loader.hpp"
#include "resource-manager.hpp"
#include "string-table.hpp"
#include "renderer/model.hpp"
#include "entity/components/behavior.hpp"
#include "entity/components/collision.hpp"
#include "entity/components/terrain.hpp"
#include "entity/components/transform.hpp"
#include "entity/components/model.hpp"
#include "entity/components/nest.hpp"
#include "entity/components/marker.hpp"
#include "entity/components/brush.hpp"
#include "entity/archetype.hpp"
#include "entity/ebt.hpp"
#include <sstream>
#include <stdexcept>

static bool load_component_behavior(entity::archetype& archetype, resource_manager& resource_manager, const std::vector<std::string>& parameters)
{
	if (parameters.size() != 2)
	{
		throw std::runtime_error("load_component_behavior(): Invalid parameter count.");
	}

	std::string filename = parameters[1];
	entity::component::behavior component;
	component.behavior_tree = resource_manager.load<entity::ebt::node>(filename);
	if (!component.behavior_tree)
	{
		std::string message = std::string("load_component_behavior(): Failed to load behavior tree \"") + filename + std::string("\"");
		throw std::runtime_error(message);
	}

	archetype.set<entity::component::behavior>(component);

	return true;
}

static bool load_component_collision(entity::archetype& archetype, resource_manager& resource_manager, const std::vector<std::string>& parameters)
{
	if (parameters.size() != 2)
	{
		throw std::runtime_error("load_component_collision(): Invalid parameter count.");
	}

	std::string filename = parameters[1];
	entity::component::collision component;
	component.mesh = resource_manager.load<geom::mesh>(filename);
	if (!component.mesh)
	{
		std::string message = std::string("load_component_collision(): Failed to load model \"") + filename + std::string("\"");
		throw std::runtime_error(message);
	}

	archetype.set<entity::component::collision>(component);

	return true;
}

static bool load_component_model(entity::archetype& archetype, resource_manager& resource_manager, const std::vector<std::string>& parameters)
{
	if (parameters.size() != 2)
	{
		throw std::runtime_error("load_component_model(): Invalid parameter count.");
	}

	std::string filename = parameters[1];
	entity::component::model component;
	component.render_model = resource_manager.load<model>(filename);
	component.instance_count = 0;
	component.layers = 1;
	if (!component.render_model)
	{
		std::string message = std::string("load_component_model(): Failed to load model \"") + filename + std::string("\"");
		throw std::runtime_error(message);
	}

	archetype.set<entity::component::model>(component);

	return true;
}

static bool load_component_nest(entity::archetype& archetype, const std::vector<std::string>& parameters)
{
	entity::component::nest component;
	archetype.set<entity::component::nest>(component);

	return true;
}

static bool load_component_marker(entity::archetype& archetype, const std::vector<std::string>& parameters)
{
	if (parameters.size() != 2)
	{
		throw std::runtime_error("load_component_marker(): Invalid parameter count.");
	}
	
	entity::component::marker component;
	component.color = std::stoi(parameters[1]);
	archetype.set<entity::component::marker>(component);
	
	return true;
}

static bool load_component_brush(entity::archetype& archetype, const std::vector<std::string>& parameters)
{
	if (parameters.size() != 2)
	{
		throw std::runtime_error("load_component_brush(): Invalid parameter count.");
	}
	
	entity::component::brush component;
	component.radius = std::stof(parameters[1]);
	archetype.set<entity::component::brush>(component);
	
	return true;
}

static bool load_component_transform(entity::archetype& archetype, const std::vector<std::string>& parameters)
{
	if (parameters.size() != 11)
	{
		throw std::runtime_error("load_component_transform(): Invalid parameter count.");
	}

	std::stringstream stream;
	for (std::size_t i = 1; i < parameters.size(); ++i)
	{
		stream << parameters[i];
		if (i < parameters.size() - 1)
			stream << ' ';
	}

	entity::component::transform component;
	stream >> component.local.translation.x;
	stream >> component.local.translation.y;
	stream >> component.local.translation.z;
	stream >> component.local.rotation.w;
	stream >> component.local.rotation.x;
	stream >> component.local.rotation.y;
	stream >> component.local.rotation.z;
	stream >> component.local.scale.x;
	stream >> component.local.scale.y;
	stream >> component.local.scale.z;
	component.warp = true;

	archetype.set<entity::component::transform>(component);

	return true;
}

static bool load_component(entity::archetype& archetype, resource_manager& resource_manager, const std::vector<std::string>& parameters)
{
	if (parameters[0] == "behavior") return load_component_behavior(archetype, resource_manager, parameters);
	if (parameters[0] == "collision") return load_component_collision(archetype, resource_manager, parameters);
	if (parameters[0] == "model") return load_component_model(archetype, resource_manager, parameters);
	if (parameters[0] == "nest") return load_component_nest(archetype, parameters);
	if (parameters[0] == "transform") return load_component_transform(archetype, parameters);
	if (parameters[0] == "marker") return load_component_marker(archetype, parameters);
	if (parameters[0] == "brush") return load_component_brush(archetype, parameters);

	std::string message = std::string("load_component(): Unknown component type \"") + parameters[0] + std::string("\"");
	throw std::runtime_error(message);
}

template <>
entity::archetype* resource_loader<entity::archetype>::load(resource_manager* resource_manager, PHYSFS_File* file)
{
	entity::archetype* archetype = new entity::archetype(resource_manager->get_archetype_registry());

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

