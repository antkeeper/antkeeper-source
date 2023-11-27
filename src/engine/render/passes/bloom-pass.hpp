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

#ifndef ANTKEEPER_RENDER_BLOOM_PASS_HPP
#define ANTKEEPER_RENDER_BLOOM_PASS_HPP

#include <engine/render/pass.hpp>
#include <engine/gl/shader-template.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-variable.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/texture.hpp>
#include <engine/gl/sampler.hpp>
#include <memory>
#include <functional>

class resource_manager;

namespace render {

/**
 * Physically-based bloom render pass.
 *
 * @see Jimenez, J. (2014). Next generation post processing in call of duty advanced warfare. SIGGRAPH Advances in Real-Time Rendering.
 * @see https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom
 */
class bloom_pass: public pass
{
public:
	/**
	 * Constructs a bloom pass.
	 *
	 * @param pipeline Graphics pipeline.
	 * @param resource_manager Resource manager.
	 */
	bloom_pass(gl::pipeline* pipeline, resource_manager* resource_manager);
	
	/**
	 * Renders a bloom texture.
	 *
	 * @param ctx Render context.
	 * @param queue Render queue.
	 */
	void render(render::context& ctx) override;
	
	/**
	 * Resizes the mip chain resolution according to the resolution of the source texture.
	 */
	void resize();
	
	/**
	 * Sets the bloom source texture.
	 *
	 * @param texture Bloom source texture.
	 */
	void set_source_texture(std::shared_ptr<gl::texture_2d> texture);
	
	/**
	 * Sets the mip chain length. A length of `1` indicates a single stage bloom.
	 *
	 * @param length Mip chain length.
	 */
	void set_mip_chain_length(unsigned int length);
	
	/**
	 * Sets the upsample filter radius.
	 *
	 * @param radius Upsample filter radius, in texture coordinates.
	 */
	void set_filter_radius(float radius);
	
	/**
	 * Returns the texture containing the bloom result.
	 */
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

} // namespace render

#endif // ANTKEEPER_RENDER_BLOOM_PASS_HPP
