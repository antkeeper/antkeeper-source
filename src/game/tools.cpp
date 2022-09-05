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

#include "game/tools.hpp"
#include "application.hpp"
#include "animation/animator.hpp"
#include "animation/animation.hpp"
#include "entity/components/tool.hpp"
#include "entity/components/celestial-body.hpp"
#include "utility/timestamp.hpp"
#include "render/material.hpp"
#include "game/graphics.hpp"

namespace game {

entity::id build_camera_tool(game::context& ctx)
{
	// Create camera tool entity
	entity::id tool_eid = ctx.entity_registry->create();
	
	// Create tool component
	entity::component::tool tool;
	
	// Setup tool activated callback
	tool.activated = [&ctx]()
	{
		if (!ctx.camera_flash_animation->is_stopped())
			return;
		
		game::graphics::save_screenshot(ctx);
		
		render::material_property<float4>* tint = static_cast<render::material_property<float4>*>(ctx.camera_flash_billboard->get_material()->get_property("tint"));
		tint->set_value({1.0f, 1.0f, 1.0f, 1.0f});
		ctx.camera_flash_billboard->get_material()->update_tweens();
		ctx.ui_scene->add_object(ctx.camera_flash_billboard);
		
		ctx.camera_flash_animation->set_end_callback
		(
			[&ctx]()
			{
				ctx.ui_scene->remove_object(ctx.camera_flash_billboard);
			}
		);
		
		ctx.camera_flash_animation->set_frame_callback
		(
			[&ctx, tint](int channel, const float& opacity)
			{
				
				tint->set_value({1.0f, 1.0f, 1.0f, opacity});
			}
		);
		
		ctx.animator->remove_animation(ctx.camera_flash_animation);
		ctx.animator->add_animation(ctx.camera_flash_animation);
		ctx.camera_flash_animation->rewind();
		ctx.camera_flash_animation->play();
	};
	
	// Add tool component to camera tool entity
	ctx.entity_registry->assign<entity::component::tool>(tool_eid, tool);
	
	return tool_eid;
}

entity::id build_time_tool(game::context& ctx)
{
	// Create time tool entity
	entity::id tool_eid = ctx.entity_registry->create();
	
	// Create tool component
	entity::component::tool tool;
	
	// Setup tool active calback
	tool.active = [&ctx]()
	{
		auto [mouse_x, mouse_y] = ctx.app->get_mouse()->get_current_position();
		auto [window_w, window_h] = ctx.app->get_viewport_dimensions();
		
		entity::id planet_eid = ctx.entities["planet"];
		entity::component::celestial_body body = ctx.entity_registry->get<entity::component::celestial_body>(planet_eid);
		//body.axial_rotation = math::radians(360.0f) * ((float)mouse_x / (float)window_w);
		ctx.entity_registry->replace<entity::component::celestial_body>(planet_eid, body);
	};
	
	// Add tool component to time tool entity
	ctx.entity_registry->assign<entity::component::tool>(tool_eid, tool);
	
	return tool_eid;
}

} // namespace game
