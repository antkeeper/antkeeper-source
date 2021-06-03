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

#include "ui.hpp"
#include "resources/resource-manager.hpp"

namespace entity {
namespace system {

ui::ui(::resource_manager* resource_manager):
	resource_manager(resource_manager),
	tool_menu_control(nullptr),
	camera(nullptr),
	scene_collection(nullptr)
{
	// Setup lighting
	indirect_light.set_intensity(0.25f);
	indirect_light.update_tweens();
	direct_light.look_at({-0.1f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f});
	direct_light.set_intensity(1.0f);
	direct_light.update_tweens();
	
	// Setup modal background
	modal_bg_material.set_shader_program(resource_manager->load<gl::shader_program>("ui-element-untextured.glsl"));
	modal_bg_material.set_flags(1);
	modal_bg_material.add_property<float4>("tint")->set_value({0, 0, 0, 0.25f});
	modal_bg.set_material(&modal_bg_material);
	modal_bg.set_translation({0, 0, -10.0f});
	
	// Setup tool selector background
	tool_selector_bg.set_material(resource_manager->load<material>("tool-selector.mtl"));
	tool_selector_bg.set_translation({0, 0, -4.0f});
	tool_selector_bg.set_scale({270, 270, 270});
	
	// Setup scene
	//scene.add_object(&camera);
	//scene.add_object(&indirect_light);
	//scene.add_object(&direct_light);
}

void ui::update(float dt)
{}

void ui::set_viewport(const float4& viewport)
{
	this->viewport = viewport;
	
	// Calculate viewport center
	viewport_center[0] = (viewport[2] - viewport[0]) * 0.5f;
	viewport_center[1] = (viewport[3] - viewport[1]) * 0.5f;
	
	
	// Resize modal BG
	modal_bg.set_scale({viewport[2] * 0.5f, viewport[3] * 0.5f, 1.0f});
	modal_bg.update_tweens();
	
	update_projection();
}

void ui::set_tool_menu_control(input::control* control)
{
	tool_menu_control = control;
	tool_menu_control->set_activated_callback(std::bind(&ui::open_tool_menu, this));
	tool_menu_control->set_deactivated_callback(std::bind(&ui::close_tool_menu, this));
}

void ui::set_camera(scene::camera* camera)
{
	this->camera = camera;
	
	if (camera)
	{
		camera->look_at({0.0f, 0.0f, 500.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
		update_projection();
	}
}

void ui::set_scene(scene::collection* collection)
{
	this->scene_collection = collection;
}

void ui::handle_event(const mouse_moved_event& event)
{
	if (tool_menu_control->is_active())
	{
		tool_selection_vector.x += event.dx;
		tool_selection_vector.y += event.dy;
		
		float max_length = 200.0f;
		float selection_threshold = 20.0f;
		int sector_count = 6;
		float sector_angle = math::two_pi<float> / static_cast<float>(sector_count);
		
		float length_squared = math::length_squared(tool_selection_vector);
		
		// Select tool if length of selection vector within threshold
		if (length_squared >= selection_threshold * selection_threshold)
		{
			// Limit length of tool selection vector
			if (length_squared > max_length * max_length)
			{
				tool_selection_vector = (tool_selection_vector / std::sqrt(length_squared)) * max_length;
			}
			
			float2 selection_direction = tool_selection_vector / std::sqrt(length_squared);
			float selection_angle = std::atan2(-selection_direction.y, selection_direction.x) - math::radians(90.0f);
			selection_angle = (selection_angle >= 0.0f ? selection_angle : (math::two_pi<float> + selection_angle));
			
			int sector = static_cast<int>((selection_angle + sector_angle * 0.5f) / sector_angle) % sector_count;
				
			float rotation_angle = static_cast<float>(sector) * sector_angle;
			tool_selector_bg.set_rotation(math::angle_axis(rotation_angle, {0, 0, 1}));
			tool_selector_bg.update_tweens();	

		}
	}
	
	mouse_position[0] = event.x;
	mouse_position[1] = event.y;
}

void ui::handle_event(const window_resized_event& event)
{
	set_viewport({0.0f, 0.0f, static_cast<float>(event.w), static_cast<float>(event.h)});
}

void ui::update_projection()
{
	if (camera)
	{
		float clip_left = -viewport[2] * 0.5f;
		float clip_right = viewport[2] * 0.5f;
		float clip_top = -viewport[3] * 0.5f;
		float clip_bottom = viewport[3] * 0.5f;
		float clip_near = 0.0f;
		float clip_far = 1000.0f;
		camera->set_orthographic(clip_left, clip_right, clip_top, clip_bottom, clip_near, clip_far);
	}
}

void ui::open_tool_menu()
{
	if (scene_collection)
	{
		scene_collection->add_object(&modal_bg);
		scene_collection->add_object(&tool_selector_bg);
	}
	tool_selection_vector = {0, 0};
}

void ui::close_tool_menu()
{
	if (scene_collection)
	{
		scene_collection->remove_object(&modal_bg);
		scene_collection->remove_object(&tool_selector_bg);
	}
}

void ui::open_elevator_menu()
{
	
}

void ui::close_elevator_menu()
{
	
}

} // namespace system
} // namespace entity
