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

#include "animation/ease.hpp"
#include "animation/screen-transition.hpp"
#include "configuration.hpp"
#include "debug/logger.hpp"
#include "entity/archetype.hpp"
#include "game/components/cavity-component.hpp"
#include "game/components/copy-transform-component.hpp"
#include "game/components/copy-translation-component.hpp"
#include "game/components/model-component.hpp"
#include "game/components/snap-component.hpp"
#include "game/components/samara-component.hpp"
#include "game/components/terrain-component.hpp"
#include "game/components/tool-component.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/camera-follow-component.hpp"
#include "game/entity-commands.hpp"
#include "game/game-context.hpp"
#include "game/states/game-states.hpp"
#include "math/math.hpp"
#include "nest.hpp"
#include "renderer/material.hpp"
#include "rasterizer/texture-2d.hpp"
#include "rasterizer/texture-filter.hpp"
#include "rasterizer/texture-wrapping.hpp"
#include "renderer/model.hpp"
#include "renderer/passes/sky-pass.hpp"
#include "resources/resource-manager.hpp"
#include "scene/model-instance.hpp"
#include "scene/scene.hpp"
#include "scene/camera.hpp"
#include "scene/ambient-light.hpp"
#include "scene/directional-light.hpp"
#include "scene/directional-light.hpp"
#include "game/systems/control-system.hpp"
#include "game/systems/camera-system.hpp"
#include "game/systems/render-system.hpp"
#include "game/systems/tool-system.hpp"
#include "game/biome.hpp"
#include "utility/fundamental-types.hpp"
#include "utility/gamma.hpp"

