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

#include <engine/render/passes/cascaded-shadow-map-pass.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/gl/rasterizer.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/drawing-mode.hpp>
#include <engine/render/context.hpp>
#include <engine/render/material.hpp>
#include <engine/render/vertex-attribute.hpp>
#include <engine/gl/shader-template.hpp>
#include <engine/scene/camera.hpp>
#include <engine/scene/collection.hpp>
#include <engine/scene/light.hpp>
#include <engine/geom/primitives/view-frustum.hpp>
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

cascaded_shadow_map_pass::cascaded_shadow_map_pass(gl::rasterizer* rasterizer, resource_manager* resource_manager):
	pass(rasterizer, nullptr)
{
	// Init shader template definitions
	m_shader_template_definitions["VERTEX_POSITION"]    = std::to_string(vertex_attribute::position);
	m_shader_template_definitions["VERTEX_UV"]          = std::to_string(vertex_attribute::uv);
	m_shader_template_definitions["VERTEX_NORMAL"]      = std::to_string(vertex_attribute::normal);
	m_shader_template_definitions["VERTEX_TANGENT"]     = std::to_string(vertex_attribute::tangent);
	m_shader_template_definitions["VERTEX_COLOR"]       = std::to_string(vertex_attribute::color);
	m_shader_template_definitions["VERTEX_BONE_INDEX"]  = std::to_string(vertex_attribute::bone_index);
	m_shader_template_definitions["VERTEX_BONE_WEIGHT"] = std::to_string(vertex_attribute::bone_weight);
	m_shader_template_definitions["VERTEX_BONE_WEIGHT"] = std::to_string(vertex_attribute::bone_weight);
	m_shader_template_definitions["MAX_BONE_COUNT"] = std::to_string(m_max_bone_count);
	
	// Static mesh shader
	{
		// Load static mesh shader template
		m_static_mesh_shader_template = resource_manager->load<gl::shader_template>("shadow-cascade-static-mesh.glsl");
		
		// Build static mesh shader program
		rebuild_static_mesh_shader_program();
	}
	
	// Skeletal mesh shader
	{
		// Load skeletal mesh shader template
		m_skeletal_mesh_shader_template = resource_manager->load<gl::shader_template>("shadow-cascade-skeletal-mesh.glsl");
		
		// Build static mesh shader program
		rebuild_skeletal_mesh_shader_program();
	}
}

void cascaded_shadow_map_pass::render(render::context& ctx)
{
	// For each light
	const auto& lights = ctx.collection->get_objects(scene::light::object_type_id);
	for (scene::object_base* object: lights)
	{
		// Ignore non-directional lights
		auto& light = static_cast<scene::light&>(*object);
		if (light.get_light_type() != scene::light_type::directional)
		{
			continue;
		}
		
		// Ignore non-shadow casters
		auto& directional_light = static_cast<scene::directional_light&>(light);
		if (!directional_light.is_shadow_caster())
		{
			continue;
		}
		
		// Ignore lights that don't share a common layer with the camera
		if (!(directional_light.get_layer_mask() & ctx.camera->get_layer_mask()))
		{
			return;
		}
		
		// Ignore improperly-configured lights
		if (!directional_light.get_shadow_framebuffer())
		{
			continue;
		}
		
		// Render shadow atlas
		render_atlas(directional_light, ctx);
	}
}

void cascaded_shadow_map_pass::set_max_bone_count(std::size_t bone_count)
{
	if (m_max_bone_count != bone_count)
	{
		m_max_bone_count = bone_count;
		
		// Update max bone count shader template definition
		m_shader_template_definitions["MAX_BONE_COUNT"] = std::to_string(m_max_bone_count);
		
		// Rebuild skeletal mesh shader
		rebuild_skeletal_mesh_shader_program();
	}
}

