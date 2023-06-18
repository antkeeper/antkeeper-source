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

#ifndef ANTKEEPER_RENDER_MATERIAL_PASS_HPP
#define ANTKEEPER_RENDER_MATERIAL_PASS_HPP

#include <engine/render/pass.hpp>
#include <engine/render/material.hpp>
#include <engine/utility/fundamental-types.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-variable.hpp>
#include <engine/gl/texture-2d.hpp>
#include <functional>
#include <unordered_map>
#include <span>

class resource_manager;

namespace render {

/**
 * Renders scene objects using their material-specified shaders and properties.
 */
class material_pass: public pass
{
public:
	material_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager);
	
	void render(render::context& ctx) override;
	
	/// Sets the material to be used when a render operation is missing a material. If no fallback material is specified, render operations without materials will not be processed.
	void set_fallback_material(std::shared_ptr<render::material> fallback);
	
	inline void set_mouse_position(const float2& position)
	{
		mouse_position = position;
	}
	
private:
	struct shader_cache_entry
	{
		std::unique_ptr<gl::shader_program> shader_program;
		
		/// Command buffer which enables the shader and updates render state-related shader variables.
		std::vector<std::function<void()>> shader_command_buffer;
		
		/// Command buffer which updates geometry-related shader variables.
		std::vector<std::function<void()>> geometry_command_buffer;
		
		/// Map of materials to command buffers which update corresponding material shader variables.
		std::unordered_map<const material*, std::vector<std::function<void()>>> material_command_buffers;
	};
	
	/// Map of state hashes to shader cache entries.
	std::unordered_map<std::size_t, shader_cache_entry> shader_cache;
	
	/**
	 * Evaluates the active camera and stores camera information in local variables to be passed to shaders.
	 */
	void evaluate_camera(const render::context& ctx);
	
	/**
	 * Evaluates scene lights and stores lighting information in local variables to be passed to shaders.
	 */
	void evaluate_lighting(const render::context& ctx);
	
	void evaluate_misc(const render::context& ctx);
	
	[[nodiscard]] std::unique_ptr<gl::shader_program> generate_shader_program(const gl::shader_template& shader_template) const;
	
	void build_shader_command_buffer(std::vector<std::function<void()>>& command_buffer, const gl::shader_program& shader_program) const;
	void build_geometry_command_buffer(std::vector<std::function<void()>>& command_buffer, const gl::shader_program& shader_program) const;
	void build_material_command_buffer(std::vector<std::function<void()>>& command_buffer, const gl::shader_program& shader_program, const material& material) const;
	
	// Camera
	const float4x4* view;
	const float4x4* projection;
	const float4x4* view_projection;
	const float3* camera_position;
	float camera_exposure;
	float2 clip_depth;
	float log_depth_coef;
	
	// Light probes
	const gl::texture_cube* light_probe_luminance_texture{};
	const gl::texture_1d* light_probe_illuminance_texture{};
	std::size_t light_probe_count;
	
	// Point lights
	std::vector<float3> point_light_colors;
	std::vector<float3> point_light_positions;
	std::size_t point_light_count;
	
	// Directional lights
	std::vector<float3> directional_light_colors;
	std::vector<float3> directional_light_directions;
	std::size_t directional_light_count;
	
	// Directional shadows
	std::vector<const gl::texture_2d*> directional_shadow_maps;
	std::vector<float> directional_shadow_biases;
	std::vector<const std::vector<float>*> directional_shadow_splits;
	std::vector<const std::vector<float4x4>*> directional_shadow_matrices;
	std::size_t directional_shadow_count;
	
	// Spot lights
	std::vector<float3> spot_light_colors;
	std::vector<float3> spot_light_positions;
	std::vector<float3> spot_light_directions;
	std::vector<float2> spot_light_cutoffs;
	std::size_t spot_light_count;
	
	// Rectangle lights
	std::vector<float3> rectangle_light_colors;
	std::vector<float3> rectangle_light_corners;
	std::size_t rectangle_light_count;
	
	// LTC
	std::shared_ptr<gl::texture_2d> ltc_lut_1;
	std::shared_ptr<gl::texture_2d> ltc_lut_2;
	
	// IBL
	std::shared_ptr<gl::texture_2d> brdf_lut;
	
	// Misc
	float time;
	float timestep;
	unsigned int frame{0};
	float subframe;
	float2 resolution;
	float2 mouse_position;
	
	// Geometry
	const float4x4* model;
	std::span<const float4x4> matrix_palette;
	
	/// Hash of the lighting state.
	std::size_t lighting_state_hash;
	
	std::shared_ptr<render::material> fallback_material;
};

} // namespace render

#endif // ANTKEEPER_RENDER_MATERIAL_PASS_HPP
