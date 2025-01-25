// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/graphics.hpp"
#include <engine/config.hpp>
#include <engine/debug/log.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/gl/texture.hpp>
#include <engine/render/passes/clear-pass.hpp>
#include <engine/render/passes/bloom-pass.hpp>
#include <engine/render/passes/composite-pass.hpp>
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

static void rebuild_scene_framebuffer(::game& ctx)
{
	// Construct scene framebuffer sampler
	auto scene_sampler = std::make_shared<gl::sampler>
	(
		gl::sampler_filter::linear,
		gl::sampler_filter::linear,
		gl::sampler_mipmap_mode::linear,
		gl::sampler_address_mode::clamp_to_edge,
		gl::sampler_address_mode::clamp_to_edge
	);
	
	// Construct scene framebuffer color texture
	ctx.scene_color_texture = std::make_shared<gl::texture_2d>
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
		scene_sampler
	);
	
	// Construct scene framebuffer depth texture
	ctx.scene_depth_stencil_texture = std::make_shared<gl::texture_2d>
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
		scene_sampler
	);
	
	// Construct scene framebuffer
	const gl::framebuffer_attachment attachments[2] =
	{
		{
			gl::color_attachment_bit,
			ctx.scene_color_texture->get_image_view(),
			0
		},
		{
			gl::depth_stencil_attachment_bits,
			ctx.scene_depth_stencil_texture->get_image_view(),
			0
		}
	};
	ctx.scene_framebuffer = std::make_shared<gl::framebuffer>(attachments, ctx.render_resolution.x(), ctx.render_resolution.y());
}

static void rebuild_ui_framebuffer(::game& ctx)
{
	auto ui_sampler = std::make_shared<gl::sampler>
	(
		gl::sampler_filter::linear,
		gl::sampler_filter::linear,
		gl::sampler_mipmap_mode::linear,
		gl::sampler_address_mode::clamp_to_edge,
		gl::sampler_address_mode::clamp_to_edge
	);
	
	// Construct UI color texture
	ctx.ui_color_texture = std::make_shared<gl::texture_2d>
	(
		std::make_shared<gl::image_view_2d>
		(
			std::make_shared<gl::image_2d>
			(
				gl::format::r8g8b8a8_srgb,
				ctx.render_resolution.x(),
				ctx.render_resolution.y()
			)
		),
		ui_sampler
	);

	// Construct UI depth-stencil texture
	// ctx.ui_depth_stencil_texture = std::make_shared<gl::texture_2d>
	// (
	// 	std::make_shared<gl::image_view_2d>
	// 	(
	// 		std::make_shared<gl::image_2d>
	// 		(
	// 			gl::format::d24_unorm_s8_uint,
	// 			ctx.render_resolution.x(),
	// 			ctx.render_resolution.y()
	// 		)
	// 	),
	// 	ui_sampler
	// );
	
	// Construct UI framebuffer
	const gl::framebuffer_attachment ui_attachments[1] =
	{
		{
			gl::color_attachment_bit,
			ctx.ui_color_texture->get_image_view(),
			0
		}
		// {
		// 	gl::depth_stencil_attachment_bits,
		// 	ctx.ui_depth_stencil_texture->get_image_view(),
		// 	0
		// }
	};
	ctx.ui_framebuffer = std::make_shared<gl::framebuffer>(ui_attachments, ctx.render_resolution.x(), ctx.render_resolution.y());
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
	
	rebuild_scene_framebuffer(ctx);
	rebuild_ui_framebuffer(ctx);
	rebuild_shadow_framebuffer(ctx);
	
	debug::log_trace("Creating framebuffers... OK");
}

void destroy_framebuffers(::game& ctx)
{
	debug::log_trace("Destroying framebuffers...");
	
	// Delete scene framebuffer and attachments
	ctx.scene_framebuffer.reset();
	ctx.scene_color_texture.reset();
	ctx.scene_depth_stencil_texture.reset();
	
	// Delete UI framebuffer and attachments
	ctx.ui_framebuffer.reset();
	ctx.ui_color_texture.reset();
	ctx.ui_depth_stencil_texture.reset();
	
	// Delete shadow map framebuffer and its attachments
	ctx.shadow_map_framebuffer.reset();
	ctx.shadow_map_depth_texture.reset();
	
	debug::log_trace("Destroying framebuffers... OK");
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
	
	debug::log_debug("Changing render resolution to {}...", scale);
	
	// Update render resolution scale
	ctx.render_scale = scale;
	ctx.render_resolution = render_resolution;
	
	rebuild_scene_framebuffer(ctx);
	rebuild_ui_framebuffer(ctx);
	
	reroute_framebuffers(ctx);
	
	debug::log_debug("Changing render resolution to {}... OK", scale);
}

void save_screenshot(::game& ctx)
{
	// Determine timestamped screenshot filename
	const auto time = std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now());
	const std::string screenshot_filename = std::format("{0}-{1:%Y%m%d}T{1:%H%M%S}Z.png", config::application_slug, time);
	
	// Determine path to screenshot file
	std::filesystem::path screenshot_filepath = ctx.screenshots_path / screenshot_filename;
	std::string screenshot_filepath_string = screenshot_filepath.string();
	debug::log_info("Saving screenshot to \"{}\"...", screenshot_filepath_string);
	
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
			
			debug::log_info("Saving screenshot to \"{}\"... OK", path);
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
			debug::log_info("Anti-aliasing disabled");
			reroute_framebuffers(ctx);
			break;

		default:
			debug::log_error("Anti-aliasing method not implemented");
			return;
	}
	
	// Update AA method setting
	ctx.anti_aliasing_method = method;
}

void reroute_framebuffers(::game& ctx)
{
	ctx.ui_material_pass->set_framebuffer(ctx.ui_framebuffer.get());
	ctx.clear_pass->set_framebuffer(ctx.scene_framebuffer.get());
	ctx.sky_pass->set_framebuffer(ctx.scene_framebuffer.get());
	ctx.scene_material_pass->set_framebuffer(ctx.scene_framebuffer.get());
	ctx.bloom_pass->set_source_texture(ctx.scene_color_texture);
	ctx.composite_pass->set_luminance_texture(ctx.scene_color_texture);
	ctx.composite_pass->set_bloom_texture(ctx.bloom_pass->get_bloom_texture());
	ctx.composite_pass->set_overlay_texture(ctx.ui_color_texture);
}

} // namespace graphics
