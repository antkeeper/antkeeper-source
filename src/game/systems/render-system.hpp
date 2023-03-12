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
#include <engine/scene/static-mesh.hpp>
#include <engine/scene/light.hpp>
#include "game/components/scene-component.hpp"
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

private:
	void on_scene_construct(entity::registry& registry, entity::id entity_id);
	void on_scene_update(entity::registry& registry, entity::id entity_id);
	void on_scene_destroy(entity::registry& registry, entity::id entity_id);
	
	void on_transform_construct(entity::registry& registry, entity::id entity_id);
	
	entt::observer updated_scene_transforms;
	
	float t;
	float dt;
	::render::renderer* renderer;
	std::vector<scene::collection*> layers;
};


#endif // ANTKEEPER_GAME_RENDER_SYSTEM_HPP

