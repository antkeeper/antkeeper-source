// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/graphics.hpp"
#include <engine/config.hpp>
#include <engine/debug/log.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/gl/texture.hpp>
#include <engine/render/passes/bloom-pass.hpp>
#include <engine/render/passes/final-pass.hpp>
#include <engine/render/passes/resample-pass.hpp>
#include <engine/render/passes/sky-pass.hpp>
#include <engine/render/passes/material-pass.hpp>
#include <chrono>
#include <filesystem>
#include <format>
#include <glad/gl.h>
#include <stb/stb_image_write.h>
#include <thread>

namespace graphics {

static void reroute_framebuffers(::game& ctx);

static void rebuild_hdr_framebuffer(::game& ctx)
{
	// Construct HDR framebuffer sampler
	auto hdr_sampler = std::make_shared<gl::sampler>
	(
		gl::sampler_filter::linear,
		gl::sampler_filter::linear,
		gl::sampler_mipmap_mode::linear,
		gl::sampler_address_mode::clamp_to_edge,
		gl::sampler_address_mode::clamp_to_edge
	);
	
	// Construct HDR framebuffer color texture
	ctx.hdr_color_texture = std::make_shared<gl::texture_2d>
	(
		std::make_shared<gl::image_view_2d>
		(
			std::make_shared<gl::image_2d>
			(
				gl::format::r32g32b32_sfloat,
				ctx.render_resolution.x(),
				ctx.render_resolution.y()
			)
		),
		hdr_sampler
	);
	
	// Construct HDR framebuffer depth texture
	ctx.hdr_depth_texture = std::make_shared<gl::texture_2d>
	(
		std::make_shared<gl::image_view_2d>
		(
			std::make_shared<gl::image_2d>
			(
				gl::format::d32_sfloat_s8_uint,
				ctx.render_resolution.x(),
				ctx.render_resolution.y()
			)
		),
		hdr_sampler
	);
	
	// Construct HDR framebuffer
	const gl::framebuffer_attachment hdr_attachments[2] =
	{
		{
			gl::color_attachment_bit,
			ctx.hdr_color_texture->get_image_view(),
			0
		},
		{
			gl::depth_stencil_attachment_bits,
			ctx.hdr_depth_texture->get_image_view(),
			0
		}
	};
	ctx.hdr_framebuffer = std::make_shared<gl::framebuffer>(hdr_attachments, ctx.render_resolution.x(), ctx.render_resolution.y());
}

static void rebuild_ldr_framebuffers(::game& ctx)
{
	auto ldr_sampler = std::make_shared<gl::sampler>
	(
		gl::sampler_filter::linear,
		gl::sampler_filter::linear,
		gl::sampler_mipmap_mode::linear,
		gl::sampler_address_mode::clamp_to_edge,
		gl::sampler_address_mode::clamp_to_edge
	);
	
	// Construct LDR framebuffer A color texture
	ctx.ldr_color_texture_a = std::make_shared<gl::texture_2d>
	(
		std::make_shared<gl::image_view_2d>
		(
			std::make_shared<gl::image_2d>
			(
				gl::format::r8g8b8_unorm,
				ctx.render_resolution.x(),
				ctx.render_resolution.y()
			)
		),
		ldr_sampler
	);
	
	// Construct LDR framebuffer A
	const gl::framebuffer_attachment ldr_attachments_a[1] =
	{
		{
			gl::color_attachment_bit,
			ctx.ldr_color_texture_a->get_image_view(),
			0
		}
	};
	ctx.ldr_framebuffer_a = std::make_shared<gl::framebuffer>(ldr_attachments_a, ctx.render_resolution.x(), ctx.render_resolution.y());
	
	// Construct LDR framebuffer B color texture
	ctx.ldr_color_texture_b = std::make_shared<gl::texture_2d>
	(
		std::make_shared<gl::image_view_2d>
		(
			std::make_shared<gl::image_2d>
			(
				gl::format::r8g8b8_unorm,
				ctx.render_resolution.x(),
				ctx.render_resolution.y()
			)
		),
		ldr_sampler
	);
	
	// Construct LDR framebuffer B
	const gl::framebuffer_attachment ldr_attachments_b[1] =
	{
		{
			gl::color_attachment_bit,
			ctx.ldr_color_texture_b->get_image_view(),
			0
		}
	};
	ctx.ldr_framebuffer_b = std::make_shared<gl::framebuffer>(ldr_attachments_b, ctx.render_resolution.x(), ctx.render_resolution.y());
}

void rebuild_shadow_framebuffer(::game& ctx)
{
	// Construct shadow map sampler
	auto shadow_sampler = std::make_shared<gl::sampler>
	(
		gl::sampler_filter::linear,
		gl::sampler_filter::linear,
		gl::sampler_mipmap_mode::linear,
		gl::sampler_address_mode::clamp_to_border,
		gl::sampler_address_mode::clamp_to_border,
		gl::sampler_address_mode::clamp_to_border,
		0.0f,
		0.0f,
		true,
		gl::compare_op::greater,
		-1000.0f,
		1000.0f,
		std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}
	);
	
	// Construct shadow map framebuffer depth texture
	ctx.shadow_map_depth_texture = std::make_shared<gl::texture_2d>
	(
		std::make_shared<gl::image_view_2d>
		(
			std::make_shared<gl::image_2d>
			(
				gl::format::d32_sfloat,
				ctx.shadow_map_resolution,
				ctx.shadow_map_resolution
			)
		),
		shadow_sampler
	);
	
