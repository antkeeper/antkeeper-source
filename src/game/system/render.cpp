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

#include "render.hpp"
#include "game/component/transform.hpp"
#include "game/component/camera.hpp"
#include "scene/point-light.hpp"
#include "scene/directional-light.hpp"
#include "scene/ambient-light.hpp"
#include "scene/spot-light.hpp"

namespace game {
namespace system {

render::render(entity::registry& registry):
	updatable(registry),
	t(0.0),
	dt(0.0),
	renderer(nullptr)
{
	registry.on_construct<component::model>().connect<&render::on_model_construct>(this);
	registry.on_update<component::model>().connect<&render::on_model_update>(this);
	registry.on_destroy<component::model>().connect<&render::on_model_destroy>(this);
	registry.on_construct<component::light>().connect<&render::on_light_construct>(this);
	registry.on_update<component::light>().connect<&render::on_light_update>(this);
	registry.on_destroy<component::light>().connect<&render::on_light_destroy>(this);
}

render::~render()
{
	registry.on_construct<component::model>().disconnect<&render::on_model_construct>(this);
	registry.on_update<component::model>().disconnect<&render::on_model_update>(this);
	registry.on_destroy<component::model>().disconnect<&render::on_model_destroy>(this);
	registry.on_construct<component::light>().disconnect<&render::on_light_construct>(this);
	registry.on_update<component::light>().disconnect<&render::on_light_update>(this);
	registry.on_destroy<component::light>().disconnect<&render::on_light_destroy>(this);
}

void render::update(double t, double dt)
{
	this->t = t;
	this->dt = dt;
	
	// Update model instance transforms
	registry.view<component::transform, component::model>().each
	(
		[this](entity::id entity_id, auto& transform, auto& model)
		{
			scene::model_instance* instance = model_instances[entity_id];

			instance->set_transform(transform.world);
			if (transform.warp)
			{
				instance->get_transform_tween().update();
				instance->update_tweens();
				transform.warp = false;
			}
		}
	);
	
	// Update camera transforms
	registry.view<component::transform, component::camera>().each
	(
		[this](entity::id entity_id, auto& transform, auto& camera)
		{
			camera.object->set_transform(transform.world);
			if (transform.warp)
			{
				camera.object->get_transform_tween().update();
				camera.object->update_tweens();
				transform.warp = false;
			}
		}
	);
	
	// Update light transforms
	registry.view<component::transform, component::light>().each
	(
		[this](entity::id entity_id, auto& transform, auto& light)
		{
			scene::light* light_object = lights[entity_id];

			light_object->set_transform(transform.world);
			if (transform.warp)
			{
				light_object->get_transform_tween().update();
				light_object->update_tweens();
				transform.warp = false;
			}
		}
	);
}

void render::draw(double alpha)
{
	if (renderer)
	{
		for (const scene::collection* collection: layers)
		{
			renderer->render(static_cast<float>(t + dt * alpha), static_cast<float>(dt), static_cast<float>(alpha), *collection);
		}
	}
}

void render::add_layer(scene::collection* layer)
{
	layers.push_back(layer);
}

void render::remove_layers()
{
	layers.clear();
}

void render::set_renderer(::render::renderer* renderer)
{
	this->renderer = renderer;
}

scene::model_instance* render::get_model_instance(entity::id entity_id)
{
	if (auto it = model_instances.find(entity_id); it != model_instances.end())
		return it->second;
	return nullptr;
}

scene::light* render::get_light(entity::id entity_id)
{
	if (auto it = lights.find(entity_id); it != lights.end())
		return it->second;
	return nullptr;
}

void render::update_model_and_materials(entity::id entity_id, component::model& model)
{
	if (auto model_it = model_instances.find(entity_id); model_it != model_instances.end())
	{
		model_it->second->set_model(model.render_model);
		model_it->second->set_instanced((model.instance_count > 0), model.instance_count);
		
		for (auto material_it = model.materials.begin(); material_it != model.materials.end(); ++material_it)
		{
			model_it->second->set_material(material_it->first, material_it->second);
		}
		
		// Add model instance to its specified layers
		for (std::size_t i = 0; i < std::min<std::size_t>(layers.size(), (sizeof(model.layers) << 3)); ++i)
		{
			layers[i]->remove_object(model_it->second);
			
			if ((model.layers >> i) & 1)
			{
				layers[i]->add_object(model_it->second);
			}
		}
	}
}

void render::update_light(entity::id entity_id, game::component::light& component)
{
	if (auto light_it = lights.find(entity_id); light_it != lights.end())
	{
		scene::light* light = light_it->second;
		
		light->set_color(component.color);
		light->set_intensity(component.intensity);
		
		switch (light->get_light_type())
		{
			case scene::light_type::point:
			{
				scene::point_light* point = static_cast<scene::point_light*>(light);
				point->set_attenuation(component.attenuation);
				break;
			}
			
			case scene::light_type::spot:
			{
				scene::spot_light* spot = static_cast<scene::spot_light*>(light);
				spot->set_attenuation(component.attenuation);
				spot->set_cutoff(component.cutoff);
				break;
			}
			
			default:
				break;
		}
	}
}

void render::on_model_construct(entity::registry& registry, entity::id entity_id)
{
	game::component::model& component = registry.get<game::component::model>(entity_id);
	
	scene::model_instance* model_instance = new scene::model_instance();	
	model_instances[entity_id] = model_instance;
	update_model_and_materials(entity_id, component);
}

void render::on_model_update(entity::registry& registry, entity::id entity_id)
{
	game::component::model& component = registry.get<game::component::model>(entity_id);
	update_model_and_materials(entity_id, component);
}

void render::on_model_destroy(entity::registry& registry, entity::id entity_id)
{
	if (auto it = model_instances.find(entity_id); it != model_instances.end())
	{
		scene::model_instance* model_instance = it->second;
		
		// Remove model instance from all layers
		for (scene::collection* layer: layers)
			layer->remove_object(model_instance);

		model_instances.erase(it);
		delete model_instance;
	}
}

void render::on_light_construct(entity::registry& registry, entity::id entity_id)
{
	game::component::light& component = registry.get<game::component::light>(entity_id);
	
	scene::light* light = nullptr;
	
	switch (component.type)
	{
		case scene::light_type::ambient:
			light = new scene::ambient_light();
			break;
		
		case scene::light_type::directional:
			light = new scene::directional_light();
			break;
		
		case scene::light_type::point:
			light = new scene::point_light();
			break;
		
		case scene::light_type::spot:
			light = new scene::spot_light();
			break;
		
		default:
			break;
	}
	
	if (light)
	{
		lights[entity_id] = light;
		for (scene::collection* layer: layers)
			layer->add_object(light);
		
		update_light(entity_id, component);
	}
}

void render::on_light_update(entity::registry& registry, entity::id entity_id)
{
	game::component::light& component = registry.get<game::component::light>(entity_id);
	update_light(entity_id, component);
}

void render::on_light_destroy(entity::registry& registry, entity::id entity_id)
{
	if (auto it = lights.find(entity_id); it != lights.end())
	{
		scene::light* light = it->second;
		
		for (scene::collection* layer: layers)
			layer->remove_object(light);
		
		lights.erase(it);
		delete light;
	}
}

} // namespace system
} // namespace game
