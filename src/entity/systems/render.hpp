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

#ifndef ANTKEEPER_ENTITY_SYSTEM_RENDER_HPP
#define ANTKEEPER_ENTITY_SYSTEM_RENDER_HPP

#include "entity/systems/updatable.hpp"
#include "scene/collection.hpp"
#include "scene/model-instance.hpp"
#include "scene/light.hpp"
#include "entity/components/model.hpp"
#include "entity/components/light.hpp"
#include "entity/id.hpp"
#include <unordered_map>
#include <vector>

class renderer;

namespace entity {
namespace system {

class render: public updatable
{
public:
	render(entity::registry& registry);
	virtual void update(double t, double dt);
	
	void draw(double alpha);
	
	
	void add_layer(scene::collection* layer);
	void remove_layers();
	
	void set_renderer(::renderer* renderer);
	
	scene::model_instance* get_model_instance(entity::id entity_id);
	scene::light* get_light(entity::id entity_id);

private:	
	void update_model_and_materials(entity::id entity_id, entity::component::model& model);
	void update_light(entity::id entity_id, entity::component::light& component);
	
	void on_model_construct(entity::registry& registry, entity::id entity_id, entity::component::model& model);
	void on_model_replace(entity::registry& registry, entity::id entity_id, entity::component::model& model);
	void on_model_destroy(entity::registry& registry, entity::id entity_id);
	
	void on_light_construct(entity::registry& registry, entity::id entity_id, entity::component::light& light);
	void on_light_replace(entity::registry& registry, entity::id entity_id, entity::component::light& light);
	void on_light_destroy(entity::registry& registry, entity::id entity_id);
	
	double t;
	double dt;
	renderer* renderer;
	std::vector<scene::collection*> layers;
	std::unordered_map<entity::id, scene::model_instance*> model_instances;
	std::unordered_map<entity::id, scene::light*> lights;
};

} // namespace system
} // namespace entity

#endif // ANTKEEPER_ENTITY_SYSTEM_RENDER_HPP

