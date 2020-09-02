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

#include "tracking-system.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/marker-component.hpp"
#include "event/event-dispatcher.hpp"
#include "resources/resource-manager.hpp"
#include "scene/scene.hpp"
#include "scene/model-instance.hpp"
#include "math/math.hpp"
#include "renderer/material.hpp"
#include "renderer/model.hpp"
#include "utility/fundamental-types.hpp"
#include "game/entity-commands.hpp"

using namespace ecs;

tracking_system::tracking_system(entt::registry& registry, ::event_dispatcher* event_dispatcher, ::resource_manager* resource_manager):
	entity_system(registry),
	event_dispatcher(event_dispatcher),
	resource_manager(resource_manager),
	scene(nullptr)
{
	registry.on_construct<trackable_component>().connect<&tracking_system::on_component_construct>(this);
	registry.on_destroy<trackable_component>().connect<&tracking_system::on_component_destroy>(this);
	
	// Load paint ball model
	paint_ball_model = resource_manager->load<model>("paint-ball.obj");
	
	// Load tracker model
	tracker_model = resource_manager->load<model>("tracker.obj");
	
	// Load paint ball materials
	paint_ball_materials = new material*[7];
	paint_ball_materials[0] = resource_manager->load<material>("paint-ball-yellow.mtl");
	paint_ball_materials[1] = resource_manager->load<material>("paint-ball-green.mtl");
	paint_ball_materials[2] = resource_manager->load<material>("paint-ball-blue.mtl");
	paint_ball_materials[3] = resource_manager->load<material>("paint-ball-purple.mtl");
	paint_ball_materials[4] = resource_manager->load<material>("paint-ball-pink.mtl");
	paint_ball_materials[5] = resource_manager->load<material>("paint-ball-red.mtl");
	paint_ball_materials[6] = resource_manager->load<material>("paint-ball-orange.mtl");
	
	event_dispatcher->subscribe<tool_pressed_event>(this);
	event_dispatcher->subscribe<tool_released_event>(this);
}

tracking_system::~tracking_system()
{
	event_dispatcher->unsubscribe<tool_pressed_event>(this);
	event_dispatcher->unsubscribe<tool_released_event>(this);
	
	for (auto it = trackers.begin(); it != trackers.end(); ++it)
	{
		delete it->second;
	}
	
	delete[] paint_ball_materials;
}


void tracking_system::update(double t, double dt)
{
	for (auto it = trackers.begin(); it != trackers.end(); ++it)
	{
		const transform_component& transform = registry.get<transform_component>(it->first);
		
		// Project world coordinates to screen coordinates
		
		// Update billboard position
		it->second->set_translation(transform.world.translation);
		if (transform.warp)
		{
			it->second->update_tweens();
		}
	}
}

void tracking_system::set_scene(::scene* scene)
{
	this->scene = scene;
}

void tracking_system::on_component_construct(entt::registry& registry, entt::entity entity, trackable_component& component)
{

}

void tracking_system::on_component_destroy(entt::registry& registry, entt::entity entity)
{
	if (auto it = trackers.find(entity); it != trackers.end())
	{
		// Remove model instance from all layers
		/*
		for (std::size_t i = 0; i < layers.size(); ++i)
		{
			layers[i]->remove_object(it->second);
		}
		*/
		
		delete it->second;
		trackers.erase(it);
	}
}

void tracking_system::handle_event(const tool_pressed_event& event)
{
	if (registry.has<marker_component>(event.entity))
	{
		math::transform<float> transform = ec::get_world_transform(registry, event.entity);
		
		int marker_index = registry.get<marker_component>(event.entity).color;
		
		if (marker_index > 0)
		{
			const float tracker_scale = 1.0f;
			
			// Create tracker model instance
			model_instance* instance = new model_instance();
			instance->set_model(tracker_model);
			instance->set_translation(transform.translation);
			instance->set_scale(float3{tracker_scale, tracker_scale, tracker_scale});
			
			// Set tracker paint ball material
			const model_group* paint_ball_model_group = tracker_model->get_group("paint-ball");
			if (paint_ball_model_group != nullptr)
			{
				instance->set_material(paint_ball_model_group->get_index(), paint_ball_materials[marker_index - 1]);
			}

			instance->update_tweens();
			
			scene->add_object(instance);
		}
	}
}

void tracking_system::handle_event(const tool_released_event& event)
{

}
