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

#include "game/menu.hpp"
#include "scene/text.hpp"
#include "application.hpp"
#include "animation/animation.hpp"
#include "animation/animator.hpp"
#include "animation/ease.hpp"
#include "config.hpp"
#include <algorithm>

namespace game {
namespace menu {

void init_menu_item_index(game::context& ctx, const std::string& menu_name)
{
	if (auto it = ctx.menu_item_indices.find(menu_name); it != ctx.menu_item_indices.end())
	{
		ctx.menu_item_index = &it->second;
	}
	else
	{
		ctx.menu_item_index = &ctx.menu_item_indices[menu_name];
		*ctx.menu_item_index = 0;
	}
}

void update_text_font(game::context& ctx)
{
	for (auto [name, value]: ctx.menu_item_texts)
	{
		name->set_material(&ctx.menu_font_material);
		name->set_font(&ctx.menu_font);
		
		if (value)
		{
			value->set_material(&ctx.menu_font_material);
			value->set_font(&ctx.menu_font);
		}
	}
}

void update_text_color(game::context& ctx)
{
	for (std::size_t i = 0; i < ctx.menu_item_texts.size(); ++i)
	{
		auto [name, value] = ctx.menu_item_texts[i];
		
		const float4& color = (i == *ctx.menu_item_index) ? config::menu_active_color : config::menu_inactive_color;
		
		name->set_color(color);
		if (value)
			value->set_color(color);
	}
}

void update_text_tweens(game::context& ctx)
{
	for (auto [name, value]: ctx.menu_item_texts)
	{
		name->update_tweens();
		if (value)
			value->update_tweens();
	}
}

void align_text(game::context& ctx, bool center, bool has_back, float anchor_y)
{
	// Calculate menu width
	float menu_width = 0.0f;
	float menu_spacing = ctx.menu_font.get_glyph_metrics(U'M').width;
	
	for (auto [name, value]: ctx.menu_item_texts)
	{
		float row_width = 0.0f;
		
		// Add name width to width
		const auto& name_bounds = static_cast<const geom::aabb<float>&>(name->get_local_bounds());
		row_width += name_bounds.max_point.x - name_bounds.min_point.x;
		
		if (value)
		{
			// Add value width to width
			//const auto& value_bounds = static_cast<const geom::aabb<float>&>(value->get_local_bounds());
			//row_width += value_bounds.max_point.x - value_bounds.min_point.x;
			
			// Add spacing to row width
			row_width += menu_spacing * 8.0f;
		}
		
		menu_width = std::max<float>(menu_width, row_width);
	}
	
	// Align texts
	float menu_height;
	if (has_back)
		menu_height = (ctx.menu_item_texts.size() - 1) * ctx.menu_font.get_font_metrics().linespace - ctx.menu_font.get_font_metrics().linegap;
	else
		menu_height = ctx.menu_item_texts.size() * ctx.menu_font.get_font_metrics().linespace - ctx.menu_font.get_font_metrics().linegap;
	
	float menu_x = -menu_width * 0.5f;
	float menu_y = anchor_y + menu_height * 0.5f - ctx.menu_font.get_font_metrics().size;
	
	for (std::size_t i = 0; i < ctx.menu_item_texts.size(); ++i)
	{
		auto [name, value] = ctx.menu_item_texts[i];
		
		float x = menu_x;
		float y = menu_y - ctx.menu_font.get_font_metrics().linespace * i;
		if (has_back && i == ctx.menu_item_texts.size() - 1)
			y -= ctx.menu_font.get_font_metrics().linespace;
		
		if (center || i == ctx.menu_item_texts.size() - 1)
		{
			const auto& name_bounds = static_cast<const geom::aabb<float>&>(name->get_local_bounds());
			const float name_width =  name_bounds.max_point.x - name_bounds.min_point.x;
			x = -name_width * 0.5f;
		}
		
		name->set_translation({std::round(x), std::round(y), 0.0f});
		
		if (value)
		{
			const auto& value_bounds = static_cast<const geom::aabb<float>&>(value->get_local_bounds());
			const float value_width =  value_bounds.max_point.x - value_bounds.min_point.x;
			
			if (center || i == ctx.menu_item_texts.size() - 1)
				x = -value_width * 0.5f;
			else
				x = menu_x + menu_width - value_width;
			
			value->set_translation({std::round(x), std::round(y), 0.0f});
		}
	}
}

void refresh_text(game::context& ctx)
{
	for (auto [name, value]: ctx.menu_item_texts)
	{
		name->refresh();
		if (value)
			value->refresh();
	}
}

void add_text_to_ui(game::context& ctx)
{
	for (auto [name, value]: ctx.menu_item_texts)
	{
		ctx.ui_scene->add_object(name);
		if (value)
			ctx.ui_scene->add_object(value);
	}
}

void remove_text_from_ui(game::context& ctx)
{
	for (auto [name, value]: ctx.menu_item_texts)
	{
		ctx.ui_scene->remove_object(name);
		if (value)
			ctx.ui_scene->remove_object(value);
	}
}

void delete_text(game::context& ctx)
{
	for (auto [name, value]: ctx.menu_item_texts)
	{
		delete name;
		if (value)
			delete value;
	}
	ctx.menu_item_texts.clear();
}

void delete_animations(game::context& ctx)
{
	ctx.animator->remove_animation(ctx.menu_fade_animation);
	delete ctx.menu_fade_animation;
	ctx.menu_fade_animation = nullptr;
}

void clear_callbacks(game::context& ctx)
{
	// Clear menu item callbacks
	ctx.menu_left_callbacks.clear();
	ctx.menu_right_callbacks.clear();
	ctx.menu_select_callbacks.clear();
	ctx.menu_back_callback = nullptr;
}

void setup_animations(game::context& ctx)
{
	ctx.menu_fade_animation = new animation<float>();
	animation_channel<float>* opacity_channel = ctx.menu_fade_animation->add_channel(0);
	
	ctx.menu_fade_animation->set_frame_callback
	(
		[&ctx](int channel, const float& opacity)
		{
			for (std::size_t i = 0; i < ctx.menu_item_texts.size(); ++i)
			{
				auto [name, value] = ctx.menu_item_texts[i];
				
				float4 color = (i == *ctx.menu_item_index) ? config::menu_active_color : config::menu_inactive_color;
				color[3] = color[3] * opacity;
				
				if (name)
					name->set_color(color);
				if (value)
					value->set_color(color);
			}
		}
	);
	
	ctx.animator->add_animation(ctx.menu_fade_animation);
}

void fade_in(game::context& ctx, const std::function<void()>& end_callback)
{
	ctx.menu_fade_animation->set_interpolator(ease<float>::out_cubic);
	animation_channel<float>* opacity_channel = ctx.menu_fade_animation->get_channel(0);
	opacity_channel->remove_keyframes();
	opacity_channel->insert_keyframe({0.0, 0.0f});
	opacity_channel->insert_keyframe({config::menu_fade_in_duration, 1.0f});
	ctx.menu_fade_animation->set_end_callback(end_callback);
	
	for (std::size_t i = 0; i < ctx.menu_item_texts.size(); ++i)
	{
		auto [name, value] = ctx.menu_item_texts[i];
		
		float4 color = (i == *ctx.menu_item_index) ? config::menu_active_color : config::menu_inactive_color;
		color[3] = 0.0f;
		
		if (name)
		{
			name->set_color(color);
			name->update_tweens();
		}
		if (value)
		{
			value->set_color(color);
			value->update_tweens();
		}
	}
	
	ctx.menu_fade_animation->stop();
	ctx.menu_fade_animation->play();
}

void fade_out(game::context& ctx, const std::function<void()>& end_callback)
{
	ctx.menu_fade_animation->set_interpolator(ease<float>::out_cubic);
	animation_channel<float>* opacity_channel = ctx.menu_fade_animation->get_channel(0);
	opacity_channel->remove_keyframes();
	opacity_channel->insert_keyframe({0.0, 1.0f});
	opacity_channel->insert_keyframe({config::menu_fade_out_duration, 0.0f});
	ctx.menu_fade_animation->set_end_callback(end_callback);
	
	ctx.menu_fade_animation->stop();
	ctx.menu_fade_animation->play();
}

void fade_in_bg(game::context& ctx)
{
	ctx.menu_bg_fade_out_animation->stop();
	ctx.menu_bg_fade_in_animation->stop();
	ctx.menu_bg_fade_in_animation->play();
}

void fade_out_bg(game::context& ctx)
{
	ctx.menu_bg_fade_in_animation->stop();
	ctx.menu_bg_fade_out_animation->stop();
	ctx.menu_bg_fade_out_animation->play();
}

void setup_controls(game::context& ctx)
{	
	ctx.controls["menu_up"]->set_activated_callback
	(
		[&ctx]()
		{
			--(*ctx.menu_item_index);
			if (*ctx.menu_item_index < 0)
				*ctx.menu_item_index = ctx.menu_item_texts.size() - 1;
			
			update_text_color(ctx);
		}
	);
	ctx.controls["menu_down"]->set_activated_callback
	(
		[&ctx]()
		{
			++(*ctx.menu_item_index);
			if (*ctx.menu_item_index >= ctx.menu_item_texts.size())
				*ctx.menu_item_index = 0;
			
			update_text_color(ctx);
		}
	);
	ctx.controls["menu_left"]->set_activated_callback
	(
		[&ctx]()
		{
			auto callback = ctx.menu_left_callbacks[*ctx.menu_item_index];
			if (callback != nullptr)
				callback();
		}
	);
	ctx.controls["menu_right"]->set_activated_callback
	(
		[&ctx]()
		{
			auto callback = ctx.menu_right_callbacks[*ctx.menu_item_index];
			if (callback != nullptr)
				callback();
		}
	);
	ctx.controls["menu_select"]->set_activated_callback
	(
		[&ctx]()
		{
			auto callback = ctx.menu_select_callbacks[*ctx.menu_item_index];
			if (callback != nullptr)
				callback();
		}
	);
	ctx.controls["menu_back"]->set_activated_callback
	(
		[&ctx]()
		{
			if (ctx.menu_back_callback != nullptr)
				ctx.menu_back_callback();
		}
	);
	
	ctx.menu_mouse_tracker->set_mouse_moved_callback
	(
		[&ctx](const mouse_moved_event& event)
		{
			const float padding = config::menu_mouseover_padding * ctx.menu_font.get_font_metrics().size;
			
			for (std::size_t i = 0; i < ctx.menu_item_texts.size(); ++i)
			{
				auto [name, value] = ctx.menu_item_texts[i];
				
				const auto& name_bounds = static_cast<const geom::aabb<float>&>(name->get_world_bounds());
				float min_x = name_bounds.min_point.x;
				float min_y = name_bounds.min_point.y;
				float max_x = name_bounds.max_point.x;
				float max_y = name_bounds.max_point.y;
				if (value)
				{
					const auto& value_bounds = static_cast<const geom::aabb<float>&>(value->get_world_bounds());
					min_x = std::min<float>(min_x, value_bounds.min_point.x);
					min_y = std::min<float>(min_y, value_bounds.min_point.y);
					max_x = std::max<float>(max_x, value_bounds.max_point.x);
					max_y = std::max<float>(max_y, value_bounds.max_point.y);
				}
				
				min_x -= padding;
				min_y -= padding;
				max_x += padding;
				max_y += padding;
				
				const auto& viewport = ctx.app->get_viewport_dimensions();
				const float x = static_cast<float>(event.x - viewport[0] / 2);
				const float y = static_cast<float>((viewport[1] - event.y + 1) - viewport[1] / 2);
				
				if (x >= min_x && x <= max_x)
				{
					if (y >= min_y && y <= max_y)
					{
						*ctx.menu_item_index = i;
						update_text_color(ctx);
						break;
					}
				}
			}
		}
	);
	
	ctx.menu_mouse_tracker->set_mouse_button_pressed_callback
	(
		[&ctx](const mouse_button_pressed_event& event)
		{
			const float padding = config::menu_mouseover_padding * ctx.menu_font.get_font_metrics().size;
			
			for (std::size_t i = 0; i < ctx.menu_item_texts.size(); ++i)
			{
				auto [name, value] = ctx.menu_item_texts[i];
				
				const auto& name_bounds = static_cast<const geom::aabb<float>&>(name->get_world_bounds());
				float min_x = name_bounds.min_point.x;
				float min_y = name_bounds.min_point.y;
				float max_x = name_bounds.max_point.x;
				float max_y = name_bounds.max_point.y;
				if (value)
				{
					const auto& value_bounds = static_cast<const geom::aabb<float>&>(value->get_world_bounds());
					min_x = std::min<float>(min_x, value_bounds.min_point.x);
					min_y = std::min<float>(min_y, value_bounds.min_point.y);
					max_x = std::max<float>(max_x, value_bounds.max_point.x);
					max_y = std::max<float>(max_y, value_bounds.max_point.y);
				}
				
				min_x -= padding;
				min_y -= padding;
				max_x += padding;
				max_y += padding;
				
				const auto& viewport = ctx.app->get_viewport_dimensions();
				const float x = static_cast<float>(event.x - viewport[0] / 2);
				const float y = static_cast<float>((viewport[1] - event.y + 1) - viewport[1] / 2);
				
				if (x >= min_x && x <= max_x)
				{
					if (y >= min_y && y <= max_y)
					{
						*ctx.menu_item_index = i;
						update_text_color(ctx);
						
						if (event.button == 1)
						{
							auto callback = ctx.menu_select_callbacks[i];
							if (callback)
								callback();
						}
						else if (event.button == 3)
						{
							auto callback = ctx.menu_left_callbacks[i];
							if (callback)
								callback();
						}
						
						return;
					}
				}
			}
		}
	);
}

void clear_controls(game::context& ctx)
{
	ctx.controls["menu_up"]->set_activated_callback(nullptr);
	ctx.controls["menu_down"]->set_activated_callback(nullptr);
	ctx.controls["menu_left"]->set_activated_callback(nullptr);
	ctx.controls["menu_right"]->set_activated_callback(nullptr);
	ctx.controls["menu_select"]->set_activated_callback(nullptr);
	ctx.controls["menu_back"]->set_activated_callback(nullptr);
	
	ctx.menu_mouse_tracker->set_mouse_moved_callback(nullptr);
	ctx.menu_mouse_tracker->set_mouse_button_pressed_callback(nullptr);
}

} // namespace menu
} // namespace game
