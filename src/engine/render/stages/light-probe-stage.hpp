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

#ifndef ANTKEEPER_RENDER_LIGHT_PROBE_STAGE_HPP
#define ANTKEEPER_RENDER_LIGHT_PROBE_STAGE_HPP

#include <engine/render/stage.hpp>
#include <engine/gl/shader-template.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-variable.hpp>
#include <engine/gl/rasterizer.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/resources/resource-manager.hpp>
#include <functional>
#include <memory>
#include <vector>

namespace render {

/**
 * Updates light probes.
 */
class light_probe_stage: public stage
{
public:
	/**
	 * Constructs a light probe stage.
	 *
	 * @param rasterizer GL rasterizer.
	 * @param resource_manager Resource manager for loading shader templates.
	 *
	 * @exception std::runtime_error Failed to build cubemap to spherical harmonics shader program.
	 * @exception std::runtime_error Cubemap to spherical harmonics shader program has no `cubemap` variable.
	 */
	light_probe_stage(gl::rasterizer& rasterizer, ::resource_manager& resource_manager);
	
	void execute(render::context& ctx) override;
	
	/**
	 * Sets the number of samples to use when projecting cubemaps into spherical harmonics.
	 *
	 * @param count Sample count.
	 *
	 * @warning Triggers rebuilding of cubemap to spherical harmonics shader program.
	 * @warning Triggers recalculation of the illuminance of all light probes on next call to `execute()`.
	 *
	 * @exception std::runtime_error Failed to build cubemap to spherical harmonics shader program.
	 * @exception std::runtime_error Cubemap to spherical harmonics shader program has no `cubemap` variable.
	 */
	void set_sh_sample_count(std::size_t count);
	
	/// Returns the number of samples used when projecting cubemaps into spherical harmonics.
	[[nodiscard]] inline std::size_t get_sh_sample_count() const noexcept
	{
		return m_sh_sample_count;
	}
	
private:
	void rebuild_cubemap_to_sh_shader_program();
	void sh_parameters_changed();
	
	gl::rasterizer* m_rasterizer;
	std::unique_ptr<gl::vertex_buffer> m_quad_vbo;
	std::unique_ptr<gl::vertex_array> m_quad_vao;
	std::shared_ptr<gl::shader_template> m_cubemap_to_sh_shader_template;
	std::unique_ptr<gl::shader_program> m_cubemap_to_sh_shader_program;
	const gl::shader_variable* m_cubemap_var{};
	std::size_t m_sh_sample_count{1024};
	bool m_reproject_sh{true};
};

} // namespace render

#endif // ANTKEEPER_RENDER_LIGHT_PROBE_STAGE_HPP
