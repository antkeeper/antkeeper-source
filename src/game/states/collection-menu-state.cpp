// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/states/collection-menu-state.hpp"
#include "game/states/main-menu-state.hpp"
#include "game/controls.hpp"
#include <engine/scene/text.hpp>
#include <engine/debug/log.hpp>
#include "game/menu.hpp"
#include "game/strings.hpp"
#include <engine/hash/fnv1a.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/animation/screen-transition.hpp>
#include <engine/animation/ease.hpp>
#include <engine/config.hpp>

using namespace hash::literals;

collection_menu_state::collection_menu_state(::game& ctx):
	game_state(ctx)
{
	debug::log_trace("Entering collection menu state...");
	
	// Construct box material
	box_material = std::make_shared<render::material>();
	box_material->set_blend_mode(render::material_blend_mode::translucent);
	box_material->set_shader_template(ctx.resource_manager->load<gl::shader_template>("ui-element-untextured.glsl"));
	box_material->set_variable("tint", std::make_shared<render::matvar_fvec4>(1, math::fvec4{0.5f, 0.5f, 0.5f, 1}));
	
	// Construct box billboard
	box_billboard.set_material(box_material);
	
	// Construct selection material
	selection_material = std::make_shared<render::material>();
	selection_material->set_blend_mode(render::material_blend_mode::translucent);
	selection_material->set_shader_template(ctx.resource_manager->load<gl::shader_template>("ui-element-untextured.glsl"));
	box_material->set_variable("tint", std::make_shared<render::matvar_fvec4>(1, math::fvec4{1, 1, 1, 1}));
	
	// Construct selection billboard
	selection_billboard.set_material(selection_material);
	
	// Add box and selection billboard to UI scene
	ctx.ui_scene->add_object(box_billboard);
	ctx.ui_scene->add_object(selection_billboard);
	
	row_count = 64;
	column_count = 6;
	selected_row = 0;
	selected_column = 0;
	resize_box();
	
	mouse_moved_subscription = ctx.input_manager->get_event_dispatcher().subscribe<input::mouse_moved_event>
	(
		[&](const auto& event)
		{

		}
	);
	
	mouse_button_pressed_subscription = ctx.input_manager->get_event_dispatcher().subscribe<input::mouse_button_pressed_event>
	(
		[&](const auto& event)
		{
			const auto& viewport_size = ctx.window->get_viewport_size();
			const math::fvec2 mouse_position =
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
					
					debug::log_debug("selected colony: ({}, {})", selected_column, selected_row);
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
	//ctx.function_queue.push(std::bind(::enable_menu_controls, std::ref(ctx)));
	
	// Fade in from black
	ctx.fade_transition->transition(config::title_fade_in_duration, true, ease<float>::out_cubic);
	
	debug::log_trace("Entered collection menu state");
}

collection_menu_state::~collection_menu_state()
{
	debug::log_trace("Exiting collection menu state...");
	
	// Destruct menu
	//::disable_menu_controls(ctx);
	
	debug::log_trace("Exited collection menu state");
}

void collection_menu_state::resize_box()
{
	const float padding = 64.0f;
	const auto viewport_size = math::fvec2(ctx.window->get_viewport_size());
	
	box_bounds.min.x() = viewport_size.x() * 0.5f + padding;
	box_bounds.max.x() = viewport_size.x() - padding;
	
	selection_size = (box_bounds.max.x() - box_bounds.min.x()) / static_cast<float>(column_count);
	
	box_bounds.max.y() = viewport_size.y() - padding;
	box_bounds.min.y() = std::max<float>(padding, box_bounds.max.y() - selection_size * row_count);
	
	const math::fvec2 box_size = box_bounds.size();
	const math::fvec2 box_center = box_bounds.center();
	
	// Resize box
	box_billboard.set_scale({box_size.x() * 0.5f, box_size.y() * 0.5f, 1.0f});
	box_billboard.set_translation({box_center.x(), box_center.y(), -1.0f});
	
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
}