void cascaded_shadow_map_pass::render_atlas(scene::directional_light& light, render::context& ctx)
{
	// Disable blending
	glDisable(GL_BLEND);
	
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GREATER);
	glDepthMask(GL_TRUE);
	
	// Enable back-face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	bool two_sided = false;
	
	// Bind and clear shadow atlas framebuffer
	rasterizer->use_framebuffer(*light.get_shadow_framebuffer());
	rasterizer->clear_framebuffer(false, true, false);
	
	// Get light layer mask
	const auto light_layer_mask = light.get_layer_mask();
	
	// Get camera
	const scene::camera& camera = *ctx.camera;
	
	// Calculate distance to shadow cascade depth clipping planes
	const auto shadow_clip_near = camera.get_clip_near();
	const auto shadow_clip_far = camera.get_clip_near() + light.get_shadow_distance();
	
	// Get light shadow cascade distances and matrices
	const auto cascade_count = light.get_shadow_cascade_count();
	const auto cascade_distances = light.get_shadow_cascade_distances();
	const auto cascade_matrices = light.get_shadow_cascade_matrices();
	
	// Calculate cascade far clipping plane distances
	cascade_distances[cascade_count - 1] = shadow_clip_far;
	for (unsigned int i = 0; i < cascade_count - 1; ++i)
	{
		const float weight = static_cast<float>(i + 1) / static_cast<float>(cascade_count);
		
		// Calculate linear and logarithmic split distances
		const float linear_distance = math::lerp(shadow_clip_near, shadow_clip_far, weight);
		const float log_distance = math::log_lerp(shadow_clip_near, shadow_clip_far, weight);
		
		// Interpolate between linear and logarithmic split distances
		cascade_distances[i] = math::lerp(linear_distance, log_distance, light.get_shadow_cascade_distribution());
	}
	
	// Calculate viewports for each shadow map
	const int shadow_map_resolution = static_cast<int>(light.get_shadow_framebuffer()->get_depth_attachment()->get_width());
	const int cascade_resolution = shadow_map_resolution >> 1;
	math::ivec4 shadow_map_viewports[4];
	for (int i = 0; i < 4; ++i)
	{
		int x = i % 2;
		int y = i / 2;
		
		math::ivec4& viewport = shadow_map_viewports[i];
		viewport[0] = x * cascade_resolution;
		viewport[1] = y * cascade_resolution;
		viewport[2] = cascade_resolution;
		viewport[3] = cascade_resolution;
	}
	
	// Sort render operations
	std::sort(std::execution::par_unseq, ctx.operations.begin(), ctx.operations.end(), operation_compare);
	
	gl::shader_program* active_shader_program = nullptr;
	
	// Precalculate frustum minimal bounding sphere terms
	const auto k = std::sqrt(1.0f + camera.get_aspect_ratio() * camera.get_aspect_ratio()) * std::tan(camera.get_vertical_fov() * 0.5f);
	const auto k2 = k * k;
	const auto k4 = k2 * k2;
	
	for (unsigned int i = 0; i < cascade_count; ++i)
	{
		// Set viewport for this shadow map
		const math::ivec4& viewport = shadow_map_viewports[i];
		rasterizer->set_viewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		
		// Find minimal bounding sphere of subfrustum in view-space
		// @see https://lxjk.github.io/2017/04/15/Calculate-Minimal-Bounding-Sphere-of-Frustum.html
		geom::sphere<float> subfrustum_bounds;
		{
			// Get subfrustum near and far distances
			const auto n = (i) ? cascade_distances[i - 1] : camera.get_clip_near();
			const auto f = cascade_distances[i];
			
			if (k2 >= (f - n) / (f + n))
			{
				subfrustum_bounds.center = {0, 0, -f};
				subfrustum_bounds.radius = f * k;
			}
			else
			{
				subfrustum_bounds.center = {0, 0, -0.5f * (f + n) * (1.0f + k2)};
				subfrustum_bounds.radius = 0.5f * std::sqrt((k4 + 2.0f * k2 + 1.0f) * (f * f + n * n) + 2.0f * f * (k4 - 1.0f) * n);
			}
		}
		
		// Transform subfrustum bounds into world-space
		subfrustum_bounds.center = camera.get_translation() + camera.get_rotation() * subfrustum_bounds.center;
		
		// Discretize view-space subfrustum bounds
		const auto texel_scale = static_cast<float>(cascade_resolution) / (subfrustum_bounds.radius * 2.0f);
		subfrustum_bounds.center = math::conjugate(light.get_rotation()) * subfrustum_bounds.center;
		subfrustum_bounds.center = math::floor(subfrustum_bounds.center * texel_scale) / texel_scale;
		subfrustum_bounds.center = light.get_rotation() * subfrustum_bounds.center;
		
		// Construct light view matrix
		const auto light_view = math::look_at_rh(subfrustum_bounds.center, subfrustum_bounds.center + light.get_direction(), light.get_rotation() * math::fvec3{0, 1, 0});
		
		// Construct light projection matrix (reversed depth)
		const auto light_projection = math::ortho_half_z
		(
			-subfrustum_bounds.radius, subfrustum_bounds.radius,
			-subfrustum_bounds.radius, subfrustum_bounds.radius,
			subfrustum_bounds.radius, -subfrustum_bounds.radius
		);
		
		// Construct light view-projection matrix
		const auto light_view_projection = light_projection * light_view;
		
		// Update world-space to cascade texture-space transformation matrix
		cascade_matrices[i] = light.get_shadow_scale_bias_matrices()[i] * light_view_projection;
		
		for (const render::operation* operation: ctx.operations)
		{
			// Skip operations which don't share any layers with the shadow-casting light
			if (!(operation->layer_mask & light_layer_mask))
			{
				continue;
			}
			
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
			gl::shader_program* shader_program = (operation->matrix_palette.empty()) ? m_static_mesh_shader_program.get() : m_skeletal_mesh_shader_program.get();
			if (active_shader_program != shader_program)
			{
				active_shader_program = shader_program;
				rasterizer->use_program(*active_shader_program);
			}
			
			// Calculate model-view-projection matrix
			math::fmat4 model_view_projection = light_view_projection * operation->transform;
			
			// Upload operation-dependent parameters to shader program
			if (active_shader_program == m_static_mesh_shader_program.get())
			{
				m_static_mesh_model_view_projection_var->update(model_view_projection);
			}
			else if (active_shader_program == m_skeletal_mesh_shader_program.get())
			{
				m_skeletal_mesh_model_view_projection_var->update(model_view_projection);
				m_skeletal_mesh_matrix_palette_var->update(operation->matrix_palette);
			}

			// Draw geometry
			rasterizer->draw_arrays(*operation->vertex_array, operation->drawing_mode, operation->start_index, operation->index_count);
		}
	}
}

