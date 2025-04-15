// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.render.light_probe_stage;
export import engine.render.stage;
import engine.render.context;
import engine.gl.shader_program;
import engine.gl.shader_variable;
import engine.gl.shader_template;
import engine.gl.pipeline;
import engine.gl.vertex_array;
import engine.gl.vertex_buffer;
import engine.gl.sampler;
import engine.gl.texture;
import engine.resources.resource_manager;
import engine.scene.object;
import engine.utility.sized_types;
import <functional>;
import <memory>;
import <vector>;

export namespace engine::render
{
	/// Updates light probes.
	class light_probe_stage: public stage
	{
	public:
		/// Constructs a light probe stage.
		/// @param pipeline Graphics pipeline.
		/// @param resource_manager Resource manager for loading shader templates.
		/// @exception std::runtime_error Failed to build cubemap to spherical harmonics shader program.
		/// @exception std::runtime_error Cubemap to spherical harmonics shader program has no `cubemap` variable.
		/// @exception std::runtime_error Failed to build cubemap downsample shader program.
		/// @exception std::runtime_error Cubemap downsample shader program has no `cubemap` variable.
		/// @exception std::runtime_error Failed to build cubemap filter LUT shader program.
		/// @exception std::runtime_error Cubemap filter LUT shader program is missing one or more required shader variables.
		light_probe_stage(gl::pipeline& pipeline, resources::resource_manager& resource_manager);
	
		/// Destructs a light probe stage.
		~light_probe_stage() override = default;
	
		void execute(render::context& ctx) override;
	
		/// Sets the number of samples to use when projecting luminance cubemaps into spherical harmonics.
		/// @param count Spherical harmonics sample count.
		/// @warning Triggers rebuilding of cubemap to spherical harmonics shader program.
		/// @warning Triggers recalculation of the illuminance of all light probes on next call to `execute()`.
		/// @exception std::runtime_error Failed to build cubemap to spherical harmonics shader program.
		/// @exception std::runtime_error Cubemap to spherical harmonics shader program has no `cubemap` variable.
		void set_sh_sample_count(usize count);
	
		/// Sets the number of samples to use when filtering luminance cubemap mip chains.
		/// @param count Cubemap filter sample count.
		void set_cubemap_filter_sample_count(usize count);
	
		/// Sets the mip bias to use when filtering luminance cubemap mip chains.
		/// @param bias Cubemap filter mip bias.
		/// @warning Triggers recalculation of the luminance of all light probes on next call to `execute()`.
		/// @exception std::runtime_error Failed to build cubemap filter LUT shader program.
		/// @exception std::runtime_error Cubemap filter LUT shader program is missing one or more required shader variables.
		void set_cubemap_filter_mip_bias(float bias);
	
		/// Returns the number of samples used when projecting luminance cubemaps into spherical harmonics.
		[[nodiscard]] inline usize get_sh_sample_count() const noexcept
		{
			return m_sh_sample_count;
		}
	
		/// Returns the number of samples used when filtering luminance cubemaps.
		[[nodiscard]] inline usize get_cubemap_filter_sample_count() const noexcept
		{
			return m_cubemap_filter_sample_count;
		}
	
	private:
		void rebuild_cubemap_to_sh_shader_program();
		void rebuild_cubemap_downsample_shader_program();
		void rebuild_cubemap_filter_lut_shader_program();
		void rebuild_cubemap_filter_lut_texture();
		void rebuild_cubemap_filter_shader_program();
		void sh_parameters_changed();
		void cubemap_filter_parameters_changed();
	
		void update_light_probes_luminance(const std::vector<scene::object_base*>& light_probes);
		void update_light_probes_illuminance(const std::vector<scene::object_base*>& light_probes);
	
		gl::pipeline* m_pipeline;
	
		std::vector<std::shared_ptr<gl::sampler>> m_downsample_samplers;
		std::vector<std::shared_ptr<gl::sampler>> m_filter_samplers;
		std::unique_ptr<gl::vertex_array> m_vertex_array;
	
		std::shared_ptr<gl::shader_template> m_cubemap_to_sh_shader_template;
		std::unique_ptr<gl::shader_program> m_cubemap_to_sh_shader_program;
		const gl::shader_variable* m_cubemap_to_sh_cubemap_var{};
		usize m_sh_sample_count{512};
		bool m_reproject_sh{true};
	
		std::shared_ptr<gl::shader_template> m_cubemap_downsample_shader_template;
		std::unique_ptr<gl::shader_program> m_cubemap_downsample_shader_program;
		const gl::shader_variable* m_cubemap_downsample_cubemap_var{};
		std::vector<std::unique_ptr<gl::framebuffer>> m_cubemap_downsample_framebuffers;
	
		std::shared_ptr<gl::texture_2d> m_cubemap_filter_lut_texture;
		std::unique_ptr<gl::framebuffer> m_cubemap_filter_lut_framebuffer;
		std::shared_ptr<gl::shader_template> m_cubemap_filter_lut_shader_template;
		std::unique_ptr<gl::shader_program> m_cubemap_filter_lut_shader_program;
		const gl::shader_variable* m_cubemap_filter_lut_resolution_var{};
		const gl::shader_variable* m_cubemap_filter_lut_face_size_var{};
		const gl::shader_variable* m_cubemap_filter_lut_mip_bias_var{};
	
		std::shared_ptr<gl::shader_template> m_cubemap_filter_shader_template;
		std::unique_ptr<gl::shader_program> m_cubemap_filter_shader_program;
		const gl::shader_variable* m_cubemap_filter_cubemap_var{};
		const gl::shader_variable* m_cubemap_filter_filter_lut_var{};
		const gl::shader_variable* m_cubemap_filter_mip_level_var{};
	
		usize m_cubemap_filter_sample_count{32};
		usize m_cubemap_filter_mip_count{5};
		float m_cubemap_filter_mip_bias{1.0f};
		bool m_refilter_cubemaps{true};
	};
}
