/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_MATERIAL_PASS_HPP
#define ANTKEEPER_MATERIAL_PASS_HPP

#include "renderer/render-pass.hpp"
#include "renderer/material.hpp"
#include "animation/tween.hpp"
#include "utility/fundamental-types.hpp"
#include "event/event-handler.hpp"
#include "event/input-events.hpp"
#include <unordered_map>
#include "gl/shader-program.hpp"
#include "gl/shader-input.hpp"
#include "gl/texture-2d.hpp"

class camera;
class resource_manager;
class shadow_map_pass;

/**
 * Renders scene objects using their material-specified shaders and properties.
 */
class material_pass: public render_pass,
	public event_handler<mouse_moved_event>
{
public:
	material_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager);
	virtual ~material_pass();
	virtual void render(render_context* context) const final;
	
	/// Sets the material to be used when a render operation is missing a material. If no fallback material is specified, render operations without materials will not be processed.
	void set_fallback_material(const material* fallback);
	
	/// Sets the time tween, which is interpolated between updates
	void set_time_tween(const tween<double>* time);
	
	void set_focal_point_tween(const tween<float3>* focal_point);	
	
	const ::shadow_map_pass* shadow_map_pass;
	const gl::texture_2d* shadow_map;
	
private:
	virtual void handle_event(const mouse_moved_event& event);
	
	/**
	 * Sets of known shader input parameters. Each time a new shader is encountered, a parameter set will be created and its inputs connected to the shader program. A null input indiciates that the shader doesn't have that parameter.
	 */
	struct parameter_set
	{
		const gl::shader_input* time;
		const gl::shader_input* mouse;
		const gl::shader_input* resolution;
		const gl::shader_input* camera_position;
		const gl::shader_input* camera_exposure;
		const gl::shader_input* model;
		const gl::shader_input* view;
		const gl::shader_input* projection;
		const gl::shader_input* model_view;
		const gl::shader_input* view_projection;
		const gl::shader_input* model_view_projection;
		const gl::shader_input* normal_model;
		const gl::shader_input* normal_model_view;
		const gl::shader_input* clip_depth;
		const gl::shader_input* log_depth_coef;
		
		const gl::shader_input* ambient_light_count;
		const gl::shader_input* ambient_light_colors;
		const gl::shader_input* point_light_count;
		const gl::shader_input* point_light_colors;
		const gl::shader_input* point_light_positions;
		const gl::shader_input* point_light_attenuations;
		const gl::shader_input* directional_light_count;
		const gl::shader_input* directional_light_colors;
		const gl::shader_input* directional_light_directions;
		const gl::shader_input* directional_light_textures;
		const gl::shader_input* directional_light_texture_matrices;
		const gl::shader_input* directional_light_texture_opacities;
		const gl::shader_input* spot_light_count;
		const gl::shader_input* spot_light_colors;
		const gl::shader_input* spot_light_positions;
		const gl::shader_input* spot_light_directions;
		const gl::shader_input* spot_light_attenuations;
		const gl::shader_input* spot_light_cutoffs;
		
		const gl::shader_input* focal_point;
		
		const gl::shader_input* shadow_map_directional;
		const gl::shader_input* shadow_splits_directional;
		const gl::shader_input* shadow_matrices_directional;
	};

	const parameter_set* load_parameter_set(const gl::shader_program* program) const;

	mutable std::unordered_map<const gl::shader_program*, parameter_set*> parameter_sets;
	const material* fallback_material;
	const tween<double>* time_tween;
	float2 mouse_position;
	const tween<float3>* focal_point_tween;
	
	int max_ambient_light_count;
	int max_point_light_count;
	int max_directional_light_count;
	int max_spot_light_count;
	
	mutable int ambient_light_count;
	mutable int point_light_count;
	mutable int directional_light_count;
	mutable int spot_light_count;

	float3* ambient_light_colors;
	float3* point_light_colors;
	float3* point_light_positions;
	float3* point_light_attenuations;
	float3* directional_light_colors;
	float3* directional_light_directions;
	const gl::texture_2d** directional_light_textures;
	float4x4* directional_light_texture_matrices;
	float* directional_light_texture_opacities;
	float3* spot_light_colors;
	float3* spot_light_positions;
	float3* spot_light_directions;
	float3* spot_light_attenuations;
	float2* spot_light_cutoffs;
};

#endif // ANTKEEPER_MATERIAL_PASS_HPP

