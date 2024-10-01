// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/render/stages/cascaded-shadow-map-stage.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/gl/pipeline.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/render/context.hpp>
#include <engine/render/material.hpp>
#include <engine/render/vertex-attribute-location.hpp>
#include <engine/scene/camera.hpp>
#include <engine/scene/collection.hpp>
#include <engine/scene/light.hpp>
#include <engine/geom/primitives/view-frustum.hpp>
#include <engine/math/functions.hpp>
#include <engine/math/vector.hpp>
#include <engine/math/matrix.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/math/projection.hpp>
#include <engine/geom/primitives/view-frustum.hpp>
#include <cmath>
#include <execution>

namespace render {

static bool operation_compare(const render::operation* a, const render::operation* b);

cascaded_shadow_map_stage::cascaded_shadow_map_stage(gl::pipeline& pipeline, ::resource_manager& resource_manager):
	m_pipeline(&pipeline)
{
	// Init shader template definitions
	m_shader_template_definitions["VERTEX_POSITION"]    = std::to_string(vertex_attribute_location::position);
	m_shader_template_definitions["VERTEX_UV"]          = std::to_string(vertex_attribute_location::uv);
	m_shader_template_definitions["VERTEX_NORMAL"]      = std::to_string(vertex_attribute_location::normal);
	m_shader_template_definitions["VERTEX_TANGENT"]     = std::to_string(vertex_attribute_location::tangent);
	m_shader_template_definitions["VERTEX_COLOR"]       = std::to_string(vertex_attribute_location::color);
	m_shader_template_definitions["VERTEX_BONE_INDEX"]  = std::to_string(vertex_attribute_location::bone_index);
	m_shader_template_definitions["VERTEX_BONE_WEIGHT"] = std::to_string(vertex_attribute_location::bone_weight);
	m_shader_template_definitions["VERTEX_BONE_WEIGHT"] = std::to_string(vertex_attribute_location::bone_weight);
	m_shader_template_definitions["MAX_BONE_COUNT"] = std::to_string(m_max_bone_count);
	
	// Static mesh shader
	{
		// Load static mesh shader template
		m_static_mesh_shader_template = resource_manager.load<gl::shader_template>("shadow-cascade-static-mesh.glsl");
		
		// Build static mesh shader program
		rebuild_static_mesh_shader_program();
	}
	
	// Skeletal mesh shader
	{
		// Load skeletal mesh shader template
		m_skeletal_mesh_shader_template = resource_manager.load<gl::shader_template>("shadow-cascade-skeletal-mesh.glsl");
		
		// Build static mesh shader program
		rebuild_skeletal_mesh_shader_program();
	}
}

void cascaded_shadow_map_stage::execute(render::context& ctx)
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
			continue;
		}
		
		// Ignore improperly-configured lights
		if (!directional_light.get_shadow_framebuffer())
		{
			continue;
		}
		
		// Render shadow atlas
		render_shadow_atlas(ctx, directional_light);
	}
	
	ctx.operations.clear();
}

void cascaded_shadow_map_stage::set_max_bone_count(std::size_t bone_count)
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

void cascaded_shadow_map_stage::queue(render::context& ctx, scene::directional_light& light, const math::fmat4& light_view_projection)
{
	// Clear pre-existing render operations
	ctx.operations.clear();
	
	// Combine camera and light layer masks
	const auto camera_light_layer_mask = ctx.camera->get_layer_mask() & light.get_layer_mask();
	
	// Build light view frustum from light view projection matrix
	const geom::view_frustum<float> light_view_frustum(light_view_projection);
	
	// Tests whether a box is completely outside a plane
	auto box_outside_plane = [](const geom::box<float>& box, const geom::plane<float>& plane) -> bool
	{
		const math::fvec3 p =
		{
			(plane.normal.x() > 0.0f) ? box.max.x() : box.min.x(),
			(plane.normal.y() > 0.0f) ? box.max.y() : box.min.y(),
			(plane.normal.z() > 0.0f) ? box.max.z() : box.min.z()
		};
		
		return plane.distance(p) < 0.0f;
	};
	
	// For each object in the scene collection
	const auto& objects = ctx.collection->get_objects();
	for (const auto& object: objects)
	{
		// Cull object if it doesn't share a common layer with the camera and light
		if (!(object->get_layer_mask() & camera_light_layer_mask))
		{
			continue;
		}
		
		// Ignore cameras and lights
		if (object->get_object_type_id() == scene::camera::object_type_id || object->get_object_type_id() == scene::light::object_type_id)
		{
			continue;
		}
		
		// Cull object if it's outside of the light view frustum (excluding near plane [reverse-z, so far=near])
		const auto& object_bounds = object->get_bounds();
		if (box_outside_plane(object_bounds, light_view_frustum.left()) ||
			box_outside_plane(object_bounds, light_view_frustum.right()) ||
			box_outside_plane(object_bounds, light_view_frustum.bottom()) ||
			box_outside_plane(object_bounds, light_view_frustum.top()) ||
			box_outside_plane(object_bounds, light_view_frustum.near()))
		{
			continue;
		}
		
		// Add object render operations to render context
		object->render(ctx);
	}
}

