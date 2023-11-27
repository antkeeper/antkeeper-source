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
#include <engine/gl/pipeline.hpp>
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
	 * @param pipeline Graphics pipeline.
	 * @param resource_manager Resource manager for loading shader templates.
	 *
	 * @exception std::runtime_error Failed to build cubemap to spherical harmonics shader program.
	 * @exception std::runtime_error Cubemap to spherical harmonics shader program has no `cubemap` variable.
	 * @exception std::runtime_error Failed to build cubemap downsample shader program.
	 * @exception std::runtime_error Cubemap downsample shader program has no `cubemap` variable.
	 * @exception std::runtime_error Failed to build cubemap filter LUT shader program.
	 * @exception std::runtime_error Cubemap filter LUT shader program is missing one or more required shader variables.
	 */
	light_probe_stage(gl::pipeline& pipeline, ::resource_manager& resource_manager);
	
	void execute(render::context& ctx) override;
	
	/**
	 * Sets the number of samples to use when projecting luminance cubemaps into spherical harmonics.
	 *
	 * @param count Spherical harmonics sample count.
	 *
	 * @warning Triggers rebuilding of cubemap to spherical harmonics shader program.
	 * @warning Triggers recalculation of the illuminance of all light probes on next call to `execute()`.
	 *
	 * @exception std::runtime_error Failed to build cubemap to spherical harmonics shader program.
	 * @exception std::runtime_error Cubemap to spherical harmonics shader program has no `cubemap` variable.
	 */
	void set_sh_sample_count(std::size_t count);
	
	/**
	 * Sets the number of samples to use when filtering luminance cubemap mip chains.
	 *
	 * @param count Cubemap filter sample count.
	 */
	void set_cubemap_filter_sample_count(std::size_t count);
	
	/**
	 * Sets the mip bias to use when filtering luminance cubemap mip chains.
	 *
	 * @param bias Cubemap filter mip bias.
	 *
	 * @warning Triggers recalculation of the luminance of all light probes on next call to `execute()`.
	 *
	 * @exception std::runtime_error Failed to build cubemap filter LUT shader program.
	 * @exception std::runtime_error Cubemap filter LUT shader program is missing one or more required shader variables.
	 */
	void set_cubemap_filter_mip_bias(float bias);
	
	/// Returns the number of samples used when projecting luminance cubemaps into spherical harmonics.
	[[nodiscard]] inline std::size_t get_sh_sample_count() const noexcept
	{
		return m_sh_sample_count;
	}
	
	/// Returns the number of samples used when filtering luminance cubemaps.
	[[nodiscard]] inline std::size_t get_cubemap_filter_sample_count() const noexcept
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
	std::size_t m_sh_sample_count{512};
	bool m_reproject_sh{true};
	
	std::shared_ptr<gl::shader_template> m_cubemap_downsample_shader_template;
	std::unique_ptr<gl::shader_program> m_cubemap_downsample_shader_program;

	const gl::shader_variable* m_cubemap_downsample_cubemap_var{};
	std::vector<std::unique_ptr<gl::framebuffer>> m_cubemap_downsample_framebuffers;
	std::unique_ptr<gl::texture_cube> m_cubemap_downsample_texture;
	
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
	
	std::size_t m_cubemap_filter_sample_count{32};
	std::size_t m_cubemap_filter_mip_count{5};
	float m_cubemap_filter_mip_bias{1.0f};
	bool m_refilter_cubemaps{true};
};

} // namespace render

#endif // ANTKEEPER_RENDER_LIGHT_PROBE_STAGE_HPP
