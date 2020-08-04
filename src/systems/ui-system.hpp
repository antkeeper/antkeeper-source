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

#ifndef ANTKEEPER_UI_SYSTEM_HPP
#define ANTKEEPER_UI_SYSTEM_HPP

#include "event/event-handler.hpp"
#include "input/input-events.hpp"
#include "scene/scene.hpp"
#include "scene/camera.hpp"
#include "scene/directional-light.hpp"
#include "scene/ambient-light.hpp"
#include "scene/model-instance.hpp"
#include "scene/billboard.hpp"
#include "renderer/material.hpp"
#include <vmq/vmq.hpp>

using namespace vmq::types;

class control;
class scene;
class resource_manager;

class ui_system:
	public event_handler<mouse_moved_event>
{
public:
	ui_system(::resource_manager* resource_manager);

	void update(float dt);

	void set_viewport(const float4& viewport);
	void set_tool_menu_control(control* control);
	
	const ::scene* get_scene() const;
	::scene* get_scene();
	const ::camera* get_camera() const;
	::camera* get_camera();

private:
	virtual void handle_event(const mouse_moved_event& event);
	
	void open_tool_menu();
	void close_tool_menu();
	
	void open_elevator_menu();
	void close_elevator_menu();

	::resource_manager* resource_manager;
	::scene scene;
	::camera camera;
	ambient_light indirect_light;
	directional_light direct_light;
	model_instance tool_selector_ant;
	billboard tool_selector_bg;
	material modal_bg_material;
	billboard modal_bg;
	
	billboard underground_bg;
	
	model_instance energy_symbol;
	
	float2 mouse_position;
	float4 viewport;
	float2 viewport_center;
	float2 tool_selection_vector;
	control* tool_menu_control;
};

inline const scene* ui_system::get_scene() const
{
	return &scene;
}

inline scene* ui_system::get_scene()
{
	return &scene;
}

inline const camera* ui_system::get_camera() const
{
	return &camera;
}

inline camera* ui_system::get_camera()
{
	return &camera;
}

#endif // ANTKEEPER_UI_SYSTEM_HPP

