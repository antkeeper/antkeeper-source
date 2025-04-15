// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.render.passes.bloom_pass;
export import engine.render.pass;
import engine.gl.shader_template;
import engine.gl.shader_program;
import engine.gl.shader_variable;
import engine.gl.vertex_buffer;
import engine.gl.vertex_array;
import engine.gl.texture;
import engine.gl.sampler;
import engine.resources.resource_manager;
import engine.math.vector;
import <memory>;
import <functional>;

export namespace engine::render
{
	/// Physically-based bloom render pass.
	/// @see Jimenez, J. (2014). Next generation post processing in call of duty advanced warfare. SIGGRAPH Advances in Real-Time Rendering.
	/// @see https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom
	class bloom_pass: public pass
	{
	public:
		/// Constructs a bloom pass.
		/// @param pipeline Graphics pipeline.
		/// @param resource_manager Resource manager.
		bloom_pass(gl::pipeline* pipeline, resources::resource_manager* resource_manager);
	
		/// Destructs a bloom pass.
		~bloom_pass() override = default;
	
		/// Renders a bloom texture.
		/// @param ctx Render context.
		void render(render::context& ctx) override;
	
		/// Resizes the mip chain resolution according to the resolution of the source texture.
		void resize();
	
		/// Sets the bloom source texture.
		/// @param texture Bloom source texture.
		void set_source_texture(std::shared_ptr<gl::texture_2d> texture);
	
		/// Sets the mip chain length. A length of `1` indicates a single stage bloom.
		/// @param length Mip chain length.
		void set_mip_chain_length(unsigned int length);
	
		/// Sets the upsample filter radius.
		/// @param radius Upsample filter radius, in texture coordinates.
		void set_filter_radius(float radius);
	
		/// Returns the texture containing the bloom result.
		[[nodiscard]] inline std::shared_ptr<gl::texture_2d> get_bloom_texture() const
		{
			return m_target_textures.empty() ? nullptr : m_target_textures.front();
		}

	private:
		void rebuild_mip_chain();
		void correct_filter_radius();
		void rebuild_command_buffer();
	
		std::shared_ptr<gl::texture_2d> m_source_texture;
		std::shared_ptr<gl::image_2d> m_target_image;
		std::vector<std::shared_ptr<gl::texture_2d>> m_target_textures;
		std::vector<std::shared_ptr<gl::framebuffer>> m_target_framebuffers;
	
		std::unique_ptr<gl::shader_program> m_downsample_karis_shader;
		std::unique_ptr<gl::shader_program> m_downsample_shader;
		std::unique_ptr<gl::shader_program> m_upsample_shader;
	
		std::shared_ptr<gl::sampler> m_sampler;
		std::unique_ptr<gl::vertex_array> m_vertex_array;
		unsigned int m_mip_chain_length{0};
		float m_filter_radius{0.005f};
		math::fvec2 m_corrected_filter_radius{0.005f, 0.005f};
	
		std::vector<std::function<void()>> m_command_buffer;
	};
}
