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

#ifndef ANTKEEPER_ENTITY_SYSTEM_TRACKING_HPP
#define ANTKEEPER_ENTITY_SYSTEM_TRACKING_HPP

#include "entity/systems/updatable.hpp"
#include "entity/components/trackable.hpp"
#include "entity/id.hpp"
#include "event/event-handler.hpp"
#include "game/events/tool-events.hpp"
#include <unordered_map>
#include "scene/collection.hpp"
#include "scene/model-instance.hpp"

class material;
class event_dispatcher;
class resource_manager;
class model;

namespace entity {
namespace system {

class tracking: public updatable,
	public event_handler<tool_pressed_event>,
	public event_handler<tool_released_event>
{
public:
	tracking(entity::registry& registry, event_dispatcher* event_dispatcher, resource_manager* resource_manager);
	virtual ~tracking();
	virtual void update(double t, double dt);
	
	void set_scene(scene::collection* collection);
	void set_viewport(const float4& viewport);
	
private:
	void on_component_construct(entity::registry& registry, entity::id entity_id, entity::component::trackable& component);
	void on_component_destroy(entity::registry& registry, entity::id entity_id);
	virtual void handle_event(const tool_pressed_event& event);
	virtual void handle_event(const tool_released_event& event);
	
	event_dispatcher* event_dispatcher;
	resource_manager* resource_manager;
	scene::collection* scene_collection;
	model* tracker_model;
	model* paint_ball_model;
	material** paint_ball_materials;
	std::unordered_map<entity::id, scene::model_instance*> trackers;
};

} // namespace system
} // namespace entity

#endif // ANTKEEPER_ENTITY_SYSTEM_TRACKING_HPP
