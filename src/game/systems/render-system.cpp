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

#include "game/systems/render-system.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include <algorithm>
#include <execution>

render_system::render_system(entity::registry& registry):
	updatable_system(registry),
	updated_scene_transforms(registry, entt::collector.update<transform_component>().where<scene_component>(entt::exclude<rigid_body_component>)),
	t(0.0),
	dt(0.0),
	renderer(nullptr)
{
	registry.on_construct<scene_component>().connect<&render_system::on_scene_construct>(this);
	registry.on_update<scene_component>().connect<&render_system::on_scene_update>(this);
	registry.on_destroy<scene_component>().connect<&render_system::on_scene_destroy>(this);
	
	registry.on_construct<transform_component>().connect<&render_system::on_transform_construct>(this);
}

render_system::~render_system()
{
	registry.on_construct<scene_component>().disconnect<&render_system::on_scene_construct>(this);
	registry.on_update<scene_component>().disconnect<&render_system::on_scene_update>(this);
	registry.on_destroy<scene_component>().disconnect<&render_system::on_scene_destroy>(this);
	
	registry.on_construct<transform_component>().disconnect<&render_system::on_transform_construct>(this);
}

void render_system::update(float t, float dt)
{
	this->t = t;
	this->dt = dt;
	
	std::for_each
	(
		std::execution::par_unseq,
		updated_scene_transforms.begin(),
		updated_scene_transforms.end(),
		[&](auto entity_id)
		{
			auto& transform = registry.get<transform_component>(entity_id);
			const auto& scene = registry.get<scene_component>(entity_id);
			
			// WARNING: could potentially lead to multithreading issues with scene::object_base::transformed()
			scene.object->set_transform(transform.world);
		}
	);
	updated_scene_transforms.clear();
}

void render_system::draw(float alpha)
{
	if (renderer)
	{
		for (scene::collection* collection: layers)
		{
			renderer->render(t + dt * alpha, dt, alpha, *collection);
		}
	}
}

void render_system::add_layer(scene::collection* layer)
{
	layers.push_back(layer);
}

void render_system::remove_layers()
{
	layers.clear();
}

void render_system::set_renderer(render::renderer* renderer)
{
	this->renderer = renderer;
}

void render_system::on_scene_construct(entity::registry& registry, entity::id entity_id)
{
	const auto& component = registry.get<::scene_component>(entity_id);
	
	// Update scene object transform with pre-existing transform component
	if (const auto transform = registry.try_get<transform_component>(entity_id))
	{
		component.object->set_transform(transform->world);
	}
	
	for (std::size_t i = 0; i < layers.size(); ++i)
	{
		if (component.layer_mask & static_cast<std::uint8_t>(1 << i))
		{
			layers[i]->add_object(*component.object);
		}
	}
}

void render_system::on_scene_update(entity::registry& registry, entity::id entity_id)
{
	const auto& component = registry.get<::scene_component>(entity_id);
	
	for (std::size_t i = 0; i < layers.size(); ++i)
	{
		// Remove from layer
		scene::collection* layer = layers[i];
		layer->remove_object(*component.object);
		
		if (component.layer_mask & static_cast<std::uint8_t>(1 << i))
		{
			// Add to layer
			layer->add_object(*component.object);
		}
	}
}

void render_system::on_scene_destroy(entity::registry& registry, entity::id entity_id)
{
	const auto& component = registry.get<::scene_component>(entity_id);
	
	for (std::size_t i = 0; i < layers.size(); ++i)
	{
		if (component.layer_mask & static_cast<std::uint8_t>(1 << i))
		{
			layers[i]->remove_object(*component.object);
		}
	}
}

void render_system::on_transform_construct(entity::registry& registry, entity::id entity_id)
{
	// Update pre-existing scene object transform withtransform component
	if (const auto scene = registry.try_get<scene_component>(entity_id))
	{
		const auto& transform = registry.get<transform_component>(entity_id);
		
		scene->object->set_transform(transform.world);
	}
}