void cascaded_shadow_map_stage::render_shadow_atlas(render::context& ctx, scene::directional_light& light)
{
	// Disable blending
	m_pipeline->set_color_blend_enabled(false);
	
	// Enable depth testing
	m_pipeline->set_depth_test_enabled(true);
	m_pipeline->set_depth_write_enabled(true);
	m_pipeline->set_depth_compare_op(gl::compare_op::greater);
	
	// Enable depth clamping ("pancaking")
	m_pipeline->set_depth_clamp_enabled(true);
	
	// Enable back-face culling
	m_pipeline->set_cull_mode(gl::cull_mode::back);
	bool two_sided = false;
	
	// Bind and clear shadow atlas framebuffer
	m_pipeline->bind_framebuffer(light.get_shadow_framebuffer().get());
	m_pipeline->clear_attachments(gl::depth_clear_bit, {});
	
	// Get camera
	const scene::camera& camera = *ctx.camera;
	
	// Get light shadow cascade distances and matrices
	const auto cascade_count = light.get_shadow_cascade_count();
	auto& cascade_distances = light.get_shadow_cascade_distances();
	const auto cascade_matrices = light.get_shadow_cascade_matrices();
	
	// Calculate cascade far clipping plane distances
	cascade_distances[cascade_count - 1] = light.get_shadow_max_distance();
	for (unsigned int i = 0; i < cascade_count - 1; ++i)
	{
		const float weight = static_cast<float>(i + 1) / static_cast<float>(cascade_count);
		
		// Calculate linear and logarithmic split distances
		const float linear_distance = math::lerp(camera.get_clip_near(), camera.get_clip_near() + light.get_shadow_max_distance(), weight);
		const float log_distance = math::log_lerp(camera.get_clip_near(), camera.get_clip_near() + light.get_shadow_max_distance(), weight);
		
		// Interpolate between linear and logarithmic split distances
		cascade_distances[i] = math::lerp(linear_distance, log_distance, light.get_shadow_cascade_distribution());
	}
	
	// Determine resolution of shadow atlas and cascades
	const auto atlas_resolution = static_cast<int>(light.get_shadow_framebuffer()->width());
	const auto cascade_resolution = atlas_resolution >> 1;
	
	// Sort render operations
	std::sort(std::execution::par_unseq, ctx.operations.begin(), ctx.operations.end(), operation_compare);
	
	gl::shader_program* active_shader_program = nullptr;
	
	for (unsigned int i = 0; i < cascade_count; ++i)
	{
		// Get distances to near and far clipping planes of camera subfrustum
		const auto subfrustum_near = i ? cascade_distances[i - 1] : camera.get_clip_near();
		const auto subfrustum_far = cascade_distances[i];
		
		// Find centroid of camera subfrustum
		const auto subfrustum_centroid = camera.get_translation() + camera.get_forward() * ((subfrustum_near + subfrustum_far) * 0.5f);
		
		// Construct light view matrix
		const auto light_view = math::look_at_rh(subfrustum_centroid, subfrustum_centroid + light.get_direction(), light.get_rotation() * math::fvec3{0, 1, 0});
		
		// Construct subfrustum inverse view-projection matrix
		const auto [subfrustum_projection, subfrustum_inv_projection] = math::perspective_half_z_inv(camera.get_vertical_fov(), camera.get_aspect_ratio(), subfrustum_far, subfrustum_near);
		const auto subfrustum_inv_view_projection = camera.get_inv_view() * subfrustum_inv_projection;
		
		// Construct matrix which transforms clip space coordinates to light view space
		const auto ndc_to_light_view = light_view * subfrustum_inv_view_projection;
		
		// Construct AABB containing subfrustum corners in light view space
		geom::box<float> light_projection_bounds = {math::inf<math::fvec3>, -math::inf<math::fvec3>};
		for (std::size_t j = 0; j < 8; ++j)
		{
			// Reverse half z clip-space coordinates of a cube
			constexpr math::fvec4 ndc_cube[8] =
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
			
			// Find light view space coordinates of subfrustum corner
			const auto corner = ndc_to_light_view * ndc_cube[j];
			
			// Expand light projection bounds to contain corner
			light_projection_bounds.extend(math::fvec3(corner) / corner[3]);
		}
		
		// Construct light projection matrix
		const auto light_projection = math::ortho_half_z
		(
			light_projection_bounds.min.x(), light_projection_bounds.max.x(),
			light_projection_bounds.min.y(), light_projection_bounds.max.y(),
			-light_projection_bounds.min.z(), -light_projection_bounds.max.z()
		);
		
		// Construct light view-projection matrix
		const auto light_view_projection = light_projection * light_view;
		const auto light_view_translation = math::fvec4(subfrustum_centroid);
		const auto light_view_rotation = math::fmat4(math::fmat3(light_view));
		
		// Update view-space to cascade texture-space transformation matrix
		{
			const auto vs_subfrustum_centroid = math::fvec3{0.0f, 0.0f, ((subfrustum_near + subfrustum_far) * -0.5f)};
			const auto vs_light_direction = light.get_direction() * camera.get_rotation();
			const auto vs_light_up = (light.get_rotation() * math::fvec3{0, 1, 0}) * camera.get_rotation();
			
			const auto vs_light_view = math::look_at_rh(vs_subfrustum_centroid, vs_subfrustum_centroid + vs_light_direction, vs_light_up);
			const auto vs_light_view_projection = light_projection * vs_light_view;
			
			cascade_matrices[i] = light.get_shadow_scale_bias_matrices()[i] * vs_light_view_projection;
		}
		
		// Queue render operations
		queue(ctx, light, light_view_projection);
		if (ctx.operations.empty())
		{
			continue;
		}
		
		// Set viewport for this cascade
		const gl::viewport viewport[1] =
		{{
			static_cast<float>(static_cast<int>(i % 2) * cascade_resolution),
			static_cast<float>(static_cast<int>(i >> 1) * cascade_resolution),
			static_cast<float>(cascade_resolution),
			static_cast<float>(cascade_resolution)
		}};
		m_pipeline->set_viewport(0, viewport);
		
		// Render geometry
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
						m_pipeline->set_cull_mode(gl::cull_mode::none);
					}
					else
					{
						m_pipeline->set_cull_mode(gl::cull_mode::back);
					}
					
					two_sided = material->is_two_sided();
				}
			}
			
			// Switch shader programs if necessary
			gl::shader_program* shader_program = (operation->skinning_matrices.empty()) ? m_static_mesh_shader_program.get() : m_skeletal_mesh_shader_program.get();
			if (active_shader_program != shader_program)
			{
				active_shader_program = shader_program;
				m_pipeline->bind_shader_program(active_shader_program);
			}
			
			// Calculate model-view-projection matrix
			// @see Persson, E., & Studios, A. (2012). Creating vast game worlds: Experiences from avalanche studios. In ACM SIGGRAPH 2012 Talks (pp. 1-1).
			auto model_view = operation->transform;
			model_view[3] -= light_view_translation;
			model_view = light_view_rotation * model_view;
			const auto model_view_projection = light_projection * model_view;
			
			// Upload operation-dependent parameters to shader program
			if (active_shader_program == m_static_mesh_shader_program.get())
			{
				m_static_mesh_model_view_projection_var->update(model_view_projection);
			}
			else if (active_shader_program == m_skeletal_mesh_shader_program.get())
			{
				m_skeletal_mesh_model_view_projection_var->update(model_view_projection);
				m_skeletal_mesh_skinning_matrices_var->update(operation->skinning_matrices);
			}
			
			// Draw geometry
			m_pipeline->set_primitive_topology(operation->primitive_topology);
			m_pipeline->bind_vertex_array(operation->vertex_array);
			m_pipeline->bind_vertex_buffers(0, {&operation->vertex_buffer, 1}, {&operation->vertex_offset, 1}, {&operation->vertex_stride, 1});
			m_pipeline->draw(operation->vertex_count, 1, 0, 0);
		}
	}
	
	// Disable depth clamping ("pancaking")
	m_pipeline->set_depth_clamp_enabled(false);
}