	// Construct shadow map framebuffer
	const gl::framebuffer_attachment shadow_map_attachments[1] =
	{
		{
			gl::depth_attachment_bit,
			ctx.shadow_map_depth_texture->get_image_view(),
			0
		}
	};
	ctx.shadow_map_framebuffer = std::make_shared<gl::framebuffer>(shadow_map_attachments, ctx.shadow_map_resolution, ctx.shadow_map_resolution);
}

void create_framebuffers(::game& ctx)
{
	debug::log_trace("Creating framebuffers...");
	
	// Calculate render resolution
	const math::ivec2& viewport_size = ctx.window->get_viewport_size();
	ctx.render_resolution = {static_cast<int>(viewport_size.x() * ctx.render_scale + 0.5f), static_cast<int>(viewport_size.y() * ctx.render_scale + 0.5f)};
	
	rebuild_hdr_framebuffer(ctx);
	rebuild_ldr_framebuffers(ctx);
	rebuild_shadow_framebuffer(ctx);
	
	debug::log_trace("Created framebuffers");
}

void destroy_framebuffers(::game& ctx)
{
	debug::log_trace("Destroying framebuffers...");
	
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
	
	debug::log_trace("Destroyed framebuffers");
}

void change_render_resolution(::game& ctx, float scale)
{
	// Recalculate render resolution
	const math::ivec2& viewport_size = ctx.window->get_viewport_size();
	const auto render_resolution = math::ivec2{static_cast<int>(viewport_size.x() * scale + 0.5f), static_cast<int>(viewport_size.y() * scale + 0.5f)};
	
	if (ctx.render_resolution == render_resolution)
	{
		return;
	}
	
	debug::log_trace("Changing render resolution to {}...", scale);
	
	// Update render resolution scale
	ctx.render_scale = scale;
	ctx.render_resolution = render_resolution;
	
	rebuild_hdr_framebuffer(ctx);
	rebuild_ldr_framebuffers(ctx);
	
	// Enable or disable resample pass
	if (viewport_size.x() != ctx.render_resolution.x() || viewport_size.y() != ctx.render_resolution.y())
	{
		ctx.resample_pass->set_enabled(true);
		debug::log_debug("Resample pass enabled");
	}
	else
	{
		ctx.resample_pass->set_enabled(false);
		debug::log_debug("Resample pass disabled");
	}
	
	reroute_framebuffers(ctx);
	
	debug::log_trace("Changed render resolution to {}", scale);
}

void save_screenshot(::game& ctx)
{
	// Determine timestamped screenshot filename
	const auto time = std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now());
	const std::string screenshot_filename = std::format("{0}-{1:%Y%m%d}T{1:%H%M%S}Z.png", config::application_name, time);
	
	// Determine path to screenshot file
	std::filesystem::path screenshot_filepath = ctx.screenshots_path / screenshot_filename;
	std::string screenshot_filepath_string = screenshot_filepath.string();
	debug::log_debug("Saving screenshot to \"{}\"...", screenshot_filepath_string);
	
	// Get viewport dimensions
	const auto& viewport_size = ctx.window->get_viewport_size();
	
	// Allocate screenshot pixel data buffer
	std::unique_ptr<std::byte[]> frame = std::make_unique<std::byte[]>(viewport_size.x() * viewport_size.y() * 3);
	
	// Read pixel data from backbuffer into pixel data buffer
	glReadBuffer(GL_BACK);
	glReadPixels(0, 0, viewport_size.x(), viewport_size.y(), GL_RGB, GL_UNSIGNED_BYTE, frame.get());
	
	// Write screenshot file in separate thread
	std::thread
	(
		[frame = std::move(frame), w = viewport_size.x(), h = viewport_size.y(), path = std::move(screenshot_filepath_string)]
		{
			stbi_flip_vertically_on_write(1);
			stbi_write_png(path.c_str(), w, h, 3, frame.get(), w * 3);
			
			debug::log_debug("Saved screenshot to \"{}\"", path);
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
			debug::log_debug("Anti-aliasing disabled");
			reroute_framebuffers(ctx);
			break;
		
		// FXAA
		case render::anti_aliasing_method::fxaa:
			debug::log_debug("Anti-aliasing enabled (FXAA)");
			reroute_framebuffers(ctx);
			break;
	}
	
	// Update AA method setting
	ctx.anti_aliasing_method = method;
}

void reroute_framebuffers(::game& ctx)
{
	if (ctx.resample_pass->is_enabled())
	{
		ctx.common_final_pass->set_framebuffer(ctx.ldr_framebuffer_a.get());
	}
	else
	{
		ctx.common_final_pass->set_framebuffer(nullptr);
	}
	
	ctx.sky_pass->set_framebuffer(ctx.hdr_framebuffer.get());
	ctx.surface_material_pass->set_framebuffer(ctx.hdr_framebuffer.get());
	ctx.bloom_pass->set_source_texture(ctx.hdr_color_texture);
	ctx.common_final_pass->set_color_texture(ctx.hdr_color_texture);
	ctx.common_final_pass->set_bloom_texture(ctx.bloom_pass->get_bloom_texture());
}

} // namespace graphics
