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
#include <engine/config.hpp>
#include <engine/debug/log.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/gl/texture-2d.hpp>
#include <engine/gl/texture-filter.hpp>
#include <engine/gl/texture-wrapping.hpp>
#include <engine/render/passes/bloom-pass.hpp>
#include <engine/render/passes/final-pass.hpp>
#include <engine/render/passes/fxaa-pass.hpp>
#include <engine/render/passes/resample-pass.hpp>
#include <chrono>
#include <filesystem>
#include <format>
#include <glad/glad.h>
#include <stb/stb_image_write.h>
#include <thread>

namespace graphics {

static void reroute_framebuffers(::game& ctx);

void create_framebuffers(::game& ctx)
{
	debug::log::trace("Creating framebuffers...");
	
	// Calculate render resolution
	const math::ivec2& viewport_size = ctx.window->get_viewport_size();
	ctx.render_resolution = {static_cast<int>(viewport_size.x() * ctx.render_scale + 0.5f), static_cast<int>(viewport_size.y() * ctx.render_scale + 0.5f)};
	
	// Create HDR framebuffer (32F color, 32F depth)
	ctx.hdr_color_texture = std::make_unique<gl::texture_2d>(ctx.render_resolution.x(), ctx.render_resolution.y(), gl::pixel_type::float_32, gl::pixel_format::rgb);
	ctx.hdr_color_texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	ctx.hdr_color_texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	ctx.hdr_color_texture->set_max_anisotropy(0.0f);
	ctx.hdr_depth_texture = std::make_unique<gl::texture_2d>(ctx.render_resolution.x(), ctx.render_resolution.y(), gl::pixel_type::float_32, gl::pixel_format::ds);
	ctx.hdr_depth_texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	ctx.hdr_depth_texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	ctx.hdr_depth_texture->set_max_anisotropy(0.0f);
	ctx.hdr_framebuffer = std::make_unique<gl::framebuffer>(ctx.render_resolution.x(), ctx.render_resolution.y());
	ctx.hdr_framebuffer->attach(gl::framebuffer_attachment_type::color, ctx.hdr_color_texture.get());
	ctx.hdr_framebuffer->attach(gl::framebuffer_attachment_type::depth, ctx.hdr_depth_texture.get());
	ctx.hdr_framebuffer->attach(gl::framebuffer_attachment_type::stencil, ctx.hdr_depth_texture.get());
	
	// Create LDR framebuffers (8-bit color, no depth)
	ctx.ldr_color_texture_a = std::make_unique<gl::texture_2d>(ctx.render_resolution.x(), ctx.render_resolution.y(), gl::pixel_type::uint_8, gl::pixel_format::rgb);
	ctx.ldr_color_texture_a->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	ctx.ldr_color_texture_a->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	ctx.ldr_color_texture_a->set_max_anisotropy(0.0f);
	ctx.ldr_framebuffer_a = std::make_unique<gl::framebuffer>(ctx.render_resolution.x(), ctx.render_resolution.y());
	ctx.ldr_framebuffer_a->attach(gl::framebuffer_attachment_type::color, ctx.ldr_color_texture_a.get());
	
	ctx.ldr_color_texture_b = std::make_unique<gl::texture_2d>(ctx.render_resolution.x(), ctx.render_resolution.y(), gl::pixel_type::uint_8, gl::pixel_format::rgb);
	ctx.ldr_color_texture_b->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	ctx.ldr_color_texture_b->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	ctx.ldr_color_texture_b->set_max_anisotropy(0.0f);
	ctx.ldr_framebuffer_b = std::make_unique<gl::framebuffer>(ctx.render_resolution.x(), ctx.render_resolution.y());
	ctx.ldr_framebuffer_b->attach(gl::framebuffer_attachment_type::color, ctx.ldr_color_texture_b.get());
	
	// Create shadow map framebuffer
	ctx.shadow_map_depth_texture = std::make_shared<gl::texture_2d>(ctx.shadow_map_resolution, ctx.shadow_map_resolution, gl::pixel_type::float_32, gl::pixel_format::d);
	ctx.shadow_map_depth_texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	ctx.shadow_map_depth_texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	ctx.shadow_map_depth_texture->set_max_anisotropy(0.0f);
	ctx.shadow_map_framebuffer = std::make_shared<gl::framebuffer>(ctx.shadow_map_resolution, ctx.shadow_map_resolution);
	ctx.shadow_map_framebuffer->attach(gl::framebuffer_attachment_type::depth, ctx.shadow_map_depth_texture.get());
	
	debug::log::trace("Created framebuffers");
}

void destroy_framebuffers(::game& ctx)
{
	debug::log::trace("Destroying framebuffers...");
	
	// Delete HDR framebuffer and its attachments
	ctx.hdr_framebuffer.reset();
	ctx.hdr_color_texture.reset();
	ctx.hdr_depth_texture.reset();
	
	// Delete LDR framebuffers and attachments
	ctx.ldr_framebuffer_a.reset();
	ctx.ldr_color_texture_a.reset();
	
	ctx.ldr_framebuffer_b.reset();
	ctx.ldr_color_texture_b.reset();
	
	// Delete shadow map framebuffer and its attachments
	ctx.shadow_map_framebuffer.reset();
	ctx.shadow_map_depth_texture.reset();
	
	debug::log::trace("Destroyed framebuffers");
}

void change_render_resolution(::game& ctx, float scale)
{
	debug::log::trace("Changing render resolution to {}...", scale);
	
	// Update render resolution scale
	ctx.render_scale = scale;
	
	// Recalculate render resolution
	const math::ivec2& viewport_size = ctx.window->get_viewport_size();
	ctx.render_resolution = {static_cast<int>(viewport_size.x() * ctx.render_scale + 0.5f), static_cast<int>(viewport_size.y() * ctx.render_scale + 0.5f)};
	
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
	if (viewport_size.x() != ctx.render_resolution.x() || viewport_size.y() != ctx.render_resolution.y())
	{
		ctx.resample_pass->set_enabled(true);
		debug::log::debug("Resample pass enabled");
	}
	else
	{
		ctx.resample_pass->set_enabled(false);
		debug::log::debug("Resample pass disabled");
	}
	reroute_framebuffers(ctx);
	
	debug::log::trace("Changed render resolution to {}", scale);
}

void save_screenshot(::game& ctx)
{
	// Determine timestamped screenshot filename
	const auto time = std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now());
	const std::string screenshot_filename = std::format("{0}-{1:%Y%m%d}T{1:%H%M%S}Z.png", config::application_name, time);
	
