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
#include <engine/gl/drawing-mode.hpp>
#include <engine/render/context.hpp>
#include <engine/render/material.hpp>
#include <engine/gl/shader-template.hpp>
#include <engine/scene/camera.hpp>
#include <engine/scene/collection.hpp>
#include <engine/scene/light.hpp>
#include <engine/geom/primitives/view-frustum.hpp>
#include <engine/config.hpp>
#include <engine/math/interpolation.hpp>
#include <engine/math/vector.hpp>
#include <engine/math/matrix.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/math/projection.hpp>
#include <cmath>
#include <glad/glad.h>
#include <execution>

namespace render {

static bool operation_compare(const render::operation* a, const render::operation* b);

shadow_map_pass::shadow_map_pass(gl::rasterizer* rasterizer, resource_manager* resource_manager):
	pass(rasterizer, nullptr)
{
	// Load unskinned shader template
	auto unskinned_shader_template = resource_manager->load<gl::shader_template>("depth-unskinned.glsl");
	
	// Build unskinned shader program
	unskinned_shader_program = unskinned_shader_template->build({});
	unskinned_model_view_projection_var = unskinned_shader_program->variable("model_view_projection");
	
	// Load skinned shader template
	auto skinned_shader_template = resource_manager->load<gl::shader_template>("depth-skinned.glsl");
	
	// Build skinned shader program
	skinned_shader_program = skinned_shader_template->build({});
	skinned_model_view_projection_var = skinned_shader_program->variable("model_view_projection");
	
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

void shadow_map_pass::render(render::context& ctx)
{
	// For each light
	const auto& lights = ctx.collection->get_objects(scene::light::object_type_id);
	for (const scene::object_base* object: lights)
	{
		// Ignore inactive lights
		if (!object->is_active())
		{
			continue;
		}
		
		// Ignore non-directional lights
		const scene::light& light = static_cast<const scene::light&>(*object);
		if (light.get_light_type() != scene::light_type::directional)
		{
			continue;
		}
		
		// Ignore non-shadow casters
		const scene::directional_light& directional_light = static_cast<const scene::directional_light&>(light);
		if (!directional_light.is_shadow_caster())
		{
			continue;
		}
		
		// Ignore improperly-configured lights
		if (!directional_light.get_shadow_cascade_count() || !directional_light.get_shadow_framebuffer())
		{
			continue;
		}
		
		// Render cascaded shadow maps
		render_csm(directional_light, ctx);
	}
}

void shadow_map_pass::render_csm(const scene::directional_light& light, render::context& ctx)
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
	
	// Calculate distance to shadow cascade depth clipping planes
	const float shadow_clip_far = math::lerp(camera.get_clip_near(), camera.get_clip_far(), light.get_shadow_cascade_coverage());
	
	const unsigned int cascade_count = light.get_shadow_cascade_count();
	
	/// @TODO: don't const_cast
	auto& cascade_distances = const_cast<std::vector<float>&>(light.get_shadow_cascade_distances());
	auto& cascade_matrices = const_cast<std::vector<float4x4>&>(light.get_shadow_cascade_matrices());
	
	// Calculate cascade far clipping plane distances
	cascade_distances[cascade_count - 1] = shadow_clip_far;
	for (unsigned int i = 0; i < cascade_count - 1; ++i)
	{
		const float weight = static_cast<float>(i + 1) / static_cast<float>(cascade_count);
		
		// Calculate linear and logarithmic distribution distances
		const float linear_distance = math::lerp(camera.get_clip_near(), shadow_clip_far, weight);
		const float log_distance = math::log_lerp(camera.get_clip_near(), shadow_clip_far, weight);
		
		// Interpolate between linear and logarithmic distribution distances
		cascade_distances[i] = math::lerp(linear_distance, log_distance, light.get_shadow_cascade_distribution());
	}
	
	// Calculate viewports for each shadow map
	const int shadow_map_resolution = static_cast<int>(light.get_shadow_framebuffer()->get_depth_attachment()->get_width());
	const int cascade_resolution = shadow_map_resolution >> 1;
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
	
	// Reverse half z clip-space coordinates of a cube
	constexpr math::vector<float, 4> clip_space_cube[8] =
	{
		{-1, -1, 1, 1}, // NBL
		{ 1, -1, 1, 1}, // NBR
		{-1,  1, 1, 1}, // NTL
		{ 1,  1, 1, 1}, // NTR
		{-1, -1, 0, 1}, // FBL
		{ 1, -1, 0, 1}, // FBR
		{-1,  1, 0, 1}, // FTL
		{ 1,  1, 0, 1}  // FTR
	};
	
	// Calculate world-space corners of camera view frustum
	math::vector<float, 3> view_frustum_corners[8];
	for (std::size_t i = 0; i < 8; ++i)
	{
		math::vector<float, 4> corner = camera.get_inverse_view_projection() * clip_space_cube[i];
		view_frustum_corners[i] = math::vector<float, 3>(corner) / corner[3];
	}
	
	// Sort render operations
	std::sort(std::execution::par_unseq, ctx.operations.begin(), ctx.operations.end(), operation_compare);
	
	gl::shader_program* active_shader_program = nullptr;
	
	for (unsigned int i = 0; i < cascade_count; ++i)
	{
		// Set viewport for this shadow map
		const int4& viewport = shadow_map_viewports[i];
		rasterizer->set_viewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		
		// Calculate world-space corners and center of camera subfrustum
		const float t_near = (i) ? cascade_distances[i - 1] / camera.get_clip_far() : 0.0f;
		const float t_far = cascade_distances[i] / camera.get_clip_far();
		math::vector<float, 3> subfrustum_center{0, 0, 0};
		math::vector<float, 3> subfrustum_corners[8];
		for (std::size_t i = 0; i < 4; ++i)
		{
			subfrustum_corners[i] = math::lerp(view_frustum_corners[i], view_frustum_corners[i + 4], t_near);
			subfrustum_corners[i + 4] = math::lerp(view_frustum_corners[i], view_frustum_corners[i + 4], t_far);
			
			subfrustum_center += subfrustum_corners[i];
			subfrustum_center += subfrustum_corners[i + 4];
		}
		subfrustum_center *= (1.0f / 8.0f);
		
		// Calculate a view-projection matrix from the light's point-of-view
		const float3 light_up = light.get_rotation() * config::global_up;
		float4x4 light_view = math::look_at(subfrustum_center, subfrustum_center + light.get_direction(), light_up);
		float4x4 light_projection = math::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
		float4x4 light_view_projection = light_projection * light_view;
		
		// Calculate AABB of the subfrustum corners in light clip-space
		geom::box<float> cropping_bounds;
		cropping_bounds.min = {std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()};
		cropping_bounds.max = {-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()};
		for (std::size_t i = 0; i < 8; ++i)
		{
			math::vector<float, 4> corner4 = math::vector<float, 4>(subfrustum_corners[i]);
			corner4[3] = 1.0f;
			corner4 = light_view_projection * corner4;
			
			const math::vector<float, 3> corner3 = math::vector<float, 3>(corner4) / corner4[3];
			
			cropping_bounds.min = math::min(cropping_bounds.min, corner3);
			cropping_bounds.max = math::max(cropping_bounds.max, corner3);
		}
		
		// Quantize clip-space coordinates
		const float texel_scale_x = (cropping_bounds.max.x() - cropping_bounds.min.x()) / static_cast<float>(cascade_resolution);
		const float texel_scale_y = (cropping_bounds.max.y() - cropping_bounds.min.y()) / static_cast<float>(cascade_resolution);
		cropping_bounds.min.x() = std::floor(cropping_bounds.min.x() / texel_scale_x) * texel_scale_x;
		cropping_bounds.max.x() = std::floor(cropping_bounds.max.x() / texel_scale_x) * texel_scale_x;
		cropping_bounds.min.y() = std::floor(cropping_bounds.min.y() / texel_scale_y) * texel_scale_y;
		cropping_bounds.max.y() = std::floor(cropping_bounds.max.y() / texel_scale_y) * texel_scale_y;
		
		/// @NOTE: light z should be modified here to included shadow casters outside the view frustum
		// cropping_bounds.min.z() -= 10.0f;
		// cropping_bounds.max.z() += 10.0f;
		
		// Crop light projection matrix
		light_projection = math::ortho_half_z
		(
			cropping_bounds.min.x(), cropping_bounds.max.x(),
			cropping_bounds.min.y(), cropping_bounds.max.y(),
			cropping_bounds.min.z(), cropping_bounds.max.z()
		);
		
		// Recalculate light view projection matrix
		light_view_projection = light_projection * light_view;
		
		// Calculate world-space to cascade texture-space transformation matrix
		cascade_matrices[i] = bias_tile_matrices[i] * light_view_projection;
		
		for (const render::operation* operation: ctx.operations)
		{
			const render::material* material = operation->material.get();
			if (material)
			{
				// Skip materials which don't cast shadows
				if (material->get_shadow_mode() == material_shadow_mode::none)
				{
					continue;
				}
				
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
			gl::shader_program* shader_program = (operation->skinning_palette.empty()) ? unskinned_shader_program.get() : skinned_shader_program.get();
			if (active_shader_program != shader_program)
			{
				active_shader_program = shader_program;
				rasterizer->use_program(*active_shader_program);
			}
			
			// Calculate model-view-projection matrix
			float4x4 model_view_projection = light_view_projection * operation->transform;
			
			// Upload operation-dependent parameters to shader program
			if (active_shader_program == unskinned_shader_program.get())
			{
				unskinned_model_view_projection_var->update(model_view_projection);
			}
			else if (active_shader_program == skinned_shader_program.get())
			{
				skinned_model_view_projection_var->update(model_view_projection);
			}

			// Draw geometry
			rasterizer->draw_arrays(*operation->vertex_array, operation->drawing_mode, operation->start_index, operation->index_count);
		}
	}
}

bool operation_compare(const render::operation* a, const render::operation* b)
{
	const bool skinned_a = !a->skinning_palette.empty();
	const bool skinned_b = !b->skinning_palette.empty();
	const bool two_sided_a = (a->material) ? a->material->is_two_sided() : false;
	const bool two_sided_b = (b->material) ? b->material->is_two_sided() : false;
	
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
					return (a->vertex_array < b->vertex_array);
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
					return (a->vertex_array < b->vertex_array);
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
					return (a->vertex_array < b->vertex_array);
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
					return (a->vertex_array < b->vertex_array);
				}
			}
		}
	}
}

} // namespace render
