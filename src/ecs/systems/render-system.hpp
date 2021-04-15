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

#ifndef ANTKEEPER_ECS_RENDER_SYSTEM_HPP
#define ANTKEEPER_ECS_RENDER_SYSTEM_HPP

#include "entity-system.hpp"
#include "scene/collection.hpp"
#include "scene/model-instance.hpp"
#include "scene/light.hpp"
#include "ecs/components/model-component.hpp"
#include "ecs/components/light-component.hpp"
#include "ecs/entity.hpp"
#include <unordered_map>
#include <vector>

class renderer;

namespace ecs {

class render_system: public entity_system
{
public:
	render_system(ecs::registry& registry);
	virtual void update(double t, double dt);
	
	void render(double alpha);
	
	
	void add_layer(scene::collection* layer);
	void remove_layers();
	
	void set_renderer(::renderer* renderer);
	
	scene::model_instance* get_model_instance(ecs::entity entity);
	scene::light* get_light(ecs::entity entity);

private:	
	void update_model_and_materials(ecs::entity entity, ecs::model_component& model);
	void update_light(ecs::entity entity, ecs::light_component& component);
	
	void on_model_construct(ecs::registry& registry, ecs::entity entity, ecs::model_component& model);
	void on_model_replace(ecs::registry& registry, ecs::entity entity, ecs::model_component& model);
	void on_model_destroy(ecs::registry& registry, ecs::entity entity);
	
	void on_light_construct(ecs::registry& registry, ecs::entity entity, ecs::light_component& light);
	void on_light_replace(ecs::registry& registry, ecs::entity entity, ecs::light_component& light);
	void on_light_destroy(ecs::registry& registry, ecs::entity entity);
	
	renderer* renderer;
	std::vector<scene::collection*> layers;
	std::unordered_map<entity, scene::model_instance*> model_instances;
	std::unordered_map<entity, scene::light*> lights;
};

} // namespace ecs

#endif // ANTKEEPER_ECS_RENDER_SYSTEM_HPP