	// Determine path to screenshot file
	std::filesystem::path screenshot_filepath = ctx.screenshots_path / screenshot_filename;
	std::string screenshot_filepath_string = screenshot_filepath.string();
	debug::log::debug("Saving screenshot to \"{}\"...", screenshot_filepath_string);
	
	// Get viewport dimensions
	const math::ivec2& viewport_size = ctx.window->get_viewport_size();
	
	// Allocate screenshot image
	std::shared_ptr<image> frame = std::make_shared<image>();
	frame->format(1, 3);
	frame->resize(viewport_size.x(), viewport_size.y());
	
	// Read pixel data from backbuffer into image
	glReadBuffer(GL_BACK);
	glReadPixels(0, 0, viewport_size.x(), viewport_size.y(), GL_RGB, GL_UNSIGNED_BYTE, frame->data());
	
	// Write screenshot file in separate thread
	std::thread
	(
		[frame, path = std::move(screenshot_filepath_string)]
		{
			stbi_flip_vertically_on_write(1);
			stbi_write_png(path.c_str(), frame->width(), frame->height(), frame->channel_count(), frame->data(), frame->width() * frame->channel_count());
			
			debug::log::debug("Saved screenshot to \"{}\"", path);
		}
	).detach();
}

void select_anti_aliasing_method(::game& ctx, render::anti_aliasing_method method)
{
	// Switch AA method
	switch (method)
	{
		// Off
		case render::anti_aliasing_method::none:
			debug::log::debug("Anti-aliasing disabled");
			ctx.fxaa_pass->set_enabled(false);
			reroute_framebuffers(ctx);
			break;
		
		// FXAA
		case render::anti_aliasing_method::fxaa:
			debug::log::debug("Anti-aliasing enabled (FXAA)");
			ctx.fxaa_pass->set_enabled(true);
			reroute_framebuffers(ctx);
			break;
	}
	
	// Update AA method setting
	ctx.anti_aliasing_method = method;
}

void reroute_framebuffers(::game& ctx)
{
	if (ctx.fxaa_pass->is_enabled())
	{
		if (ctx.resample_pass->is_enabled())
		{
			ctx.common_final_pass->set_framebuffer(ctx.ldr_framebuffer_a.get());
			ctx.fxaa_pass->set_framebuffer(ctx.ldr_framebuffer_b.get());
		}
		else
		{
			ctx.common_final_pass->set_framebuffer(ctx.ldr_framebuffer_a.get());
			ctx.fxaa_pass->set_framebuffer(&ctx.window->get_rasterizer()->get_default_framebuffer());
		}
	}
	else
	{
		if (ctx.resample_pass->is_enabled())
		{
			ctx.common_final_pass->set_framebuffer(ctx.ldr_framebuffer_b.get());
		}
		else
		{
			ctx.common_final_pass->set_framebuffer(&ctx.window->get_rasterizer()->get_default_framebuffer());
		}
	}
}

} // namespace graphics
