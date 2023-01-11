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

#include "render/pass.hpp"
#include "utility/fundamental-types.hpp"
#include "scene/directional-light.hpp"
#include "gl/shader-program.hpp"
#include "gl/shader-input.hpp"

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
	 * Destructs a shadow map pass.
	 */
	virtual ~shadow_map_pass();
	
	/**
	 * Renders shadow maps for a single camera.
	 *
	 * @param ctx Render context.
	 * @param queue Render queue.
	 */
	virtual void render(const render::context& ctx, render::queue& queue) const final;

private:
	/**
	 * Renders cascaded shadow maps for a single directional light.
	 *
	 * @param light Shadow-casting directional light.
	 * @param ctx Render context.
	 * @param queue Render queue.
	 */
	void render_csm(const scene::directional_light& light, const render::context& ctx, render::queue& queue) const;
	
	gl::shader_program* unskinned_shader_program;
	const gl::shader_input* unskinned_model_view_projection_input;
	
	gl::shader_program* skinned_shader_program;
	const gl::shader_input* skinned_model_view_projection_input;
	
	float4x4 bias_tile_matrices[4];
};

} // namespace render

#endif // ANTKEEPER_RENDER_SHADOW_MAP_PASS_HPP
