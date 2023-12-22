// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RENDER_RESAMPLE_PASS_HPP
#define ANTKEEPER_RENDER_RESAMPLE_PASS_HPP

#include <engine/render/pass.hpp>
#include <engine/gl/shader-template.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-variable.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/texture.hpp>
#include <functional>
#include <memory>

class resource_manager;

namespace render {

/**
 * Resamples a texture.
 */
class resample_pass: public pass
{
public:
	/**
	 * Constructs a resample pass.
	 *
	 * @param pipeline Graphics pipeline.
	 * @param framebuffer Target framebuffer.
	 * @param resource_manager Resource manager.
	 */
	resample_pass(gl::pipeline* pipeline, const gl::framebuffer* framebuffer, resource_manager* resource_manager);
	
	/** Destructs a resample pass. */
	~resample_pass() override = default;
	
	/**
	 * Resamples a texture.
	 *
	 * @param ctx Render context.
	 * @param queue Render queue.
	 */
	void render(render::context& ctx) override;
	
	/**
	 * Sets the resample source texture.
	 *
	 * @param texture Texture to resample.
	 */
	void set_source_texture(std::shared_ptr<gl::texture_2d> texture);

private:
	void rebuild_command_buffer();
	
	std::unique_ptr<gl::vertex_array> m_vertex_array;
	std::unique_ptr<gl::shader_program> m_shader_program;
	std::shared_ptr<gl::texture_2d> m_source_texture;
	std::vector<std::function<void()>> m_command_buffer;
};

} // namespace render

#endif // ANTKEEPER_RENDER_RESAMPLE_PASS_HPP
