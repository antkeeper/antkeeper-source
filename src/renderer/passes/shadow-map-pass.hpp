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

#ifndef ANTKEEPER_SHADOW_MAP_PASS_HPP
#define ANTKEEPER_SHADOW_MAP_PASS_HPP

#include "renderer/render-pass.hpp"
#include "utility/fundamental-types.hpp"

class shader_program;
class shader_input;
class camera;
class resource_manager;
class directional_light;

/**
 *
 */
class shadow_map_pass: public render_pass
{
public:
	shadow_map_pass(::rasterizer* rasterizer, const ::framebuffer* framebuffer, resource_manager* resource_manager);
	virtual ~shadow_map_pass();
	virtual void render(render_context* context) const final;
	
	/**
	 * Sets the linear interpolation weight between uniform and logarithmic frustum-splitting schemes.
	 *
	 * @param weight Linear interpolation weight between uniform and logarithmic frustum-splitting schemes. A value of `0.0` indicates a uniform split scheme, while `1.0` indicates a logarithmic split scheme.
	 */
	void set_split_scheme_weight(float weight);
	
	void set_light(const directional_light* light);
	
	const float4x4* get_shadow_matrices() const;
	const float* get_split_distances() const;

private:
	/**
	 * Calculates the distances along the depth axis at which a view-frustum should be split, given a frustum-splitting scheme.
	 *
	 * @param[out] split_distances Array containing the distances to each split.
	 * @param split_count Number of times the frustum should be split.
	 * @param split_scheme Linear interpolation weight between uniform and logarithmic frustum-splitting schemes. A value of `0.0` indicates a uniform split scheme, while `1.0` indicates a logarithmic split scheme.
	 * @param near Distance to the near clipping plane of the frustum to be split.
	 * @param far Distance to the far clipping plane of the frustum to be split.
	 */
	static void distribute_frustum_splits(float* split_distances, std::size_t split_count, float split_scheme, float near, float far);
	
	shader_program* unskinned_shader_program;
	const shader_input* unskinned_model_view_projection_input;
	
	shader_program* skinned_shader_program;
	const shader_input* skinned_model_view_projection_input;
	
	mutable float split_distances[5];
	mutable float4x4 shadow_matrices[4];
	float4x4 bias_tile_matrices[4];
	float split_scheme_weight;
	const directional_light* light;
};

inline const float4x4* shadow_map_pass::get_shadow_matrices() const
{
	return shadow_matrices;
}

inline const float* shadow_map_pass::get_split_distances() const
{
	return split_distances;
}

#endif // ANTKEEPER_SHADOW_MAP_PASS_HPP
