/*
 * Copyright (C) 2017-2019  Christopher J. Howard
 *
 * This file is part of Antkeeper Source Code.
 *
 * Antkeeper Source Code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper Source Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper Source Code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "resource-loader.hpp"
#include "resource-manager.hpp"
#include "csv-table.hpp"
#include "../entity/components/ant-hill-component.hpp"
#include "../entity/components/collision-component.hpp"
#include "../entity/components/model-component.hpp"
#include "../entity/components/terrain-patch-component.hpp"
#include "../entity/components/tool-component.hpp"
#include "../entity/components/transform-component.hpp"
#include "../entity/entity-template.hpp"

#include <sstream>
#include <stdexcept>

static ComponentBase* loadAntHillComponent(const std::vector<std::string>& parameters)
{
	if (parameters.size() != 1)
	{
		throw std::runtime_error("loadAntHillComponent(): Invalid parameter count.");
	}

	AntHillComponent* component = new AntHillComponent();
	
	return component;
}

static ComponentBase* loadCollisionComponent(ResourceManager* resourceManager, const std::vector<std::string>& parameters)
{
	if (parameters.size() != 2)
	{
		throw std::runtime_error("loadCollisionComponent(): Invalid parameter count.");
	}

	CollisionComponent* component = new CollisionComponent();
	component->mesh = resourceManager->load<TriangleMesh>(parameters[1]);
	
	return component;
}

static ComponentBase* loadModelComponent(ResourceManager* resourceManager, const std::vector<std::string>& parameters)
{
	if (parameters.size() != 2)
	{
		throw std::runtime_error("loadModelComponent(): Invalid parameter count.");
	}

	std::string filename = parameters[1];
	Model* model = resourceManager->load<Model>(filename);
	if (!model)
	{
		std::string message = std::string("loadModelComponent(): Failed to load model \"") + filename + std::string("\"");
		throw std::runtime_error(message);
	}

	ModelComponent* component = new ModelComponent();
	component->model.setModel(model);
	component->model.setPose(nullptr);

	return component;
}

static ComponentBase* loadTerrainPatchComponent(const std::vector<std::string>& parameters)
{
	if (parameters.size() != 4)
	{
		throw std::runtime_error("loadTerrainPatchComponent(): Invalid parameter count.");
	}

	int subdivisions;
	std::tuple<int, int> position;
	float size;
	std::stringstream stream;
	for (std::size_t i = 1; i < parameters.size(); ++i)
	{
		stream << parameters[i];
		if (i < parameters.size() - 1)
			stream << ' ';
	}
	stream >> subdivisions;
	stream >> std::get<0>(position);
	stream >> std::get<1>(position);

	TerrainPatchComponent* component = new TerrainPatchComponent();
	component->subdivisions = subdivisions;
	component->position = position;

	return component;
}

static ComponentBase* loadToolComponent(const std::vector<std::string>& parameters)
{
	if (parameters.size() != 1)
	{
		throw std::runtime_error("loadToolComponent(): Invalid parameter count.");
	}

	ToolComponent* component = new ToolComponent();
	component->active = true;
	
	return component;
}

static ComponentBase* loadTransformComponent(const std::vector<std::string>& parameters)
{
	if (parameters.size() != 11)
	{
		throw std::runtime_error("loadTransformComponent(): Invalid parameter count.");
	}

	Vector3 translation;
	Quaternion rotation;
	Vector3 scale;

	std::stringstream stream;
	for (std::size_t i = 1; i < parameters.size(); ++i)
	{
		stream << parameters[i];
		if (i < parameters.size() - 1)
			stream << ' ';
	}

	stream >> translation.x;
	stream >> translation.y;
	stream >> translation.z;
	stream >> rotation.w;
	stream >> rotation.x;
	stream >> rotation.y;
	stream >> rotation.z;
	stream >> scale.x;
	stream >> scale.y;
	stream >> scale.z;

	TransformComponent* component = new TransformComponent();
	component->transform.translation = translation;
	component->transform.rotation = rotation;
	component->transform.scale = scale;

	return component;
}

static ComponentBase* loadComponent(ResourceManager* resourceManager, const std::vector<std::string>& parameters)
{
	if (parameters[0] == "ant-hill") return loadAntHillComponent(parameters);
	if (parameters[0] == "collision") return loadCollisionComponent(resourceManager, parameters);
	if (parameters[0] == "model") return loadModelComponent(resourceManager, parameters);
	if (parameters[0] == "terrain-patch") return loadTerrainPatchComponent(parameters);
	if (parameters[0] == "tool") return loadToolComponent(parameters);
	if (parameters[0] == "transform") return loadTransformComponent(parameters);

	std::string message = std::string("loadComponent(): Unknown component type \"") + parameters[0] + std::string("\"");
	throw std::runtime_error(message);
}

template <>
EntityTemplate* ResourceLoader<EntityTemplate>::load(ResourceManager* resourceManager, std::istream* is)
{
	std::list<ComponentBase*> components;
	EntityTemplate* entityTemplate = nullptr;

	// Load CSV table from input stream
	CSVTable* table = ResourceLoader<CSVTable>::load(resourceManager, is);

	// Ensure table is not empty.
	if (!table || table->empty())
	{
		delete table;
		return nullptr;
	}

	// Load components from table rows
	for (const CSVRow& row: *table)
	{
		// Skip empty rows and comments
		if (row.empty() || row[0].empty() || row[0][0] == '#')
		{
			continue;
		}

		ComponentBase* component = loadComponent(resourceManager, row);
		if (component != nullptr)
		{
			components.push_back(component);
		}
	}
	
	// Create entity template and free loaded components
	entityTemplate = new EntityTemplate(components);
	for (ComponentBase* component: components)
	{
		delete component;
	}

	return entityTemplate;
}

