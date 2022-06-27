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

#include "render/passes/shadow-map-pass.hpp"
#include "resources/resource-manager.hpp"
#include "gl/rasterizer.hpp"
#include "gl/framebuffer.hpp"
#include "gl/shader-program.hpp"
#include "gl/shader-input.hpp"
#include "gl/drawing-mode.hpp"
#include "render/context.hpp"
#include "render/material.hpp"
#include "render/material-flags.hpp"
#include "scene/camera.hpp"
#include "scene/light.hpp"
#include "geom/view-frustum.hpp"
#include "geom/aabb.hpp"
#include "config.hpp"
#include "math/math.hpp"
#include <cmath>
#include <glad/glad.h>

namespace render {

static bool operation_compare(const render::operation& a, const render::operation& b);

void shadow_map_pass::distribute_frustum_splits(float* split_distances, std::size_t split_count, float split_scheme, float near, float far)
{
	// Calculate split distances
	for (std::size_t i = 0; i < split_count; ++i)
	{
		float part = static_cast<float>(i + 1) / static_cast<float>(split_count + 1);
		
		// Calculate uniform split distance
		float uniform_split_distance = near + (far - near) * part;
		
		// Calculate logarithmic split distance
		float log_split_distance = near * std::pow(far / near, part);
			
		// Interpolate between uniform and logarithmic split distances
		split_distances[i] = log_split_distance * split_scheme + uniform_split_distance * (1.0f - split_scheme);
	}
}

shadow_map_pass::shadow_map_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager):
	pass(rasterizer, framebuffer),
	split_scheme_weight(0.5f),
	light(nullptr)
{
	// Load skinned shader program
	unskinned_shader_program = resource_manager->load<gl::shader_program>("depth-unskinned.glsl");
	unskinned_model_view_projection_input = unskinned_shader_program->get_input("model_view_projection");
	
	// Load unskinned shader program
	skinned_shader_program = resource_manager->load<gl::shader_program>("depth-skinned.glsl");
	skinned_model_view_projection_input = skinned_shader_program->get_input("model_view_projection");
	
	// Calculate bias-tile matrices
	float4x4 bias_matrix = math::translate(math::identity4x4<float>, float3{0.5f, 0.5f, 0.5f}) * math::scale(math::identity4x4<float>, float3{0.5f, 0.5f, 0.5f});
	float4x4 tile_scale = math::scale(math::identity4x4<float>, float3{0.5f, 0.5f, 1.0f});
	for (int i = 0; i < 4; ++i)
	{
		float x = static_cast<float>(i % 2) * 0.5f;
		float y = static_cast<float>(i / 2) * 0.5f;
		float4x4 tile_matrix = math::translate(math::identity4x4<float>, float3{x, y, 0.0f}) * tile_scale;
		bias_tile_matrices[i] = tile_matrix * bias_matrix;
	}
}

shadow_map_pass::~shadow_map_pass()
{}

