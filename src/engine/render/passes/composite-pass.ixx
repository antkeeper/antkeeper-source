// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.render.passes.composite_pass;
export import engine.render.pass;
import engine.gl.shader_template;
import engine.gl.shader_program;
import engine.gl.shader_variable;
import engine.gl.vertex_buffer;
import engine.gl.vertex_array;
import engine.gl.texture;
import engine.resources.resource_manager;
import engine.math.vector;
import <functional>;
import <memory>;

export namespace engine::render
{
	/// Combines multiple images to form a single, output display image.
	class composite_pass: public pass
	{
	public:
		/// Constructs a composite pass.
		composite_pass(gl::pipeline* pipeline, const gl::framebuffer* framebuffer, resources::resource_manager* resource_manager);
	
		/// Destructs a composite pass.
		~composite_pass() override = default;
	
		void render(render::context& ctx) override;
	
		/// Sets the luminance texture.
		/// @param texture Luminance texture containing scene-referred open domain tristimulus values.
		void set_luminance_texture(std::shared_ptr<gl::texture_2d> texture);

		/// @name Bloom
		/// @{

		/// Sets the bloom texture.
		/// @param texture Bloom texture containing scene-referred open domain tristimulus values.
		void set_bloom_texture(std::shared_ptr<gl::texture_2d> texture) noexcept;

		/// Sets the bloom strength.
		/// @param strength Bloom strength, on [0, 1].
		void set_bloom_strength(float strength) noexcept;

		/// @}

		/// @name Noise
		/// @{

		/// Sets the noise texture.
		/// @param texture Noise texture.
		void set_noise_texture(std::shared_ptr<gl::texture_2d> texture);

		/// Sets the noise strength.
		/// @param strength Noise strength, on [0, 1].
		void set_noise_strength(float strength) noexcept;

		/// @}

		/// @name Overlay
		/// @{

		/// Sets the overlay texture.
		/// @param texture Overlay texture containing linear, closed domain tristimulus values, in the display color space.
		void set_overlay_texture(std::shared_ptr<gl::texture_2d> texture);

		/// @}

	private:
		void rebuild_command_buffer();
	
		std::unique_ptr<gl::vertex_array> m_vertex_array;
		std::unique_ptr<gl::shader_program> m_shader_program;
	
		std::shared_ptr<gl::texture_2d> m_luminance_texture{};
		std::shared_ptr<gl::texture_2d> m_bloom_texture{};
		float m_bloom_strength{0.04f};
		std::shared_ptr<gl::texture_2d> m_noise_texture;
		float m_noise_strength{1.0f / 255.0f};
		std::shared_ptr<gl::texture_2d> m_overlay_texture;
		math::fvec2 m_resolution{};
		float m_time{};
		int m_frame{};
	
		std::vector<std::function<void()>> m_command_buffer;
	};
}
