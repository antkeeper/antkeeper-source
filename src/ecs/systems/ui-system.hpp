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

#ifndef ANTKEEPER_ECS_UI_SYSTEM_HPP
#define ANTKEEPER_ECS_UI_SYSTEM_HPP

#include "event/event-handler.hpp"
#include "event/input-events.hpp"
#include "event/window-events.hpp"
#include "scene/collection.hpp"
#include "scene/camera.hpp"
#include "scene/directional-light.hpp"
#include "scene/ambient-light.hpp"
#include "scene/model-instance.hpp"
#include "scene/billboard.hpp"
#include "renderer/material.hpp"
#include "math/math.hpp"
#include "input/control.hpp"

class resource_manager;

namespace ecs {

class ui_system:
	public event_handler<mouse_moved_event>,
	public event_handler<window_resized_event>
{
public:
	ui_system(::resource_manager* resource_manager);

	void update(float dt);

	void set_viewport(const float4& viewport);
	void set_tool_menu_control(input::control* control);
	
	void set_camera(scene::camera* camera);
	void set_scene(scene::collection* collection);

private:
	virtual void handle_event(const mouse_moved_event& event);
	virtual void handle_event(const window_resized_event& event);
	
	void update_projection();
	
	void open_tool_menu();
	void close_tool_menu();
	
	void open_elevator_menu();
	void close_elevator_menu();

	resource_manager* resource_manager;
	scene::collection* scene_collection;
	scene::camera* camera;
	scene::ambient_light indirect_light;
	scene::directional_light direct_light;
	scene::billboard tool_selector_bg;
	material modal_bg_material;
	scene::billboard modal_bg;
	
	scene::billboard underground_bg;
	
	scene::model_instance energy_symbol;
	
	float2 mouse_position;
	float4 viewport;
	float2 viewport_center;
	float2 tool_selection_vector;
	input::control* tool_menu_control;
};

} // namespace ecs

#endif // ANTKEEPER_ECS_UI_SYSTEM_HPP
