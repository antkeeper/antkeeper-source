// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RENDER_CASCADED_SHADOW_MAP_STAGE_HPP
#define ANTKEEPER_RENDER_CASCADED_SHADOW_MAP_STAGE_HPP

#include <engine/render/stage.hpp>
#include <engine/gl/shader-template.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-variable.hpp>
#include <engine/gl/pipeline.hpp>
#include <engine/scene/directional-light.hpp>
#include <engine/resources/resource-manager.hpp>
#include <memory>
#include <string>
#include <unordered_map>

namespace render {

/**
 * Renders cascaded shadow maps for directional lights.
 */
class cascaded_shadow_map_stage: public stage
{
public:
	/**
	 * Constructs a cascaded shadow map stage.
	 *
	 * @param pipeline Graphics pipeline.
	 * @param resource_manager Resource manager for loading shader templates.
	 */
	cascaded_shadow_map_stage(gl::pipeline& pipeline, ::resource_manager& resource_manager);
	
	/** Destructs a cascaded shadow map stage. */
	~cascaded_shadow_map_stage() override = default;
	
	void execute(render::context& ctx) override;
	
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
	 * Queues render operations of objects that may cast shadows visible to the current camera.
	 */
	void queue(render::context& ctx, scene::directional_light& light, const math::fmat4& light_view_projection);
	
	/**
	 * Renders an atlas of cascaded shadow maps for a single directional light.
	 *
	 * @param light Shadow-casting directional light.
	 * @param ctx Render context.
	 */
	void render_shadow_atlas(render::context& ctx, scene::directional_light& light);
	
	/// Rebuilds the shader program for static meshes.
	void rebuild_static_mesh_shader_program();
	
	/// Rebuilds the shader program for skeletal meshes.
	void rebuild_skeletal_mesh_shader_program();
	
	gl::pipeline* m_pipeline;
	
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

#endif // ANTKEEPER_RENDER_CASCADED_SHADOW_MAP_STAGE_HPP
