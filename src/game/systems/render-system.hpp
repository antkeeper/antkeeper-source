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

#ifndef ANTKEEPER_RENDER_SYSTEM_HPP
#define ANTKEEPER_RENDER_SYSTEM_HPP

#include "entity-system.hpp"
#include "scene/scene.hpp"
#include "scene/model-instance.hpp"
#include "game/components/model-component.hpp"
#include <unordered_map>
#include <vector>

class renderer;

class render_system: public entity_system
{
public:
	render_system(entt::registry& registry);
	virtual void update(double t, double dt);
	
	void render(double alpha);
	
	
	void add_layer(::scene* layer);
	void remove_layers();
	
	void set_renderer(::renderer* renderer);

private:	
	void update_model_and_materials(entt::entity entity, ecs::model_component& model);

	void on_model_construct(entt::registry& registry, entt::entity entity, ecs::model_component& model);
	void on_model_replace(entt::registry& registry, entt::entity entity, ecs::model_component& model);
	void on_model_destroy(entt::registry& registry, entt::entity entity);

	renderer* renderer;
	std::vector<scene*> layers;
	std::unordered_map<entt::entity, model_instance*> model_instances;
};

#endif // ANTKEEPER_RENDER_SYSTEM_HPP

