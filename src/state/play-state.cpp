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

#include "application-states.hpp"
#include "configuration.hpp"
#include "application.hpp"
#include "scene/model-instance.hpp"
#include "resources/resource-manager.hpp"
#include "renderer/model.hpp"
#include "renderer/material.hpp"
#include "systems/control-system.hpp"
#include "entity/components/model-component.hpp"
#include "entity/components/transform-component.hpp"
#include "entity/components/terrain-component.hpp"
#include "entity/components/samara-component.hpp"
#include "entity/components/cavity-component.hpp"
#include "entity/components/tool-component.hpp"
#include "entity/components/placement-component.hpp"
#include "entity/archetype.hpp"
#include "nest.hpp"
#include "math.hpp"
#include "geometry/mesh-accelerator.hpp"
#include "behavior/ebt.hpp"
#include <iostream>

using namespace vmq::operators;

void enter_play_state(application* app)
{
	logger* logger = app->get_logger();
	int enter_state_task = logger->open_task("Entering play state");


	resource_manager* resource_manager = app->get_resource_manager();
	entt::registry& ecs_registry = app->get_ecs_registry();

	// Load entity archetypes
	ecs::archetype* ant_hill_archetype = resource_manager->load<ecs::archetype>("ant-hill.ent");
	ecs::archetype* maple_tree_archetype = resource_manager->load<ecs::archetype>("maple-tree.ent");
	ecs::archetype* darkness_volume_archetype = resource_manager->load<ecs::archetype>("darkness-volume.ent");
	ecs::archetype* nest_archetype = resource_manager->load<ecs::archetype>("harvester-nest.ent");
	ecs::archetype* samara_archetype = resource_manager->load<ecs::archetype>("samara.ent");
	ecs::archetype* forceps_archetype = resource_manager->load<ecs::archetype>("forceps.ent");
	ecs::archetype* larva_archetype = resource_manager->load<ecs::archetype>("larva.ent");
	ecs::archetype* pebble_archetype = resource_manager->load<ecs::archetype>("pebble.ent");



	ecs::placement_component placement;

	auto ant_hill_entity = ant_hill_archetype->create(ecs_registry);
	placement.ray.origin = {0, 10000, 0};
	placement.ray.direction = {0, -1, 0};
	ecs_registry.assign<ecs::placement_component>(ant_hill_entity, placement);
	

	
	float pebble_radius = 300.0f;
	int pebble_count = 100;
	
	for (int i = 0; i < pebble_count; ++i)
	{
		float x = frand(-pebble_radius, pebble_radius);
		float z = frand(-pebble_radius, pebble_radius);
		
		auto pebble_entity = pebble_archetype->create(ecs_registry);
		
		auto& transform = ecs_registry.get<ecs::transform_component>(pebble_entity);
		transform.transform = vmq::identity_transform<float>;
		transform.transform.rotation = vmq::angle_axis(frand(0.0f, vmq::two_pi<float>), {0, 1, 0});
		transform.transform.scale = float3{1, 1, 1} * frand(0.75f, 1.25f);
		
		placement.ray.origin = {x, 10000, z};
		ecs_registry.assign<ecs::placement_component>(pebble_entity, placement);
	}

	auto maple_tree_entity = maple_tree_archetype->create(ecs_registry);
	placement.ray.origin = {300, 10000, 200};
	placement.ray.direction = {0, -1, 0};
	ecs_registry.assign<ecs::placement_component>(maple_tree_entity, placement);

	//auto darkness_volume_entity = darkness_volume_archetype->create(ecs_registry);
	auto nest_entity = nest_archetype->create(ecs_registry);

	int terrain_radius = 2;
	for (int x = -terrain_radius; x <= terrain_radius; ++x)
	{
		for (int z = -terrain_radius; z <= terrain_radius; ++z)
		{
			ecs::terrain_component terrain_component;
			terrain_component.subdivisions = TERRAIN_PATCH_RESOLUTION;
			terrain_component.x = x;
			terrain_component.z = z;
			auto terrain_entity = ecs_registry.create();
			ecs_registry.assign<ecs::terrain_component>(terrain_entity, terrain_component);
		}
	}

	for (int i = 0; i < 15; ++i)
	{
		auto samara_entity = samara_archetype->create(ecs_registry);

		auto& transform = ecs_registry.get<ecs::transform_component>(samara_entity);
		float zone = 200.0f;
		transform.transform = vmq::identity_transform<float>;
		transform.transform.translation.x = frand(-zone, zone);
		transform.transform.translation.y = frand(50.0f, 150.0f);
		transform.transform.translation.z = frand(-zone, zone);

		ecs::samara_component samara_component;
		samara_component.angle = frand(0.0f, vmq::radians(360.0f));
		samara_component.direction = vmq::normalize(float3{frand(-1, 1), frand(-1, -5), frand(-1, 1)});
		samara_component.chirality = (frand(0, 1) < 0.5f) ? -1.0f : 1.0f;

		ecs_registry.assign_or_replace<ecs::samara_component>(samara_entity, samara_component);
	}

	/*
	ecs::archetype* grass_archetype = resource_manager->load<ecs::archetype>("grassland-grass.ent");
	auto grass_entity_1 = grass_archetype->create(ecs_registry);
	auto grass_entity_2 = grass_archetype->create(ecs_registry);
	ecs_registry.get<ecs::transform_component>(grass_entity_2).transform.rotation = vmq::angle_axis(vmq::radians(120.0f), float3{0, 1, 0});
	*/

	// Setup overworld camera
	camera* camera = app->get_overworld_camera();
	orbit_cam* orbit_cam = app->get_orbit_cam();
	orbit_cam->attach(camera);
	orbit_cam->set_target_focal_point({0, 0, 0});
	orbit_cam->set_target_focal_distance(15.0f);
	orbit_cam->set_target_elevation(vmq::radians(25.0f));
	orbit_cam->set_target_azimuth(0.0f);
	orbit_cam->set_focal_point(orbit_cam->get_target_focal_point());
	orbit_cam->set_focal_distance(orbit_cam->get_target_focal_distance());
	orbit_cam->set_elevation(orbit_cam->get_target_elevation());
	orbit_cam->set_azimuth(orbit_cam->get_target_azimuth());




	// Create forceps tool
	auto forceps_entity = forceps_archetype->create(ecs_registry);
	ecs::tool_component forceps_tool_component;
	forceps_tool_component.active = true;
	ecs_registry.assign<ecs::tool_component>(forceps_entity, forceps_tool_component);

	app->get_scene().update_tweens();

	// Allocate a nest
	nest* nest = new ::nest();

	// Setup initial nest parameters
	float tunnel_radius = 1.15f;
	nest->set_tunnel_radius(tunnel_radius);
	nest::shaft* central_shaft = nest->get_central_shaft();
	central_shaft->chirality = -1.0f;
	central_shaft->rotation = vmq::radians(0.0f);
	central_shaft->depth = {0.0f, 200.0f};
	central_shaft->radius = {0.0f, 5.0f};
	central_shaft->pitch = {4.0f, 8.0f};
	central_shaft->translation = {{{0.0f, 0.0f}, {40.0f, 26.0f}}};
	central_shaft->current_depth = 0.0f;
	for (std::size_t i = 0; i < 4; ++i)
	{
		nest::chamber chamber;
		chamber.shaft = central_shaft;
		chamber.depth = (i + 1) * 50.0f;
		chamber.rotation = vmq::radians(0.0f);
		chamber.inner_radius = 4.0f;
		chamber.outer_radius = 10.0f;
		central_shaft->chambers.push_back(chamber);
	}

	// Dig nest shafts
	float shift = 0.1f;
	for (int i = 0; i < 400; ++i)
	{
		ecs::cavity_component cavity;
		cavity.position = nest->extend_shaft(*nest->get_central_shaft());
		cavity.position += float3{frand(-shift, shift), frand(-shift, shift), frand(-shift, shift)};
		cavity.radius = tunnel_radius * frand(1.0f, 1.1f);

		ecs_registry.assign<ecs::cavity_component>(ecs_registry.create(), cavity);
	}

	// Dig nest chambers
	for (int i = 0; i < central_shaft->chambers.size(); ++i)
	{
		for (int j = 0; j < 150; ++j)
		{
			ecs::cavity_component cavity;
			cavity.position = nest->expand_chamber(central_shaft->chambers[i]);
			cavity.position += float3{frand(-shift, shift), frand(-shift, shift), frand(-shift, shift)};
			cavity.radius = tunnel_radius * frand(1.0f, 1.1f);

			ecs_registry.assign<ecs::cavity_component>(ecs_registry.create(), cavity);
		}
	}
	
	// Place larva in chamber
	{
		auto larva_entity = larva_archetype->create(ecs_registry);
		auto& transform = ecs_registry.get<ecs::transform_component>(larva_entity);
		transform.transform = vmq::identity_transform<float>;
		transform.transform.translation = nest->get_shaft_position(*central_shaft, central_shaft->depth[1]);
		//transform.transform.translation.y -= 1.0f;
	}
	
	control_system* control_system = app->get_control_system();
	control_system->update(0.0f);
	control_system->set_nest(nest);
	orbit_cam->update(0.0f);

	logger->close_task(enter_state_task, EXIT_SUCCESS);
}

void exit_play_state(application* app)
{
	logger* logger = app->get_logger();
	logger->log("Exiting play state...\n");
	
	int exit_state_task = logger->open_task("Exiting play state");

	logger->close_task(exit_state_task, EXIT_SUCCESS);
}

