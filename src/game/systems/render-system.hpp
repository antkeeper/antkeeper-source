// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
	~render_system() override;
	
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
	
	entt::observer m_updated_scene_transforms;
	
	float m_t;
	float m_dt;
	::render::renderer* m_renderer;
	std::vector<scene::collection*> m_layers;
};


#endif // ANTKEEPER_GAME_RENDER_SYSTEM_HPP

