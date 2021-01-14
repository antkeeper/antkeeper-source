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

#ifndef ANTKEEPER_ECS_PAINTING_SYSTEM_HPP
#define ANTKEEPER_ECS_PAINTING_SYSTEM_HPP

#include "entity-system.hpp"
#include "ecs/entity.hpp"
#include "event/event-handler.hpp"
#include "game/events/tool-events.hpp"
#include "utility/fundamental-types.hpp"
#include "scene/collection.hpp"
#include "scene/model-instance.hpp"
#include <vector>
#include <optional>

class material;
class event_dispatcher;
class resource_manager;
class model;
class model_group;
class vertex_buffer;

namespace ecs {

class painting_system: public entity_system,
	public event_handler<tool_pressed_event>,
	public event_handler<tool_released_event>
{
public:
	painting_system(ecs::registry& registry, event_dispatcher* event_dispatcher, resource_manager* resource_manager);
	virtual ~painting_system();
	virtual void update(double t, double dt);
	
	void set_scene(scene::collection* collection);
	
private:
	virtual void handle_event(const tool_pressed_event& event);
	virtual void handle_event(const tool_released_event& event);
	
	std::optional<std::tuple<float3, float3>> cast_ray(const float3& position) const;
	
	event_dispatcher* event_dispatcher;
	resource_manager* resource_manager;
	scene::collection* scene_collection;
	
	bool painting;
	entity brush_entity;
	float3 stroke_start;
	float3 stroke_end;
	float min_stroke_length;
	float min_stroke_length_squared;
	float stroke_width;
	int max_stroke_segments;
	int current_stroke_segment;
	float max_miter_angle;
	float decal_offset;
	float3 stroke_bounds_min;
	float3 stroke_bounds_max;
	float3 p0;
	float3 p0a;
	float3 p0b;
	
	std::size_t vertex_size;
	std::size_t vertex_stride;
	std::size_t vertex_count;
	
	model* stroke_model;
	model_group* stroke_model_group;
	vertex_buffer* stroke_vbo;
	bool midstroke;
	
	scene::model_instance* stroke_model_instance;
};

} // namespace ecs

#endif // ANTKEEPER_ECS_PAINTING_SYSTEM_HPP