void cascaded_shadow_map_pass::rebuild_static_mesh_shader_program()
{
	m_static_mesh_shader_program = m_static_mesh_shader_template->build(m_shader_template_definitions);
	if (!m_static_mesh_shader_program->linked())
	{
		debug::log::error("Failed to build cascaded shadow map shader program for static meshes: {}", m_static_mesh_shader_program->info());
		debug::log::warning("{}", m_static_mesh_shader_template->configure(gl::shader_stage::vertex));
		
		m_static_mesh_model_view_projection_var = nullptr;
	}
	else
	{
		m_static_mesh_model_view_projection_var = m_static_mesh_shader_program->variable("model_view_projection");
	}
}

void cascaded_shadow_map_pass::rebuild_skeletal_mesh_shader_program()
{
	m_skeletal_mesh_shader_program = m_skeletal_mesh_shader_template->build(m_shader_template_definitions);
	if (!m_skeletal_mesh_shader_program->linked())
	{
		debug::log::error("Failed to build cascaded shadow map shader program for skeletal meshes: {}", m_skeletal_mesh_shader_program->info());
		debug::log::warning("{}", m_skeletal_mesh_shader_template->configure(gl::shader_stage::vertex));
		
		m_skeletal_mesh_model_view_projection_var = nullptr;
		m_skeletal_mesh_matrix_palette_var = nullptr;
	}
	else
	{
		m_skeletal_mesh_model_view_projection_var = m_skeletal_mesh_shader_program->variable("model_view_projection");
		m_skeletal_mesh_matrix_palette_var = m_skeletal_mesh_shader_program->variable("matrix_palette");
	}
}

bool operation_compare(const render::operation* a, const render::operation* b)
{
	const bool skinned_a = !a->matrix_palette.empty();
	const bool skinned_b = !b->matrix_palette.empty();
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
