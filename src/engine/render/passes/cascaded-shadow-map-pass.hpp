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

#ifndef ANTKEEPER_RENDER_CASCADED_SHADOW_MAP_PASS_HPP
#define ANTKEEPER_RENDER_CASCADED_SHADOW_MAP_PASS_HPP

#include <engine/render/pass.hpp>
#include <engine/math/vector.hpp>
#include <engine/scene/directional-light.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-variable.hpp>
#include <memory>
#include <string>
#include <unordered_map>

class resource_manager;

namespace render {

/**
 * Renders cascaded shadow maps for directional lights.
 */
class cascaded_shadow_map_pass: public pass
{
public:
	/**
	 * Constructs a shadow map pass.
	 *
	 * @param rasterizer Rasterizer.
	 * @param framebuffer Shadow map framebuffer.
	 * @param resource_manage Resource manager.
	 */
	cascaded_shadow_map_pass(gl::rasterizer* rasterizer, resource_manager* resource_manager);
	
	/**
	 * Renders shadow maps for a single camera.
	 *
	 * @param ctx Render context.
	 * @param queue Render queue.
	 */
	void render(render::context& ctx) override;
	
	/**
	 * Sets the maximum bone count for shadow-casting skeletal meshes.
	 *
	 * @param bone_count Max bone count.
	 *
	 * @warning Triggers rebuilding of skeletal mesh shader.
	 */
	void set_max_bone_count(std::size_t bone_count);
	
	/// Returns the maximum bone count for shadow-casting skeletal meshes.
	[[nodiscard]] inline constexpr std::size_t get_max_bone_count() const noexcept
	{
		return m_max_bone_count;
	}

private:
	/**
	 * Renders an atlas of cascaded shadow maps for a single directional light.
	 *
	 * @param light Shadow-casting directional light.
	 * @param ctx Render context.
	 */
	void render_atlas(scene::directional_light& light, render::context& ctx);
	
	/// Rebuilds the shader program for static meshes.
	void rebuild_static_mesh_shader_program();
	
	/// Rebuilds the shader program for skeletal meshes.
	void rebuild_skeletal_mesh_shader_program();
	
	std::size_t m_max_bone_count{64};
	
	std::unordered_map<std::string, std::string> m_shader_template_definitions;
	
	std::shared_ptr<gl::shader_template> m_static_mesh_shader_template;
	std::unique_ptr<gl::shader_program> m_static_mesh_shader_program;
	const gl::shader_variable* m_static_mesh_model_view_projection_var;
	
	std::shared_ptr<gl::shader_template> m_skeletal_mesh_shader_template;
	std::unique_ptr<gl::shader_program> m_skeletal_mesh_shader_program;
	const gl::shader_variable* m_skeletal_mesh_model_view_projection_var;
	const gl::shader_variable* m_skeletal_mesh_matrix_palette_var;
};

} // namespace render

#endif // ANTKEEPER_RENDER_CASCADED_SHADOW_MAP_PASS_HPP
