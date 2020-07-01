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

#include "ui-system.hpp"
#include "input/control.hpp"
#include "resources/resource-manager.hpp"

using namespace vmq::operators;

ui_system::ui_system(::resource_manager* resource_manager):
	resource_manager(resource_manager),
	tool_menu_control(nullptr)
{
	// Setup camera
	camera.look_at({0.0f, 0.0f, 500.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
	
	// Setup lighting
	indirect_light.set_intensity(0.25f);
	indirect_light.update_tweens();
	direct_light.look_at({-0.1f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f});
	direct_light.set_intensity(1.0f);
	direct_light.update_tweens();
	
	// Setup modal background
	modal_bg_material.set_shader_program(resource_manager->load<shader_program>("ui-element-untextured.glsl"));
	modal_bg_material.set_flags(1);
	modal_bg_material.add_property<float4>("tint")->set_value({0, 0, 0, 0.25f});
	modal_bg.set_material(&modal_bg_material);
	modal_bg.set_translation({0, 0, -10.0f});
	
	// Setup tool selector background
	tool_selector_bg.set_material(resource_manager->load<material>("tool-selector.mtl"));
	tool_selector_bg.set_translation({0, 0, -4.0f});
	tool_selector_bg.set_scale({270, 270, 270});
	
	// Setup tool selector ant
	tool_selector_ant.set_model(resource_manager->load<model>("worker-ant.obj"));
	tool_selector_ant.set_scale({350, 350, 350});
	tool_selector_ant.set_rotation(vmq::angle_axis(vmq::radians(180.0f), {0, 0, 1}) * vmq::angle_axis(vmq::radians(90.0f), {1, 0, 0}));
	tool_selector_ant.update_tweens();
	
	// Setup energy symbol
	energy_symbol.set_model(resource_manager->load<model>("energy.obj"));
	energy_symbol.set_scale({30, 30, 30});
	energy_symbol.update_tweens();
	energy_symbol.set_active(false);
	
	// Setup scene
	scene.add_object(&camera);
	scene.add_object(&indirect_light);
	scene.add_object(&direct_light);
	scene.add_object(&energy_symbol);
}

void ui_system::update(float dt)
{}

void ui_system::set_viewport(const float4& viewport)
{
	this->viewport = viewport;
	
	// Calculate viewport center
	viewport_center[0] = (viewport[2] - viewport[0]) * 0.5f;
	viewport_center[1] = (viewport[3] - viewport[1]) * 0.5f;
	
	// Recalculate orthographic projection
	float clip_left = -viewport[2] * 0.5f;
	float clip_right = viewport[2] * 0.5f;
	float clip_top = -viewport[3] * 0.5f;
	float clip_bottom = viewport[3] * 0.5f;
	float clip_near = 0.0f;
	float clip_far = 1000.0f;
	camera.set_orthographic(clip_left, clip_right, clip_top, clip_bottom, clip_near, clip_far);
	
	energy_symbol.set_translation({viewport[2] * 0.25f, 0.0f, 0.0f});
	energy_symbol.update_tweens();
	
	// Resize modal BG
	modal_bg.set_scale({viewport[2] * 0.5f, viewport[3] * 0.5f, 1.0f});
	modal_bg.update_tweens();
}

void ui_system::set_tool_menu_control(control* control)
{
	tool_menu_control = control;
	tool_menu_control->set_activated_callback(std::bind(&ui_system::open_tool_menu, this));
	tool_menu_control->set_deactivated_callback(std::bind(&ui_system::close_tool_menu, this));
}

void ui_system::handle_event(const mouse_moved_event& event)
{
	if (tool_menu_control->is_active())
	{
		tool_selection_vector.x += event.dx;
		tool_selection_vector.y += event.dy;
		
		float max_length = 200.0f;
		float selection_threshold = 20.0f;
		int sector_count = 6;
		float sector_angle = vmq::two_pi<float> / static_cast<float>(sector_count);
		
		float length_squared = vmq::length_squared(tool_selection_vector);
		
		// Select tool if length of selection vector within threshold
		if (length_squared >= selection_threshold * selection_threshold)
		{
			// Limit length of tool selection vector
			if (length_squared > max_length * max_length)
			{
				tool_selection_vector = (tool_selection_vector / std::sqrt(length_squared)) * max_length;
			}
			
			float2 selection_direction = tool_selection_vector / std::sqrt(length_squared);
			float selection_angle = std::atan2(-selection_direction.y, selection_direction.x) - vmq::radians(90.0f);
			selection_angle = (selection_angle >= 0.0f ? selection_angle : (vmq::two_pi<float> + selection_angle));
			
			int sector = static_cast<int>((selection_angle + sector_angle * 0.5f) / sector_angle) % sector_count;
						
			float rotation_angle = static_cast<float>(sector) * sector_angle;
			tool_selector_bg.set_rotation(vmq::angle_axis(rotation_angle, {0, 0, 1}));
			tool_selector_bg.update_tweens();	

			tool_selector_ant.set_rotation(vmq::angle_axis(rotation_angle + vmq::radians(180.0f), {0, 0, 1}) * vmq::angle_axis(vmq::radians(90.0f), {1, 0, 0}));
		}
	}
	
	mouse_position[0] = event.x;
	mouse_position[1] = event.y;
}

void ui_system::open_tool_menu()
{
	scene.add_object(&modal_bg);
	scene.add_object(&tool_selector_bg);
	scene.add_object(&tool_selector_ant);
	tool_selection_vector = {0, 0};
}

void ui_system::close_tool_menu()
{
	scene.remove_object(&modal_bg);
	scene.remove_object(&tool_selector_bg);
	scene.remove_object(&tool_selector_ant);
}
