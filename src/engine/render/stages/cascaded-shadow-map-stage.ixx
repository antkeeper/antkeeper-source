// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.render.cascaded_shadow_map_stage;
export import engine.render.stage;
import engine.gl.shader_template;
import engine.gl.shader_program;
import engine.gl.shader_variable;
import engine.gl.pipeline;
import engine.scene.directional_light;
import engine.resources.resource_manager;
import engine.utility.sized_types;
import <memory>;
import <string>;
import <unordered_map>;

export namespace engine::render
{
	/// Renders cascaded shadow maps for directional lights.
	class cascaded_shadow_map_stage: public stage
	{
	public:
		/// Constructs a cascaded shadow map stage.
		/// @param pipeline Graphics pipeline.
		/// @param resource_manager Resource manager for loading shader templates.
		cascaded_shadow_map_stage(gl::pipeline& pipeline, resources::resource_manager& resource_manager);
	
		/// Destructs a cascaded shadow map stage.
		~cascaded_shadow_map_stage() override = default;
	
		void execute(render::context& ctx) override;
	
		/// Sets the maximum bone count for shadow-casting skeletal meshes.
		/// @param bone_count Max bone count.
		/// @warning Triggers rebuilding of skeletal mesh shader.
		void set_max_bone_count(usize bone_count);
	
		/// Returns the maximum bone count for shadow-casting skeletal meshes.
		[[nodiscard]] inline constexpr usize get_max_bone_count() const noexcept
		{
			return m_max_bone_count;
		}

	private:
		/// Queues render operations of objects that may cast shadows visible to the current camera.
		void queue(render::context& ctx, scene::directional_light& light, const math::fmat4& light_view_projection);
	
		/// Renders an atlas of cascaded shadow maps for a single directional light.
		/// @param light Shadow-casting directional light.
		/// @param ctx Render context.
		void render_shadow_atlas(render::context& ctx, scene::directional_light& light);
	
		/// Rebuilds the shader program for static meshes.
		void rebuild_static_mesh_shader_program();
	
		/// Rebuilds the shader program for skeletal meshes.
		void rebuild_skeletal_mesh_shader_program();
	
		gl::pipeline* m_pipeline;
		usize m_max_bone_count{64};
		std::unordered_map<std::string, std::string> m_shader_template_definitions;
		std::shared_ptr<gl::shader_template> m_static_mesh_shader_template;
		std::unique_ptr<gl::shader_program> m_static_mesh_shader_program;
		const gl::shader_variable* m_static_mesh_model_view_projection_var;
		std::shared_ptr<gl::shader_template> m_skeletal_mesh_shader_template;
		std::unique_ptr<gl::shader_program> m_skeletal_mesh_shader_program;
		const gl::shader_variable* m_skeletal_mesh_model_view_projection_var;
		const gl::shader_variable* m_skeletal_mesh_skinning_matrices_var;
	};
}
