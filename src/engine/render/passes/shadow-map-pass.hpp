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

#ifndef ANTKEEPER_RENDER_SHADOW_MAP_PASS_HPP
#define ANTKEEPER_RENDER_SHADOW_MAP_PASS_HPP

#include <engine/render/pass.hpp>
#include <engine/utility/fundamental-types.hpp>
#include <engine/scene/directional-light.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-variable.hpp>
#include <memory>

class resource_manager;

namespace render {

/**
 * Renders shadow maps.
 */
class shadow_map_pass: public pass
{
public:
	/**
	 * Constructs a shadow map pass.
	 *
	 * @param rasterizer Rasterizer.
	 * @param framebuffer Shadow map framebuffer.
	 * @param resource_manage Resource manager.
	 */
	shadow_map_pass(gl::rasterizer* rasterizer, resource_manager* resource_manager);
	
	/**
	 * Renders shadow maps for a single camera.
	 *
	 * @param ctx Render context.
	 * @param queue Render queue.
	 */
	void render(const render::context& ctx, render::queue& queue) override;

private:
	/**
	 * Renders cascaded shadow maps for a single directional light.
	 *
	 * @param light Shadow-casting directional light.
	 * @param ctx Render context.
	 * @param queue Render queue.
	 */
	void render_csm(const scene::directional_light& light, const render::context& ctx, render::queue& queue) const;
	
	std::unique_ptr<gl::shader_program> unskinned_shader_program;
	const gl::shader_variable* unskinned_model_view_projection_var;
	
	std::unique_ptr<gl::shader_program> skinned_shader_program;
	const gl::shader_variable* skinned_model_view_projection_var;
	
	float4x4 bias_tile_matrices[4];
};

} // namespace render

#endif // ANTKEEPER_RENDER_SHADOW_MAP_PASS_HPP