void play_state_enter(game_context* ctx)
{
	logger* logger = ctx->logger;
	logger->push_task("Entering play state");
	
	// Load biome
	if (ctx->option_biome.has_value())
	{
		ctx->biome = ctx->resource_manager->load<biome>(ctx->option_biome.value() + ".bio");
	}
	else
	{
		ctx->biome = ctx->resource_manager->load<biome>("grassland.bio");
	}
	
	// Apply biome parameters to scene
	ctx->sun_indirect->set_color(ctx->biome->ambient_color);
	ctx->sun_indirect->set_intensity(ctx->biome->ambient_intensity);
	
	math::quaternion<float> sun_azimuth_rotation = math::angle_axis(ctx->biome->sun_azimuth, float3{0, 1, 0});
	math::quaternion<float> sun_elevation_rotation = math::angle_axis(ctx->biome->sun_elevation, float3{-1, 0, 0});
	math::quaternion<float> sun_rotation = math::normalize(sun_azimuth_rotation * sun_elevation_rotation);
	
	ctx->sun_direct->set_rotation(sun_rotation);
	ctx->sun_direct->set_color(ctx->biome->sun_color);
	ctx->sun_direct->set_intensity(ctx->biome->sun_intensity);

	sky_pass* sky_pass = ctx->overworld_sky_pass;
	sky_pass->set_enabled(true);
	sky_pass->set_sun_angular_radius(ctx->biome->sun_angular_radius);
	sky_pass->set_sun_color(ctx->biome->sun_color * ctx->biome->sun_intensity);
	sky_pass->set_horizon_color(ctx->biome->horizon_color);
	sky_pass->set_zenith_color(ctx->biome->zenith_color);
	
	texture_2d* sky_palette = ctx->resource_manager->load<texture_2d>("sky-palette.png");
	sky_palette->set_wrapping(texture_wrapping::repeat, texture_wrapping::clamp);
	sky_palette->set_filters(texture_min_filter::linear, texture_mag_filter::linear);
	sky_pass->set_sky_palette(sky_palette);
	
	ctx->tool_system->set_sun_direction(ctx->sun_direct->get_direction());

	resource_manager* resource_manager = ctx->resource_manager;
	entt::registry& ecs_registry = *ctx->ecs_registry;

	// Load entity archetypes
	ecs::archetype* ant_hill_archetype = resource_manager->load<ecs::archetype>("ant-hill.ent");
	ecs::archetype* maple_tree_archetype = resource_manager->load<ecs::archetype>("maple-tree.ent");
	ecs::archetype* nest_archetype = resource_manager->load<ecs::archetype>("harvester-nest.ent");
	ecs::archetype* samara_archetype = resource_manager->load<ecs::archetype>("samara.ent");
	ecs::archetype* forceps_archetype = resource_manager->load<ecs::archetype>("forceps.ent");
	ecs::archetype* lens_archetype = resource_manager->load<ecs::archetype>("lens.ent");
	ecs::archetype* brush_archetype = resource_manager->load<ecs::archetype>("brush.ent");
	ecs::archetype* marker_archetype = resource_manager->load<ecs::archetype>("marker.ent");
	ecs::archetype* container_archetype = resource_manager->load<ecs::archetype>("container.ent");
	ecs::archetype* twig_archetype = resource_manager->load<ecs::archetype>("twig.ent");
	ecs::archetype* larva_archetype = resource_manager->load<ecs::archetype>("larva.ent");
	ecs::archetype* pebble_archetype = resource_manager->load<ecs::archetype>("pebble.ent");
	ecs::archetype* flashlight_archetype = resource_manager->load<ecs::archetype>("flashlight.ent");
	ecs::archetype* flashlight_light_cone_archetype = resource_manager->load<ecs::archetype>("flashlight-light-cone.ent");
	ecs::archetype* lens_light_cone_archetype = resource_manager->load<ecs::archetype>("lens-light-cone.ent");
	ecs::archetype* ant_head_archetype = resource_manager->load<ecs::archetype>("ant-head.ent");
	ecs::archetype* dandelion_plant_archetype = resource_manager->load<ecs::archetype>("dandelion-plant.ent");
	
	// Create tools
	forceps_archetype->assign(ecs_registry, ctx->forceps_entity);
	lens_archetype->assign(ecs_registry, ctx->lens_entity);
	brush_archetype->assign(ecs_registry, ctx->brush_entity);
	marker_archetype->assign(ecs_registry, ctx->marker_entity);
	container_archetype->assign(ecs_registry, ctx->container_entity);
	twig_archetype->assign(ecs_registry, ctx->twig_entity);
		
	// Create flashlight and light cone, set light cone parent to flashlight, and move both to underworld scene
	flashlight_archetype->assign(ecs_registry, ctx->flashlight_entity);
	auto flashlight_light_cone = flashlight_light_cone_archetype->create(ecs_registry);
	ec::parent(ecs_registry, flashlight_light_cone, ctx->flashlight_entity);
	ec::assign_render_layers(ecs_registry, ctx->flashlight_entity, 2);
	
	// Make lens tool's model instance unculled, so its shadow is always visible.
	model_instance* lens_model_instance = ctx->render_system->get_model_instance(ctx->lens_entity);
	if (lens_model_instance)
	{
		lens_model_instance->set_culling_mask(&ctx->no_cull);
	}
	
	// Create lens light cone and set its parent to lens
	auto lens_light_cone = lens_light_cone_archetype->create(ecs_registry);
	//ec::bind_transform(ecs_registry, lens_light_cone, ctx->lens_entity);
	ec::parent(ecs_registry, lens_light_cone, ctx->lens_entity);
	

	
	// Hide inactive tools
	ec::assign_render_layers(ecs_registry, ctx->forceps_entity, 0);
	ec::assign_render_layers(ecs_registry, ctx->brush_entity, 0);
	ec::assign_render_layers(ecs_registry, ctx->lens_entity, 0);
	ec::assign_render_layers(ecs_registry, ctx->marker_entity, 0);
	ec::assign_render_layers(ecs_registry, ctx->container_entity, 0);
	ec::assign_render_layers(ecs_registry, ctx->twig_entity, 0);
	
	// Activate brush tool
	ctx->tool_system->set_active_tool(ctx->brush_entity);

	// Create ant-hill
	auto ant_hill_entity = ant_hill_archetype->create(ecs_registry);
	ec::place(ecs_registry, ant_hill_entity, {0, 0});
	
	// Generate pebbles
	float pebble_radius = 300.0f;
	int pebble_count = 100;
	for (int i = 0; i < pebble_count; ++i)
	{
		float x = math::random(-pebble_radius, pebble_radius);
		float z = math::random(-pebble_radius, pebble_radius);
		
		auto pebble_entity = ant_head_archetype->create(ecs_registry);
		
		auto& transform = ecs_registry.get<ecs::transform_component>(pebble_entity);
		transform.local = math::identity_transform<float>;
		transform.local.rotation = math::angle_axis(math::random(0.0f, math::two_pi<float>), {0, 1, 0});
		transform.local.scale = float3{1, 1, 1} * math::random(0.75f, 1.25f);
		
		ec::place(ecs_registry, pebble_entity, {x, z});
	}

	// Create maple tree
	auto maple_tree_entity = maple_tree_archetype->create(ecs_registry);
	ec::place(ecs_registry, maple_tree_entity, {300, 200});

	// Creat nest
	auto nest_entity = nest_archetype->create(ecs_registry);

	// Create terrain
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

	// Create samaras
	for (int i = 0; i < 15; ++i)
	{
		auto samara_entity = samara_archetype->create(ecs_registry);

		auto& transform = ecs_registry.get<ecs::transform_component>(samara_entity);
		float zone = 200.0f;
		transform.local = math::identity_transform<float>;
		transform.local.translation.x = math::random(-zone, zone);
		transform.local.translation.y = math::random(50.0f, 150.0f);
		transform.local.translation.z = math::random(-zone, zone);

		ecs::samara_component samara_component;
		samara_component.angle = math::random(0.0f, math::radians(360.0f));
		samara_component.direction = math::normalize(float3{math::random(-1.0f, 1.0f), math::random(-1.0f, -5.0f), math::random(-1.0f, 1.0f)});
		samara_component.chirality = (math::random(0.0f, 1.0f) < 0.5f) ? -1.0f : 1.0f;

		ecs_registry.assign_or_replace<ecs::samara_component>(samara_entity, samara_component);
	}
	
	// Setup camera focal point
	ecs::transform_component focal_point_transform;
	focal_point_transform.local = math::identity_transform<float>;
	focal_point_transform.warp = true;
	ecs::camera_follow_component focal_point_follow;
	ecs::snap_component focal_point_snap;
	focal_point_snap.ray = {float3{0, 10000, 0}, float3{0, -1, 0}};
	focal_point_snap.warp = false;
	focal_point_snap.relative = true;
	focal_point_snap.autoremove = false;
	ecs_registry.assign_or_replace<ecs::transform_component>(ctx->focal_point_entity, focal_point_transform);
	ecs_registry.assign_or_replace<ecs::camera_follow_component>(ctx->focal_point_entity, focal_point_follow);
	ecs_registry.assign_or_replace<ecs::snap_component>(ctx->focal_point_entity, focal_point_snap);

	// Setup camera
	ctx->overworld_camera->look_at({0, 0, 1}, {0, 0, 0}, {0, 1, 0});
	ctx->camera_system->set_camera(ctx->overworld_camera);

	auto ant_head = ant_head_archetype->create(ecs_registry);
	ec::place(ecs_registry, ant_head, {50, 0});

	ctx->overworld_scene->update_tweens();

	// Allocate a nest
	nest* nest = new ::nest();

	// Setup initial nest parameters
	float tunnel_radius = 1.15f;
	nest->set_tunnel_radius(tunnel_radius);
	nest::shaft* central_shaft = nest->get_central_shaft();
	central_shaft->chirality = 1.0f;
	central_shaft->rotation = math::radians(0.0f);
	central_shaft->depth = {0.0f, 200.0f};
	central_shaft->radius = {15.0f, 15.0f};
	central_shaft->pitch = {40.0f, 40.0f};
	central_shaft->translation = {{{0.0f, 0.0f}, {0.0f, 0.0f}}};
	central_shaft->current_depth = 0.0f;
	for (std::size_t i = 0; i < 4; ++i)
	{
		nest::chamber chamber;
		chamber.shaft = central_shaft;
		chamber.depth = (i + 1) * 50.0f;
		chamber.rotation = math::radians(0.0f);
		chamber.inner_radius = 4.0f;
		chamber.outer_radius = 10.0f;
		central_shaft->chambers.push_back(chamber);
	}

	// Dig nest shafts
	float shift = 0.1f;
	for (int i = 0; i < 800; ++i)
	{
		ecs::cavity_component cavity;
		cavity.position = nest->extend_shaft(*nest->get_central_shaft());
		cavity.position += float3{math::random(-shift, shift), math::random(-shift, shift), math::random(-shift, shift)};
		cavity.radius = tunnel_radius * math::random(1.0f, 1.1f);

		ecs_registry.assign<ecs::cavity_component>(ecs_registry.create(), cavity);
	}

	// Dig nest chambers
	/*
	for (int i = 0; i < central_shaft->chambers.size(); ++i)
	{
		for (int j = 0; j < 150; ++j)
		{
			ecs::cavity_component cavity;
			cavity.position = nest->expand_chamber(central_shaft->chambers[i]);
			cavity.position += float3{math::random(-shift, shift), math::random(-shift, shift), math::random(-shift, shift)};
			cavity.radius = tunnel_radius * math::random(1.0f, 1.1f);

			ecs_registry.assign<ecs::cavity_component>(ecs_registry.create(), cavity);
		}
	}
	*/
	
	// Place larva in chamber
	{
		auto larva = larva_archetype->create(ecs_registry);
		ec::assign_render_layers(ecs_registry, larva, 1);
		ec::warp_to(ecs_registry, larva, {50, 0.1935f, 10});
		//auto& transform = ecs_registry.get<ecs::transform_component>(larva_entity);
		//transform.transform = math::identity_transform<float>;
		//transform.transform.translation = nest->get_shaft_position(*central_shaft, central_shaft->depth[1]);
		//transform.transform.translation.y -= 1.0f;
	}
	
	auto dandelion_plant = dandelion_plant_archetype->create(ecs_registry);
	ec::place(ecs_registry, dandelion_plant, {55, -30});
	
	control_system* control_system = ctx->control_system;
	control_system->update(0.0, 0.0);
	control_system->set_nest(nest);
	
	// Start fade in
	ctx->fade_transition->transition(1.0f, true, ease<float>::in_quad);

	logger->pop_task(EXIT_SUCCESS);
	
	std::string biome_name = (*ctx->strings)[ctx->biome->name];
	logger->log("Entered biome \"" + biome_name + "\"");
}

void play_state_exit(game_context* ctx)
{
	logger* logger = ctx->logger;
	logger->push_task("Exiting play state");
	
	logger->pop_task(EXIT_SUCCESS);
}