void cascaded_shadow_map_stage::rebuild_static_mesh_shader_program()
{
	m_static_mesh_shader_program = m_static_mesh_shader_template->build(m_shader_template_definitions);
	if (!m_static_mesh_shader_program->linked())
	{
		debug::log_error("Failed to build cascaded shadow map shader program for static meshes: {}", m_static_mesh_shader_program->info());
		debug::log_warning("{}", m_static_mesh_shader_template->configure(gl::shader_stage::vertex));
		
		m_static_mesh_model_view_projection_var = nullptr;
	}
	else
	{
		m_static_mesh_model_view_projection_var = m_static_mesh_shader_program->variable("model_view_projection");
	}
}

void cascaded_shadow_map_stage::rebuild_skeletal_mesh_shader_program()
{
	m_skeletal_mesh_shader_program = m_skeletal_mesh_shader_template->build(m_shader_template_definitions);
	if (!m_skeletal_mesh_shader_program->linked())
	{
		debug::log_error("Failed to build cascaded shadow map shader program for skeletal meshes: {}", m_skeletal_mesh_shader_program->info());
		debug::log_warning("{}", m_skeletal_mesh_shader_template->configure(gl::shader_stage::vertex));
		
		m_skeletal_mesh_model_view_projection_var = nullptr;
		m_skeletal_mesh_skinning_matrices_var = nullptr;
	}
	else
	{
		m_skeletal_mesh_model_view_projection_var = m_skeletal_mesh_shader_program->variable("model_view_projection");
		m_skeletal_mesh_skinning_matrices_var = m_skeletal_mesh_shader_program->variable("skinning_matrices");
	}
}

bool operation_compare(const render::operation* a, const render::operation* b)
{
	const bool skinned_a = !a->skinning_matrices.empty();
	const bool skinned_b = !b->skinning_matrices.empty();
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
