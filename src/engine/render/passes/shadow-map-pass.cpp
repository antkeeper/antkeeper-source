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

#include <engine/render/passes/shadow-map-pass.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/gl/rasterizer.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-input.hpp>
#include <engine/gl/drawing-mode.hpp>
#include <engine/render/context.hpp>
#include <engine/render/material.hpp>
#include <engine/scene/camera.hpp>
#include <engine/scene/collection.hpp>
#include <engine/scene/light.hpp>
#include <engine/geom/view-frustum.hpp>
#include <engine/geom/aabb.hpp>
#include <engine/config.hpp>
#include <engine/math/interpolation.hpp>
#include <engine/math/vector.hpp>
#include <engine/math/matrix.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/math/projection.hpp>
#include <cmath>
#include <glad/glad.h>

namespace render {

static bool operation_compare(const render::operation& a, const render::operation& b);

shadow_map_pass::shadow_map_pass(gl::rasterizer* rasterizer, resource_manager* resource_manager):
	pass(rasterizer, nullptr)
{
	// Load skinned shader program
	unskinned_shader_program = resource_manager->load<gl::shader_program>("depth-unskinned.glsl");
	unskinned_model_view_projection_input = unskinned_shader_program->get_input("model_view_projection");
	
	// Load unskinned shader program
	skinned_shader_program = resource_manager->load<gl::shader_program>("depth-skinned.glsl");
	skinned_model_view_projection_input = skinned_shader_program->get_input("model_view_projection");
	
	// Calculate bias-tile matrices
	float4x4 bias_matrix = math::translate(math::matrix4<float>::identity(), float3{0.5f, 0.5f, 0.5f}) * math::scale(math::matrix4<float>::identity(), float3{0.5f, 0.5f, 0.5f});
	float4x4 tile_scale = math::scale(math::matrix4<float>::identity(), float3{0.5f, 0.5f, 1.0f});
	for (int i = 0; i < 4; ++i)
	{
		float x = static_cast<float>(i % 2) * 0.5f;
		float y = static_cast<float>(i / 2) * 0.5f;
		float4x4 tile_matrix = math::translate(math::matrix4<float>::identity(), float3{x, y, 0.0f}) * tile_scale;
		bias_tile_matrices[i] = tile_matrix * bias_matrix;
	}
}

shadow_map_pass::~shadow_map_pass()
{}

void shadow_map_pass::render(const render::context& ctx, render::queue& queue) const
{
	// Collect lights
	const std::list<scene::object_base*>* lights = ctx.collection->get_objects(scene::light::object_type_id);
	for (const scene::object_base* object: *lights)
	{
		// Ignore inactive lights
		if (!object->is_active())
				continue;
		
		// Ignore non-directional lights
		const scene::light* light = static_cast<const scene::light*>(object);
		if (light->get_light_type() != scene::light_type::directional)
			continue;
		
		// Ignore non-shadow casters
		const scene::directional_light* directional_light = static_cast<const scene::directional_light*>(light);
		if (!directional_light->is_shadow_caster())
			continue;
		
		// Ignore improperly-configured lights
		if (!directional_light->get_shadow_cascade_count() || !directional_light->get_shadow_framebuffer())
			continue;
		
		// Render cascaded shadow maps for light
		render_csm(*directional_light, ctx, queue);
	}
}

void shadow_map_pass::render_csm(const scene::directional_light& light, const render::context& ctx, render::queue& queue) const
{
	rasterizer->use_framebuffer(*light.get_shadow_framebuffer());
	
	// Disable blending
	glDisable(GL_BLEND);
	
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	
	// Enable back-face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	bool two_sided = false;
	
	// For half-z buffer
	glDepthRange(-1.0f, 1.0f);
	
	// Get camera
	const scene::camera& camera = *ctx.camera;
	
	// Get tweened camera parameters
	const float camera_fov = camera.get_fov_tween().interpolate(ctx.alpha);
	const float camera_aspect_ratio = camera.get_aspect_ratio_tween().interpolate(ctx.alpha);
	const float camera_clip_near = camera.get_clip_near_tween().interpolate(ctx.alpha);
	const float camera_clip_far = camera.get_clip_far_tween().interpolate(ctx.alpha);
	
	// Calculate distance to shadow cascade depth clipping planes
	const float shadow_clip_far = math::lerp(camera_clip_near, camera_clip_far, light.get_shadow_cascade_coverage());
	
	const unsigned int cascade_count = light.get_shadow_cascade_count();
	float* cascade_distances = light.get_shadow_cascade_distances();
	float4x4* cascade_matrices = light.get_shadow_cascade_matrices();
	
	// Calculate cascade far clipping plane distances
	cascade_distances[cascade_count - 1] = shadow_clip_far;
	for (unsigned int i = 0; i < cascade_count - 1; ++i)
	{
		const float weight = static_cast<float>(i + 1) / static_cast<float>(cascade_count);
		
		// Calculate linear and logarithmic distribution distances
		const float linear_distance = math::lerp(camera_clip_near, shadow_clip_far, weight);
		const float log_distance = math::log_lerp(camera_clip_near, shadow_clip_far, weight);
		
		// Interpolate between linear and logarithmic distribution distances
		cascade_distances[i] = math::lerp(linear_distance, log_distance, light.get_shadow_cascade_distribution());
	}
	
	// Calculate viewports for each shadow map
	const int shadow_map_resolution = static_cast<int>(light.get_shadow_framebuffer()->get_depth_attachment()->get_width());
	const int cascade_resolution = shadow_map_resolution / 2;
	int4 shadow_map_viewports[4];
	for (int i = 0; i < 4; ++i)
	{
		int x = i % 2;
		int y = i / 2;
		
		int4& viewport = shadow_map_viewports[i];
		viewport[0] = x * cascade_resolution;
		viewport[1] = y * cascade_resolution;
		viewport[2] = cascade_resolution;
		viewport[3] = cascade_resolution;
	}
	
	// Calculate a view-projection matrix from the directional light's transform
	math::transform<float> light_transform = light.get_transform_tween().interpolate(ctx.alpha);
	float3 forward = light_transform.rotation * config::global_forward;
	float3 up = light_transform.rotation * config::global_up;
	float4x4 light_view = math::look_at(light_transform.translation, light_transform.translation + forward, up);
	float4x4 light_projection = math::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	float4x4 light_view_projection = light_projection * light_view;
	
	float4x4 cropped_view_projection;
	float4x4 model_view_projection;
	
	// Sort render queue
	queue.sort(operation_compare);
	
	gl::shader_program* active_shader_program = nullptr;
	
	for (unsigned int i = 0; i < cascade_count; ++i)
	{
		// Set viewport for this shadow map
		const int4& viewport = shadow_map_viewports[i];
		rasterizer->set_viewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		
		// Calculate projection matrix for view camera subfrustum
		const float subfrustum_near = (i) ? cascade_distances[i - 1] : camera_clip_near;
		const float subfrustum_far = cascade_distances[i];
		float4x4 subfrustum_projection = math::perspective_half_z(camera_fov, camera_aspect_ratio, subfrustum_near, subfrustum_far);
		
		// Calculate view camera subfrustum
		geom::view_frustum<float> subfrustum(subfrustum_projection * ctx.view);
		
		// Create AABB containing the view camera subfrustum corners
		const std::array<float3, 8>& subfrustum_corners = subfrustum.get_corners();
		geom::aabb<float> subfrustum_aabb = {subfrustum_corners[0], subfrustum_corners[0]};
		for (int j = 1; j < 8; ++j)
		{
			subfrustum_aabb.min_point = math::min(subfrustum_aabb.min_point, subfrustum_corners[j]);
			subfrustum_aabb.max_point = math::max(subfrustum_aabb.max_point, subfrustum_corners[j]);
		}
		
		// Transform subfrustum AABB into the light clip-space
		geom::aabb<float> cropping_bounds = geom::aabb<float>::transform(subfrustum_aabb, light_view_projection);
		
		// Quantize clip-space coordinates
		const float texel_scale_x = (cropping_bounds.max_point.x() - cropping_bounds.min_point.x()) / static_cast<float>(cascade_resolution);
		const float texel_scale_y = (cropping_bounds.max_point.y() - cropping_bounds.min_point.y()) / static_cast<float>(cascade_resolution);
		cropping_bounds.min_point.x() = std::floor(cropping_bounds.min_point.x() / texel_scale_x) * texel_scale_x;
		cropping_bounds.max_point.x() = std::floor(cropping_bounds.max_point.x() / texel_scale_x) * texel_scale_x;
		cropping_bounds.min_point.y() = std::floor(cropping_bounds.min_point.y() / texel_scale_y) * texel_scale_y;
		cropping_bounds.max_point.y() = std::floor(cropping_bounds.max_point.y() / texel_scale_y) * texel_scale_y;
		
		// Recalculate light projection matrix with quantized coordinates
		light_projection = math::ortho_half_z
		(
			cropping_bounds.min_point.x(), cropping_bounds.max_point.x(),
			cropping_bounds.min_point.y(), cropping_bounds.max_point.y(),
			cropping_bounds.min_point.z(), cropping_bounds.max_point.z()
		);
		
		// Calculate cropped view projection matrix
		cropped_view_projection = light_projection * light_view;
		
		// Calculate world-space to cascade texture-space transformation matrix
		cascade_matrices[i] = bias_tile_matrices[i] * cropped_view_projection;
		
		for (const render::operation& operation: queue)
		{
			const render::material* material = operation.material;
			if (material)
			{
				// Skip materials which don't cast shadows
				if (material->get_shadow_mode() == shadow_mode::none)
					continue;
				
				if (material->is_two_sided() != two_sided)
				{
					if (material->is_two_sided())
					{
						glDisable(GL_CULL_FACE);
					}
					else
					{
						glEnable(GL_CULL_FACE);
					}
					
					two_sided = material->is_two_sided();
				}
			}
			
			// Switch shader programs if necessary
			gl::shader_program* shader_program = (operation.bone_count) ? skinned_shader_program : unskinned_shader_program;
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

bool operation_compare(const render::operation& a, const render::operation& b)
{
	const bool skinned_a = (a.bone_count);
	const bool skinned_b = (b.bone_count);
	const bool two_sided_a = (a.material) ? a.material->is_two_sided() : false;
	const bool two_sided_b = (b.material) ? b.material->is_two_sided() : false;
	
	if (skinned_a)
	{
		if (skinned_b)
		{
			// A and B are both skinned, sort by two-sided
			if (two_sided_a)
			{
				if (two_sided_b)
				{
					// A and B are both two-sided, sort by VAO
					return (a.vertex_array < b.vertex_array);
				}
				else
				{
					// A is two-sided, B is one-sided. Render B first
					return false;
				}
			}
			else
			{
				if (two_sided_b)
				{
					// A is one-sided, B is two-sided. Render A first
					return true;
				}
				else
				{
					// A and B are both one-sided, sort by VAO
					return (a.vertex_array < b.vertex_array);
				}
			}
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
			// A and B are both unskinned, sort by two-sided
			if (two_sided_a)
			{
				if (two_sided_b)
				{
					// A and B are both two-sided, sort by VAO
					return (a.vertex_array < b.vertex_array);
				}
				else
				{
					// A is two-sided, B is one-sided. Render B first
					return false;
				}
			}
			else
			{
				if (two_sided_b)
				{
					// A is one-sided, B is two-sided. Render A first
					return true;
				}
				else
				{
					// A and B are both one-sided, sort by VAO
					return (a.vertex_array < b.vertex_array);
				}
			}
		}
	}
}

} // namespace render
