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

#include "game/graphics.hpp"
#include "render/passes/bloom-pass.hpp"
#include "render/passes/fxaa-pass.hpp"
#include "render/passes/final-pass.hpp"
#include "render/passes/resample-pass.hpp"
#include "gl/framebuffer.hpp"
#include "gl/texture-2d.hpp"
#include "gl/texture-wrapping.hpp"
#include "gl/texture-filter.hpp"
#include "debug/logger.hpp"
#include "utility/timestamp.hpp"
#include <glad/glad.h>
#include <stb/stb_image_write.h>
#include <thread>
#include <filesystem>

namespace game {
namespace graphics {

static void reroute_framebuffers(game::context& ctx);

void create_framebuffers(game::context& ctx)
{
	ctx.logger->push_task("Creating framebuffers");
	
	// Load render resolution scale from config
	ctx.render_scale = 1.0f;
	if (ctx.config->contains("render_scale"))
		ctx.render_scale = (*ctx.config)["render_scale"].get<float>();
	
	// Calculate render resolution
	const int2& viewport_dimensions = ctx.app->get_viewport_dimensions();
	ctx.render_resolution = {static_cast<int>(viewport_dimensions.x() * ctx.render_scale + 0.5f), static_cast<int>(viewport_dimensions.y() * ctx.render_scale + 0.5f)};
	
	// Create HDR framebuffer (32F color, 32F depth)
	ctx.hdr_color_texture = new gl::texture_2d(ctx.render_resolution.x(), ctx.render_resolution.y(), gl::pixel_type::float_32, gl::pixel_format::rgb);
	ctx.hdr_color_texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	ctx.hdr_color_texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	ctx.hdr_color_texture->set_max_anisotropy(0.0f);
	ctx.hdr_depth_texture = new gl::texture_2d(ctx.render_resolution.x(), ctx.render_resolution.y(), gl::pixel_type::float_32, gl::pixel_format::ds);
	ctx.hdr_depth_texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	ctx.hdr_depth_texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	ctx.hdr_depth_texture->set_max_anisotropy(0.0f);
	ctx.hdr_framebuffer = new gl::framebuffer(ctx.render_resolution.x(), ctx.render_resolution.y());
	ctx.hdr_framebuffer->attach(gl::framebuffer_attachment_type::color, ctx.hdr_color_texture);
	ctx.hdr_framebuffer->attach(gl::framebuffer_attachment_type::depth, ctx.hdr_depth_texture);
	ctx.hdr_framebuffer->attach(gl::framebuffer_attachment_type::stencil, ctx.hdr_depth_texture);
	
	// Create LDR framebuffers (8-bit color, no depth)
	ctx.ldr_color_texture_a = new gl::texture_2d(ctx.render_resolution.x(), ctx.render_resolution.y(), gl::pixel_type::uint_8, gl::pixel_format::rgb);
	ctx.ldr_color_texture_a->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	ctx.ldr_color_texture_a->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	ctx.ldr_color_texture_a->set_max_anisotropy(0.0f);
	ctx.ldr_framebuffer_a = new gl::framebuffer(ctx.render_resolution.x(), ctx.render_resolution.y());
	ctx.ldr_framebuffer_a->attach(gl::framebuffer_attachment_type::color, ctx.ldr_color_texture_a);
	
	ctx.ldr_color_texture_b = new gl::texture_2d(ctx.render_resolution.x(), ctx.render_resolution.y(), gl::pixel_type::uint_8, gl::pixel_format::rgb);
	ctx.ldr_color_texture_b->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	ctx.ldr_color_texture_b->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	ctx.ldr_color_texture_b->set_max_anisotropy(0.0f);
	ctx.ldr_framebuffer_b = new gl::framebuffer(ctx.render_resolution.x(), ctx.render_resolution.y());
	ctx.ldr_framebuffer_b->attach(gl::framebuffer_attachment_type::color, ctx.ldr_color_texture_b);
	
	// Load shadow map resolution from config
	int shadow_map_resolution = 4096;
	if (ctx.config->contains("shadow_map_resolution"))
		shadow_map_resolution = (*ctx.config)["shadow_map_resolution"].get<int>();
	
	// Create shadow map framebuffer
	ctx.shadow_map_depth_texture = new gl::texture_2d(shadow_map_resolution, shadow_map_resolution, gl::pixel_type::float_32, gl::pixel_format::d);
	ctx.shadow_map_depth_texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	ctx.shadow_map_depth_texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	ctx.shadow_map_depth_texture->set_max_anisotropy(0.0f);
	ctx.shadow_map_framebuffer = new gl::framebuffer(shadow_map_resolution, shadow_map_resolution);
	ctx.shadow_map_framebuffer->attach(gl::framebuffer_attachment_type::depth, ctx.shadow_map_depth_texture);
	
	ctx.logger->pop_task(EXIT_SUCCESS);
}

void destroy_framebuffers(game::context& ctx)
{
	ctx.logger->push_task("Destroying framebuffers");
	
	// Delete HDR framebuffer and its attachments
	delete ctx.hdr_framebuffer;
	ctx.hdr_framebuffer = nullptr;
	delete ctx.hdr_color_texture;
	ctx.hdr_color_texture = nullptr;
	delete ctx.hdr_depth_texture;
	ctx.hdr_depth_texture = nullptr;
	
	// Delete LDR framebuffers and attachments
	delete ctx.ldr_framebuffer_a;
	ctx.ldr_framebuffer_a = nullptr;
	delete ctx.ldr_color_texture_a;
	ctx.ldr_color_texture_a = nullptr;
	
	delete ctx.ldr_framebuffer_b;
	ctx.ldr_framebuffer_b = nullptr;
	delete ctx.ldr_color_texture_b;
	ctx.ldr_color_texture_b = nullptr;
	
	// Delete shadow map framebuffer and its attachments
	delete ctx.shadow_map_framebuffer;
	ctx.shadow_map_framebuffer = nullptr;
	delete ctx.shadow_map_depth_texture;
	ctx.shadow_map_depth_texture = nullptr;
	
	ctx.logger->pop_task(EXIT_SUCCESS);
}

void change_render_resolution(game::context& ctx, float scale)
{
	ctx.logger->push_task("Changing render resolution");
	
	// Update render resolution scale
	ctx.render_scale = scale;
	
	// Recalculate render resolution
	const int2& viewport_dimensions = ctx.app->get_viewport_dimensions();
	ctx.render_resolution = {static_cast<int>(viewport_dimensions.x() * ctx.render_scale + 0.5f), static_cast<int>(viewport_dimensions.y() * ctx.render_scale + 0.5f)};
	
	// Resize HDR framebuffer and attachments
	ctx.hdr_framebuffer->resize({ctx.render_resolution.x(), ctx.render_resolution.y()});
	ctx.hdr_color_texture->resize(ctx.render_resolution.x(), ctx.render_resolution.y(), nullptr);
	ctx.hdr_depth_texture->resize(ctx.render_resolution.x(), ctx.render_resolution.y(), nullptr);
	
	// Resize LDR framebuffers and attachments
	ctx.ldr_framebuffer_a->resize({ctx.render_resolution.x(), ctx.render_resolution.y()});
	ctx.ldr_color_texture_a->resize(ctx.render_resolution.x(), ctx.render_resolution.y(), nullptr);
	ctx.ldr_framebuffer_b->resize({ctx.render_resolution.x(), ctx.render_resolution.y()});
	ctx.ldr_color_texture_b->resize(ctx.render_resolution.x(), ctx.render_resolution.y(), nullptr);
	
	// Resize bloom render pass
	ctx.bloom_pass->resize();
	
	// Enable or disable resample pass
	if (viewport_dimensions.x() != ctx.render_resolution.x() || viewport_dimensions.y() != ctx.render_resolution.y())
	{
		ctx.resample_pass->set_enabled(true);
	}
	else
	{
		ctx.resample_pass->set_enabled(false);
	}
	reroute_framebuffers(ctx);
	
	ctx.logger->pop_task(EXIT_SUCCESS);
}

void save_screenshot(game::context& ctx)
{
	// Determine screenshot path
	std::string filename = "antkeeper-" + timestamp() + ".png";
	std::filesystem::path path = ctx.config_path / "gallery" / filename;
	
	ctx.logger->push_task("Saving screenshot to \"" + path.string() + "\"");
	
	const int2& viewport_dimensions = ctx.app->get_viewport_dimensions();
	
	// Allocate image
	std::shared_ptr<image> frame = std::make_shared<image>();
	frame->format(1, 3);
	frame->resize(viewport_dimensions.x(), viewport_dimensions.y());
	
	// Read pixel data from backbuffer into image
	glReadBuffer(GL_BACK);
	glReadPixels(0, 0, viewport_dimensions.x(), viewport_dimensions.y(), GL_RGB, GL_UNSIGNED_BYTE, frame->data());
	
	// Write screenshot file in separate thread
	std::thread
	(
		[frame, path]
		{
			stbi_flip_vertically_on_write(1);
			stbi_write_png(path.string().c_str(), frame->get_width(), frame->get_height(), frame->get_channel_count(), frame->data(), frame->get_width() * frame->get_channel_count());
		}
	).detach();
	
	ctx.logger->pop_task(EXIT_SUCCESS);
}

void toggle_bloom(game::context& ctx, bool enabled)
{
	if (enabled)
	{
		ctx.bloom_pass->set_mip_chain_length(6);
		ctx.bloom_pass->set_enabled(true);
		ctx.common_final_pass->set_bloom_weight(0.04f);
	}
	else
	{
		ctx.bloom_pass->set_mip_chain_length(0);
		ctx.bloom_pass->set_enabled(false);
		ctx.common_final_pass->set_bloom_weight(0.0f);
	}
	
	ctx.bloom_enabled = enabled;
}

void select_anti_aliasing_method(game::context& ctx, render::anti_aliasing_method method)
{
	// Switch AA method
	switch (method)
	{
		// Off
		case render::anti_aliasing_method::none:
			ctx.fxaa_pass->set_enabled(false);
			reroute_framebuffers(ctx);
			break;
		
		// FXAA
		case render::anti_aliasing_method::fxaa:
			ctx.fxaa_pass->set_enabled(true);
			reroute_framebuffers(ctx);
			break;
	}
	
	// Update AA method setting
	ctx.anti_aliasing_method = method;
}

void reroute_framebuffers(game::context& ctx)
{
	if (ctx.fxaa_pass->is_enabled())
	{
		if (ctx.resample_pass->is_enabled())
		{
			ctx.common_final_pass->set_framebuffer(ctx.ldr_framebuffer_a);
			ctx.fxaa_pass->set_framebuffer(ctx.ldr_framebuffer_b);
		}
		else
		{
			ctx.common_final_pass->set_framebuffer(ctx.ldr_framebuffer_a);
			ctx.fxaa_pass->set_framebuffer(&ctx.rasterizer->get_default_framebuffer());
		}
	}
	else
	{
		if (ctx.resample_pass->is_enabled())
		{
			ctx.common_final_pass->set_framebuffer(ctx.ldr_framebuffer_b);
		}
		else
		{
			ctx.common_final_pass->set_framebuffer(&ctx.rasterizer->get_default_framebuffer());
		}
	}
}

} // namespace graphics
} // namespace game
