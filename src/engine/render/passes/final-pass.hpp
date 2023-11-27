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

#ifndef ANTKEEPER_RENDER_FINAL_PASS_HPP
#define ANTKEEPER_RENDER_FINAL_PASS_HPP

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
 *
 */
class final_pass: public pass
{
public:
	final_pass(gl::pipeline* pipeline, const gl::framebuffer* framebuffer, resource_manager* resource_manager);
	void render(render::context& ctx) override;
	
	void set_color_texture(std::shared_ptr<gl::texture_2d> texture);
	void set_bloom_texture(std::shared_ptr<gl::texture_2d> texture) noexcept;
	void set_bloom_weight(float weight) noexcept;
	void set_blue_noise_texture(std::shared_ptr<gl::texture_2d> texture);

private:
	void rebuild_command_buffer();
	
	std::unique_ptr<gl::vertex_array> m_vertex_array;
	std::unique_ptr<gl::shader_program> m_shader_program;
	
	std::shared_ptr<gl::texture_2d> m_color_texture{};
	std::shared_ptr<gl::texture_2d> m_bloom_texture{};
	float m_bloom_weight{0.04f};
	std::shared_ptr<gl::texture_2d> m_blue_noise_texture;
	float m_blue_noise_scale{1.0f};
	math::fvec2 m_resolution{};
	float m_time{};
	int m_frame{};
	
	std::vector<std::function<void()>> m_command_buffer;
};

} // namespace render

#endif // ANTKEEPER_RENDER_FINAL_PASS_HPP

