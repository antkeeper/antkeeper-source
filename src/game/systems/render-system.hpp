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

#ifndef ANTKEEPER_GAME_RENDER_SYSTEM_HPP
#define ANTKEEPER_GAME_RENDER_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"
#include <engine/scene/collection.hpp>
#include <engine/scene/model-instance.hpp>
#include <engine/scene/light.hpp>
#include "game/components/model-component.hpp"
#include "game/components/light-component.hpp"
#include <engine/entity/id.hpp>
#include <engine/render/renderer.hpp>
#include <unordered_map>
#include <vector>
#include <memory>

class render_system: public updatable_system
{
public:
	explicit render_system(entity::registry& registry);
	~render_system();
	
	virtual void update(float t, float dt);
	
	void draw(float alpha);
	
	
	void add_layer(scene::collection* layer);
	void remove_layers();
	
	void set_renderer(::render::renderer* renderer);
	
	scene::model_instance* get_model_instance(entity::id entity_id);
	scene::light* get_light(entity::id entity_id);

private:	
	void update_model_and_materials(entity::id entity_id, ::model_component& model);
	void update_light(entity::id entity_id, ::light_component& component);
	
	void on_model_construct(entity::registry& registry, entity::id entity_id);
	void on_model_update(entity::registry& registry, entity::id entity_id);
	void on_model_destroy(entity::registry& registry, entity::id entity_id);
	void on_light_construct(entity::registry& registry, entity::id entity_id);
	void on_light_update(entity::registry& registry, entity::id entity_id);
	void on_light_destroy(entity::registry& registry, entity::id entity_id);
	
	float t;
	float dt;
	::render::renderer* renderer;
	std::vector<scene::collection*> layers;
	std::unordered_map<entity::id, std::unique_ptr<scene::model_instance>> model_instances;
	std::unordered_map<entity::id, std::unique_ptr<scene::light>> lights;
};


#endif // ANTKEEPER_GAME_RENDER_SYSTEM_HPP

