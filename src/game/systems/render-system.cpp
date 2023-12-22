// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/systems/render-system.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include <algorithm>
#include <execution>

render_system::render_system(entity::registry& registry):
	updatable_system(registry),
	m_updated_scene_transforms(registry, entt::collector.update<transform_component>().where<scene_component>(entt::exclude<rigid_body_component>)),
	m_t(0.0),
	m_dt(0.0),
	m_renderer(nullptr)
{
	m_registry.on_construct<scene_component>().connect<&render_system::on_scene_construct>(this);
	m_registry.on_update<scene_component>().connect<&render_system::on_scene_update>(this);
	m_registry.on_destroy<scene_component>().connect<&render_system::on_scene_destroy>(this);
	
	m_registry.on_construct<transform_component>().connect<&render_system::on_transform_construct>(this);
}

render_system::~render_system()
{
	m_registry.on_construct<scene_component>().disconnect<&render_system::on_scene_construct>(this);
	m_registry.on_update<scene_component>().disconnect<&render_system::on_scene_update>(this);
	m_registry.on_destroy<scene_component>().disconnect<&render_system::on_scene_destroy>(this);
	
	m_registry.on_construct<transform_component>().disconnect<&render_system::on_transform_construct>(this);
}

void render_system::update(float t, float dt)
{
	m_t = t;
	m_dt = dt;
	
	std::for_each
	(
		std::execution::par_unseq,
		m_updated_scene_transforms.begin(),
		m_updated_scene_transforms.end(),
		[&](auto entity_id)
		{
			auto& transform = m_registry.get<transform_component>(entity_id);
			const auto& scene = m_registry.get<scene_component>(entity_id);
			
			// WARNING: could potentially lead to multithreading issues with scene::object_base::transformed()
			scene.object->set_transform(transform.world);
		}
	);
	m_updated_scene_transforms.clear();
}

void render_system::draw(float alpha)
{
	if (m_renderer)
	{
		for (scene::collection* collection: m_layers)
		{
			m_renderer->render(m_t + m_dt * alpha, m_dt, alpha, *collection);
		}
	}
}

void render_system::add_layer(scene::collection* layer)
{
	m_layers.push_back(layer);
}

void render_system::remove_layers()
{
	m_layers.clear();
}

void render_system::set_renderer(render::renderer* renderer)
{
	m_renderer = renderer;
}

void render_system::on_scene_construct(entity::registry& registry, entity::id entity_id)
{
	const auto& component = registry.get<::scene_component>(entity_id);
	
	// Update scene object transform with pre-existing transform component
	if (const auto transform = registry.try_get<transform_component>(entity_id))
	{
		component.object->set_transform(transform->world);
	}
	
	for (std::size_t i = 0; i < m_layers.size(); ++i)
	{
		if (component.layer_mask & static_cast<std::uint8_t>(1 << i))
		{
			m_layers[i]->add_object(*component.object);
		}
	}
}

void render_system::on_scene_update(entity::registry& registry, entity::id entity_id)
{
	const auto& component = registry.get<::scene_component>(entity_id);
	
	for (std::size_t i = 0; i < m_layers.size(); ++i)
	{
		// Remove from layer
		scene::collection* layer = m_layers[i];
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
	
	for (std::size_t i = 0; i < m_layers.size(); ++i)
	{
		if (component.layer_mask & static_cast<std::uint8_t>(1 << i))
		{
			m_layers[i]->remove_object(*component.object);
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
