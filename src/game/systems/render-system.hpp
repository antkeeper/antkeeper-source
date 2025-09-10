// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_RENDER_SYSTEM_HPP
#define ANTKEEPER_GAME_RENDER_SYSTEM_HPP

#include <entt/entt.hpp>
#include "game/systems/fixed-update-system.hpp"
#include "game/systems/variable-update-system.hpp"
#include <engine/render/renderer.hpp>
#include <engine/entity/id.hpp>
#include <engine/scene/collection.hpp>
#include <vector>

using namespace engine;

class render_system:
	public fixed_update_system,
	public variable_update_system
{
public:
	explicit render_system(entity::registry& registry);
	~render_system() override;
	
	void fixed_update(entity::registry& registry, float t, float dt) override;
	void variable_update(entity::registry& registry, float t, float dt, float alpha) override;
	
	void add_layer(scene::collection* layer);
	void remove_layers();
	
	void set_renderer(::render::renderer* renderer);

private:
	/// Called when a component is constructed.
	/// @tparam T Component type.
	template <class T>
	void on_construct(entity::registry& registry, entity::id entity_id);

	/// Called when a component is updated.
	/// @tparam T Component type.
	template <class T>
	void on_update(entity::registry& registry, entity::id entity_id);

	/// Called when a component is destroyed.
	/// @tparam T Component type.
	template <class T>
	void on_destroy(entity::registry& registry, entity::id entity_id);
	
	entity::registry& m_registry;
	entt::observer m_transformed_scene_object_components;
	::render::renderer* m_renderer;
	std::vector<scene::collection*> m_layers;
};

#endif // ANTKEEPER_GAME_RENDER_SYSTEM_HPP
