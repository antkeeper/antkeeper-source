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

#include "game/state/collection-menu.hpp"
#include "game/state/main-menu.hpp"
#include "game/controls.hpp"
#include "scene/text.hpp"
#include "debug/log.hpp"
#include "game/menu.hpp"
#include "game/strings.hpp"
#include "utility/hash/fnv1a.hpp"
#include "resources/resource-manager.hpp"
#include "animation/screen-transition.hpp"
#include "animation/ease.hpp"
#include "render/passes/clear-pass.hpp"

using namespace hash::literals;

namespace game {
namespace state {

collection_menu::collection_menu(game::context& ctx):
	game::state::base(ctx)
{
	debug::log::trace("Entering collection menu state...");
	
	// Enable color buffer clearing in UI pass
	ctx.ui_clear_pass->set_cleared_buffers(true, true, false);
	
	// Construct box material
	box_material.set_blend_mode(render::blend_mode::translucent);
	box_material.set_shader_program(ctx.resource_manager->load<gl::shader_program>("ui-element-untextured.glsl"));
	box_material.add_property<float4>("tint")->set_value(float4{0.5f, 0.5f, 0.5f, 1});
	box_material.update_tweens();
	
	// Construct box billboard
	box_billboard.set_material(&box_material);
	
	// Construct selection material
	selection_material.set_blend_mode(render::blend_mode::translucent);
	selection_material.set_shader_program(ctx.resource_manager->load<gl::shader_program>("ui-element-untextured.glsl"));
	selection_material.add_property<float4>("tint")->set_value(float4{1, 1, 1, 1});
	selection_material.update_tweens();
	
	// Construct selection billboard
	selection_billboard.set_material(&selection_material);
	
	// Add box and selection billboard to UI scene
	ctx.ui_scene->add_object(&box_billboard);
	ctx.ui_scene->add_object(&selection_billboard);
	
	row_count = 64;
	column_count = 6;
	selected_row = 0;
	selected_column = 0;
	resize_box();
	
	mouse_moved_subscription = ctx.input_manager->get_event_queue().subscribe<input::mouse_moved_event>
	(
		[&](const auto& event)
		{

		}
	);
	
	mouse_button_pressed_subscription = ctx.input_manager->get_event_queue().subscribe<input::mouse_button_pressed_event>
	(
		[&](const auto& event)
		{
			const auto& viewport_size = ctx.window->get_viewport_size();
			const float2 mouse_position =
			{
				static_cast<float>(event.position.x()),
				static_cast<float>(viewport_size.y() - event.position.y() + 1)
			};
			
			if (box_bounds.contains(mouse_position))
			{
				int column = static_cast<int>((mouse_position.x() - box_bounds.min.x()) / selection_size);
				int row = static_cast<int>((box_bounds.max.y() - mouse_position.y()) / selection_size);
				
				if (column != selected_column || row != selected_row)
				{
					selected_column = column;
					selected_row = row;
						
					selection_billboard.set_translation
					(
						{
							(box_bounds.min.x() + selection_size * 0.5f) + selection_size * selected_column,
							(box_bounds.max.y() - selection_size * 0.5f) - selection_size * selected_row ,
							0.0f
						}
					);
					selection_billboard.update_tweens();
					
					debug::log::debug("selected colony: ({}, {})", selected_column, selected_row);
				}
			}
		}
	);
	
	window_resized_subscription = ctx.window->get_resized_channel().subscribe
	(
		[&](const auto& event)
		{
			this->resize_box();
		}
	);
	
	// Queue enable menu controls
	//ctx.function_queue.push(std::bind(game::enable_menu_controls, std::ref(ctx)));
	
	// Fade in from black
	ctx.fade_transition->transition(config::title_fade_in_duration, true, ease<float>::out_cubic);
	
	debug::log::trace("Entered collection menu state");
}

collection_menu::~collection_menu()
{
	debug::log::trace("Exiting collection menu state...");
	
	// Destruct menu
	//game::disable_menu_controls(ctx);
	
	debug::log::trace("Exited collection menu state");
}

void collection_menu::resize_box()
{
	const float padding = 64.0f;
	const auto viewport_size = float2(ctx.window->get_viewport_size());
	
	box_bounds.min.x() = viewport_size.x() * 0.5f + padding;
	box_bounds.max.x() = viewport_size.x() - padding;
	
	selection_size = (box_bounds.max.x() - box_bounds.min.x()) / static_cast<float>(column_count);
	
	box_bounds.max.y() = viewport_size.y() - padding;
	box_bounds.min.y() = std::max<float>(padding, box_bounds.max.y() - selection_size * row_count);
	
	const float2 box_size = box_bounds.size();
	const float2 box_center = box_bounds.center();
	
	// Resize box
	box_billboard.set_scale({box_size.x() * 0.5f, box_size.y() * 0.5f, 1.0f});
	box_billboard.set_translation({box_center.x(), box_center.y(), -1.0f});
	box_billboard.update_tweens();
	
	// Resize selection
	selection_billboard.set_scale({selection_size * 0.5f, selection_size * 0.5f, 1.0f});
	selection_billboard.set_translation
	(
		{
			(box_bounds.min.x() + selection_size * 0.5f) + selection_size * selected_column,
			(box_bounds.max.y() - selection_size * 0.5f) - selection_size * selected_row,
			0.0f
		}
	);
	selection_billboard.update_tweens();
}

} // namespace state
} // namespace game
