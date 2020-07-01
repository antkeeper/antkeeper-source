/*
 * Copyright (C) 2020  Christopher J. Howard
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
#include <unordered_map>

class camera;
class shader_program;
class shader_input;
class resource_manager;
class texture_2d;
class shadow_map_pass;

/**
 * Renders scene objects using their material-specified shaders and properties.
 */
class material_pass: public render_pass
{
public:
	material_pass(::rasterizer* rasterizer, const ::framebuffer* framebuffer, resource_manager* resource_manager);
	virtual ~material_pass();
	virtual void render(render_context* context) const final;
	
	/// Sets the material to be used when a render operation is missing a material. If no fallback material is specified, render operations without materials will not be processed.
	void set_fallback_material(const material* fallback);
	
	/// Sets the time tween, which is interpolated between updates
	void set_time_tween(const tween<double>* time);
	
	void set_focal_point_tween(const tween<float3>* focal_point);	
	
	const ::shadow_map_pass* shadow_map_pass;
	const texture_2d* shadow_map;
	
private:
	/**
	 * Sets of known shader input parameters. Each time a new shader is encountered, a parameter set will be created and its inputs connected to the shader program. A null input indiciates that the shader doesn't have that parameter.
	 */
	struct parameter_set
	{
		const shader_input* time;
		const shader_input* model;
		const shader_input* view;
		const shader_input* projection;
		const shader_input* model_view;
		const shader_input* view_projection;
		const shader_input* model_view_projection;
		const shader_input* normal_model_view;
		
		const shader_input* ambient_light_count;
		const shader_input* ambient_light_colors;
		const shader_input* point_light_count;
		const shader_input* point_light_colors;
		const shader_input* point_light_positions;
		const shader_input* point_light_attenuations;
		const shader_input* directional_light_count;
		const shader_input* directional_light_colors;
		const shader_input* directional_light_directions;
		const shader_input* spotlight_count;
		const shader_input* spotlight_colors;
		const shader_input* spotlight_positions;
		const shader_input* spotlight_directions;
		const shader_input* spotlight_attenuations;
		const shader_input* spotlight_cutoffs;
		
		const shader_input* soft_shadows;
		const shader_input* focal_point;
		
		const shader_input* shadow_map_matrices;
		const shader_input* shadow_map_split_distances;
		const shader_input* shadow_map;
	};

	const parameter_set* load_parameter_set(const shader_program* program) const;

	mutable std::unordered_map<const shader_program*, parameter_set*> parameter_sets;
	const material* fallback_material;
	const tween<double>* time_tween;
	const tween<float3>* focal_point_tween;
	texture_2d* soft_shadows_texture;
	
	int max_ambient_light_count;
	int max_point_light_count;
	int max_directional_light_count;
	int max_spotlight_count;
	
	mutable int ambient_light_count;
	mutable int point_light_count;
	mutable int directional_light_count;
	mutable int spotlight_count;

	float3* ambient_light_colors;
	float3* point_light_colors;
	float3* point_light_positions;
	float3* point_light_attenuations;
	float3* directional_light_colors;
	float3* directional_light_directions;
	float3* spotlight_colors;
	float3* spotlight_positions;
	float3* spotlight_directions;
	float3* spotlight_attenuations;
	float2* spotlight_cutoffs;
};

#endif // ANTKEEPER_MATERIAL_PASS_HPP

