// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/menu.hpp"
#include <engine/scene/text.hpp>
#include <engine/animation/ease.hpp>
#include <engine/config.hpp>
#include <algorithm>
#include <engine/math/vector.hpp>
#include "game/components/animation-component.hpp"

namespace menu {

void init_menu_item_index(::game& ctx, hash::fnv32_t menu_name)
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

void update_text_font(::game& ctx)
{
	for (auto [name, value]: ctx.menu_item_texts)
	{
		if (name)
		{
			name->set_font(ctx.menu_font);
			name->set_material(ctx.menu_font_material);
		}
		
		if (value)
		{
			value->set_font(ctx.menu_font);
			value->set_material(ctx.menu_font_material);
		}
	}
	
	ctx.command_line_text->set_font(ctx.debug_font);
	ctx.command_line_text->set_material(ctx.debug_font_material);
	ctx.shell_buffer_text->set_font(ctx.debug_font);
	ctx.shell_buffer_text->set_material(ctx.debug_font_material);
	ctx.frame_time_text->set_font(ctx.debug_font);
	ctx.frame_time_text->set_material(ctx.debug_font_material);
}

void update_text_color(::game& ctx)
{
	for (std::size_t i = 0; i < ctx.menu_item_texts.size(); ++i)
	{
		auto [name, value] = ctx.menu_item_texts[i];
		
		const math::fvec4& color = (i == *ctx.menu_item_index) ? config::menu_active_color : config::menu_inactive_color;
		
		if (name)
		{
			name->set_color(color);
		}
		if (value)
		{
			value->set_color(color);
		}
	}
}

void align_text(::game& ctx, bool center, bool has_back, float anchor_y)
{
	const math::fvec2 viewport_size = math::fvec2(ctx.window->get_viewport_size());
	const math::fvec2 viewport_center = viewport_size * 0.5f;
	
	const float viewport_padding = viewport_size.y() * (1.0f / 9.0f);
	
	// Calculate menu width
	float m_width = 0.0f;
	if (ctx.menu_font->cache_glyph(U'M'); auto m_glyph = ctx.menu_font->get_cached_glyph(U'M'))
	{
		m_width = m_glyph->dimensions[0];
	}
	float column_spacing = m_width * 2.0f;
	const float min_two_column_row_width = m_width * 18.0f;
	float menu_width = 0.0f;
	
	for (auto [name, value]: ctx.menu_item_texts)
	{
		float row_width = 0.0f;
		
		// Add name width to row width
		const auto& name_bounds = name->get_bounds();
		row_width += name_bounds.max.x() - name_bounds.min.x();
		
		if (value)
		{
			// Add value width to row width
			const auto& value_bounds = value->get_bounds();
			row_width += value_bounds.max.x() - value_bounds.min.x();
			
			// Add column spacing to row width
			row_width += column_spacing;
			
			row_width = std::max<float>(min_two_column_row_width, row_width);
		}
		
		menu_width = std::max<float>(menu_width, row_width);
	}
	
	// Align texts
	float menu_height;
	if (has_back)
		menu_height = (ctx.menu_item_texts.size() - 1) * ctx.menu_font->get_metrics().linespace - ctx.menu_font->get_metrics().linegap;
	else
		menu_height = ctx.menu_item_texts.size() * ctx.menu_font->get_metrics().linespace - ctx.menu_font->get_metrics().linegap;
	
	float menu_x = viewport_center.x() - menu_width * 0.5f;
	float menu_y = viewport_center.y() + anchor_y + menu_height * 0.5f - ctx.menu_font->get_metrics().size;
	
	for (std::size_t i = 0; i < ctx.menu_item_texts.size(); ++i)
	{
		auto [name, value] = ctx.menu_item_texts[i];
		
		float x = menu_x;
		float y = menu_y - ctx.menu_font->get_metrics().linespace * i;
		if (has_back && i == ctx.menu_item_texts.size() - 1)
		{
			y = viewport_padding;// + ctx.menu_font->get_metrics().linespace;
		}
		
		if (center || i == ctx.menu_item_texts.size() - 1)
		{
			const auto& name_bounds = name->get_bounds();
			const float name_width =  name_bounds.max.x() - name_bounds.min.x();
			x = viewport_center.x() - name_width * 0.5f;
		}
		
		name->set_translation({std::round(x), std::round(y), 0.0f});
		
		if (value)
		{
			const auto& value_bounds = value->get_bounds();
			const float value_width =  value_bounds.max.x() - value_bounds.min.x();
			
			if (center || i == ctx.menu_item_texts.size() - 1)
				x = viewport_center.x() - value_width * 0.5f;
			else
				x = menu_x + menu_width - value_width;
			
			value->set_translation({std::round(x), std::round(y), 0.0f});
		}
	}
}

void refresh_text(::game& ctx)
{
	for (auto [name, value]: ctx.menu_item_texts)
	{
		name->refresh();
		if (value)
			value->refresh();
	}
}

void add_text_to_ui(::game& ctx)
{
	for (auto [name, value]: ctx.menu_item_texts)
	{
		if (name)
			ctx.ui_canvas->get_scene().add_object(*name);
		if (value)
			ctx.ui_canvas->get_scene().add_object(*value);
	}
}

void remove_text_from_ui(::game& ctx)
{
	for (auto [name, value]: ctx.menu_item_texts)
	{
		if (name)
			ctx.ui_canvas->get_scene().remove_object(*name);
		if (value)
			ctx.ui_canvas->get_scene().remove_object(*value);
	}
}

void delete_text(::game& ctx)
{
	ctx.menu_item_texts.clear();
}

void clear_callbacks(::game& ctx)
{
	// Clear menu item callbacks
	ctx.menu_left_callbacks.clear();
	ctx.menu_right_callbacks.clear();
	ctx.menu_select_callbacks.clear();
	ctx.menu_back_callback = nullptr;
}

void setup_animations(::game& ctx)
{
	auto output_function = [&ctx](auto samples, auto&)
	{
		for (std::size_t i = 0; i < ctx.menu_item_texts.size(); ++i)
		{
			auto [name, value] = ctx.menu_item_texts[i];
			
			math::fvec4 color = (i == *ctx.menu_item_index) ? config::menu_active_color : config::menu_inactive_color;
			color[3] = color[3] * samples[0];
			
			if (name)
				name->set_color(color);
			if (value)
				value->set_color(color);
		}
	};

	// Construct menu fade in sequence
	{
		ctx.menu_fade_in_sequence = std::make_shared<animation_sequence>();
		auto& opacity_track = ctx.menu_fade_in_sequence->tracks()["opacity"];
		auto& opacity_channel = opacity_track.channels().emplace_back();
		opacity_channel.keyframes().emplace(0.0f, 0.0f);
		opacity_channel.keyframes().emplace(config::menu_fade_in_duration, 1.0f);
		opacity_track.output() = output_function;
	}

	// Construct menu fade in sequence
	{
		ctx.menu_fade_out_sequence = std::make_shared<animation_sequence>();
		auto& opacity_track = ctx.menu_fade_out_sequence->tracks()["opacity"];
		auto& opacity_channel = opacity_track.channels().emplace_back();
		opacity_channel.keyframes().emplace(0.0f, 1.0f);
		opacity_channel.keyframes().emplace(config::menu_fade_out_duration, 0.0f);
		opacity_track.output() = output_function;
	}
}

void fade_in(::game& ctx, const std::function<void()>& end_callback)
{
	ctx.menu_fade_in_sequence->cues().clear();
	if (end_callback)
	{
		ctx.menu_fade_in_sequence->cues().emplace(config::menu_fade_in_duration, [end_callback](auto&)
		{
			end_callback();
		});
	}

	auto& player = ctx.entity_registry->get<animation_component>(ctx.menu_entity).player;
	player.rewind();
	player.play(ctx.menu_fade_in_sequence);
	// player.advance(0.0f);
}

void fade_out(::game& ctx, const std::function<void()>& end_callback)
{
	ctx.menu_fade_out_sequence->cues().clear();
	if (end_callback)
	{
		ctx.menu_fade_out_sequence->cues().emplace(config::menu_fade_out_duration, [end_callback](auto&)
		{
			end_callback();
		});
	}

	auto& player = ctx.entity_registry->get<animation_component>(ctx.menu_entity).player;
	player.rewind();
	player.play(ctx.menu_fade_out_sequence);
	// player.advance(0.0f);
}

void fade_in_bg(::game& ctx)
{
	auto& player = ctx.entity_registry->get<animation_component>(ctx.menu_bg_entity).player;
	player.rewind();
	player.play(ctx.menu_bg_fade_in_sequence);
}

void fade_out_bg(::game& ctx)
{
	auto& player = ctx.entity_registry->get<animation_component>(ctx.menu_bg_entity).player;
	player.rewind();
	player.play(ctx.menu_bg_fade_out_sequence);
}

} // namespace menu
