// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/systems/render-system.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include "game/components/scene-object-component.hpp"
import engine.math.quaternion;
import engine.math.transform;
import engine.utility.sized_types;

using namespace engine;

template <>
void render_system::on_construct<scene_object_component>(entity::registry& registry, entity::id entity_id)
{
	const auto& component = registry.get<::scene_object_component>(entity_id);

	// Update scene object transform with pre-existing transform component
	if (const auto transform = registry.try_get<transform_component>(entity_id))
	{
		component.object->set_transform(transform->world);
	}

	for (usize i = 0; i < m_layers.size(); ++i)
	{
		if (component.layer_mask & static_cast<u8>(1 << i))
		{
			m_layers[i]->add_object(*component.object);
		}
	}
}

template <>
void render_system::on_update<scene_object_component>(entity::registry& registry, entity::id entity_id)
{
	const auto& component = registry.get<::scene_object_component>(entity_id);

	for (usize i = 0; i < m_layers.size(); ++i)
	{
		// Remove from layer
		scene::collection* layer = m_layers[i];
		layer->remove_object(*component.object);

		if (component.layer_mask & static_cast<u8>(1 << i))
		{
			// Add to layer
			layer->add_object(*component.object);
		}
	}
}

template <>
void render_system::on_destroy<scene_object_component>(entity::registry& registry, entity::id entity_id)
{
	const auto& component = registry.get<::scene_object_component>(entity_id);

	for (usize i = 0; i < m_layers.size(); ++i)
	{
		if (component.layer_mask & static_cast<u8>(1 << i))
		{
			m_layers[i]->remove_object(*component.object);
		}
	}
}

template <>
void render_system::on_construct<transform_component>(entity::registry& registry, entity::id entity_id)
{
	// Update pre-existing scene object transform withtransform component
	if (const auto scene = registry.try_get<scene_object_component>(entity_id))
	{
		const auto& transform = registry.get<transform_component>(entity_id);
		scene->object->set_transform(transform.world);
	}
}

render_system::render_system(entity::registry& registry):
	m_registry(registry),
	m_transformed_scene_object_components(registry, entt::collector.update<transform_component>().where<scene_object_component>(entt::exclude<rigid_body_component>)),
	m_renderer(nullptr)
{
	// Connect scene component observers
	m_registry.on_construct<scene_object_component>().connect<&render_system::on_construct<scene_object_component>>(this);
	m_registry.on_update<scene_object_component>().connect<&render_system::on_update<scene_object_component>>(this);
	m_registry.on_destroy<scene_object_component>().connect<&render_system::on_destroy<scene_object_component>>(this);
	
	// Connect transform component observers
	m_registry.on_construct<transform_component>().connect<&render_system::on_construct<transform_component>>(this);
}

render_system::~render_system()
{
	// Disconnect scene component observers
	m_registry.on_construct<scene_object_component>().disconnect<&render_system::on_construct<scene_object_component>>(this);
	m_registry.on_update<scene_object_component>().disconnect<&render_system::on_update<scene_object_component>>(this);
	m_registry.on_destroy<scene_object_component>().disconnect<&render_system::on_destroy<scene_object_component>>(this);
	
	// Disconnect transform component observers
	m_registry.on_construct<transform_component>().disconnect<&render_system::on_construct<transform_component>>(this);
}

void render_system::fixed_update(entity::registry& registry, float, float)
{
	// Update scene object transforms
	for (auto entity_id: m_transformed_scene_object_components)
	{
		const auto& transform = registry.get<transform_component>(entity_id);
		const auto& scene = registry.get<scene_object_component>(entity_id);
		scene.object->set_transform(transform.world);
	}
	m_transformed_scene_object_components.clear();
}

void render_system::variable_update(entity::registry&, float t, float dt, float alpha)
{
	if (m_renderer)
	{
		for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it)
		{
			m_renderer->render(t + dt * alpha, dt, alpha, **it);
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