void shadow_map_pass::render(const render::context& ctx, render::queue& queue) const
{
	// Abort if no directional light was set
	if (!light)
	{
		return;
	}
	
	rasterizer->use_framebuffer(*framebuffer);
	
	// Disable blending
	glDisable(GL_BLEND);
	
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	
	// Disable face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	
	// For half-z buffer
	//glDepthRange(-1.0f, 1.0f);
	
	// Get camera
	const scene::camera& camera = *ctx.camera;
	
	// Calculate distances to the depth clipping planes of each frustum split
	float clip_near = camera.get_clip_near_tween().interpolate(ctx.alpha);
	float clip_far = camera.get_clip_far_tween().interpolate(ctx.alpha);
	split_distances[0] = clip_near;
	split_distances[4] = clip_far;
	distribute_frustum_splits(&split_distances[1], 3, split_scheme_weight, clip_near, clip_far);
	
	// Calculate viewports for each shadow map
	const int shadow_map_resolution = std::get<0>(framebuffer->get_dimensions()) / 2;
	float4 shadow_map_viewports[4];
	for (int i = 0; i < 4; ++i)
	{
		int x = i % 2;
		int y = i / 2;
		
		float4& viewport = shadow_map_viewports[i];
		viewport[0] = static_cast<float>(x * shadow_map_resolution);
		viewport[1] = static_cast<float>(y * shadow_map_resolution);
		viewport[2] = static_cast<float>(shadow_map_resolution);
		viewport[3] = static_cast<float>(shadow_map_resolution);
	}
	
	// Calculate a view-projection matrix from the directional light's transform
	math::transform<float> light_transform = light->get_transform_tween().interpolate(ctx.alpha);
	float3 forward = light_transform.rotation * config::global_forward;
	float3 up = light_transform.rotation * config::global_up;
	float4x4 light_view = math::look_at(light_transform.translation, light_transform.translation + forward, up);
	float4x4 light_projection = math::ortho_half_z(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	float4x4 light_view_projection = light_projection * light_view;
	
	// Get the camera's view matrix
	float4x4 camera_view = camera.get_view_tween().interpolate(ctx.alpha);	
	
	float4x4 crop_matrix;
	float4x4 cropped_view_projection;
	float4x4 model_view_projection;
	
	// Sort render queue
	queue.sort(operation_compare);
	
	gl::shader_program* active_shader_program = nullptr;
	
	for (int i = 0; i < 4; ++i)
	{
		// Set viewport for this shadow map
		const float4& viewport = shadow_map_viewports[i];
		rasterizer->set_viewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		
		// Calculate projection matrix for view camera subfrustum
		const float subfrustum_near = split_distances[i];
		const float subfrustum_far = split_distances[i + 1];
		float4x4 subfrustum_projection = math::perspective_half_z(camera.get_fov(), camera.get_aspect_ratio(), subfrustum_near, subfrustum_far);
		
		// Calculate view camera subfrustum
		geom::view_frustum<float> subfrustum(subfrustum_projection * camera_view);
		
		// Create AABB containing the view camera subfrustum corners
		const std::array<float3, 8>& subfrustum_corners = subfrustum.get_corners();
		geom::aabb<float> subfrustum_aabb = {subfrustum_corners[0], subfrustum_corners[0]};
		for (int j = 1; j < 8; ++j)
		{
			for (int k = 0; k < 3; ++k)
			{
				subfrustum_aabb.min_point[k] = std::min<float>(subfrustum_aabb.min_point[k], subfrustum_corners[j][k]);
				subfrustum_aabb.max_point[k] = std::max<float>(subfrustum_aabb.max_point[k], subfrustum_corners[j][k]);
			}
		}
		
		// Transform subfrustum AABB into the light clip space
		geom::aabb<float> cropping_bounds = geom::aabb<float>::transform(subfrustum_aabb, light_view_projection);
	
		// Calculate scale
		float3 scale;
		scale.x = 2.0f / (cropping_bounds.max_point.x - cropping_bounds.min_point.x);
		scale.y = 2.0f / (cropping_bounds.max_point.y - cropping_bounds.min_point.y);
		scale.z = 1.0f / (cropping_bounds.max_point.z - cropping_bounds.min_point.z);
		//scale.z = 2.0f / (cropping_bounds.max_point.z - cropping_bounds.min_point.z);
		
		// Quantize scale
		float scale_quantizer = 64.0f;
		scale.x = 1.0f / std::ceil(1.0f / scale.x * scale_quantizer) * scale_quantizer;
		scale.y = 1.0f / std::ceil(1.0f / scale.y * scale_quantizer) * scale_quantizer;
		
		// Calculate offset
		float3 offset;
		offset.x = (cropping_bounds.max_point.x + cropping_bounds.min_point.x) * scale.x * -0.5f;
		offset.y = (cropping_bounds.max_point.y + cropping_bounds.min_point.y) * scale.y * -0.5f;
		offset.z = -cropping_bounds.min_point.z * scale.z;
		//offset.z = (cropping_bounds.max_point.z + cropping_bounds.min_point.z) * scale.z * -0.5f;

		// Quantize offset
		float half_shadow_map_resolution = static_cast<float>(shadow_map_resolution) * 0.5f;
		offset.x = std::ceil(offset.x * half_shadow_map_resolution) / half_shadow_map_resolution;
		offset.y = std::ceil(offset.y * half_shadow_map_resolution) / half_shadow_map_resolution;
		
		// Crop the light view-projection matrix
		crop_matrix = math::translate(math::identity4x4<float>, offset) * math::scale(math::identity4x4<float>, scale);
		cropped_view_projection = crop_matrix * light_view_projection;
		
		// Calculate shadow matrix
		shadow_matrices[i] = bias_tile_matrices[i] * cropped_view_projection;
		
		for (const render::operation& operation: queue)
		{
			// Skip materials which don't cast shadows
			const render::material* material = operation.material;
			if (material && (material->get_flags() & MATERIAL_FLAG_NOT_SHADOW_CASTER))
			{
				continue;
			}
			
			// Switch shader programs if necessary
			gl::shader_program* shader_program = (operation.pose != nullptr) ? skinned_shader_program : unskinned_shader_program;
			if (active_shader_program != shader_program)
			{
				active_shader_program = shader_program;
				rasterizer->use_program(*active_shader_program);
			}
			
			// Calculate model-view-projection matrix
			model_view_projection = cropped_view_projection * operation.transform;
			
			// Upload operation-dependent parameters to shader program
			if (active_shader_program == unskinned_shader_program)
			{
				unskinned_model_view_projection_input->upload(model_view_projection);
			}
			else if (active_shader_program == skinned_shader_program)
			{
				skinned_model_view_projection_input->upload(model_view_projection);
			}

			// Draw geometry
			rasterizer->draw_arrays(*operation.vertex_array, operation.drawing_mode, operation.start_index, operation.index_count);
		}
	}
}

void shadow_map_pass::set_split_scheme_weight(float weight)
{
	split_scheme_weight = weight;
}

void shadow_map_pass::set_light(const scene::directional_light* light)
{
	this->light = light;
}

bool operation_compare(const render::operation& a, const render::operation& b)
{
	// Determine transparency
	bool skinned_a = (a.pose != nullptr);
	bool skinned_b = (b.pose != nullptr);
	
	if (skinned_a)
	{
		if (skinned_b)
		{
			// A and B are both skinned, sort by VAO
			return (a.vertex_array < b.vertex_array);
		}
		else
		{
			// A is skinned, B is unskinned. Render B first
			return false;
		}
	}
	else
	{
		if (skinned_b)
		{
			// A is unskinned, B is skinned. Render A first
			return true;
		}
		else
		{
			// A and B are both unskinned, sort by VAO
			return (a.vertex_array < b.vertex_array);
		}
	}
}

} // namespace render
