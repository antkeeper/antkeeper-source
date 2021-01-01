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

#include "renderer/renderer.hpp"
#include "renderer/render-context.hpp"
#include "renderer/compositor.hpp"
#include "scene/scene.hpp"
#include "scene/camera.hpp"
#include "scene/model-instance.hpp"
#include "scene/billboard.hpp"
#include "scene/lod-group.hpp"
#include "renderer/model.hpp"
#include "rasterizer/drawing-mode.hpp"
#include "math/math.hpp"
#include "geometry/projection.hpp"
#include "configuration.hpp"
#include <functional>
#include <set>

renderer::renderer()
{
	// Setup billboard render operation
	billboard_op.pose = nullptr;
	billboard_op.drawing_mode = drawing_mode::triangles;
	billboard_op.vertex_array = nullptr;
	billboard_op.start_index = 0;
	billboard_op.index_count = 6;
	billboard_op.instance_count = 0;
}

void renderer::render(float alpha, const scene& scene) const
{
	// Get list of all objects in the scene
	const std::list<scene_object_base*>* objects = scene.get_objects();
	
	// Build list of cameras to be sorted
	const std::list<scene_object_base*>* cameras = scene.get_objects(camera::object_type_id);
	std::list<camera*> sorted_cameras;
	for (scene_object_base* object: *cameras)
	{
		sorted_cameras.push_back(static_cast<camera*>(object));
	}
	
	// Sort cameras according to their respective compositing indices
	sorted_cameras.sort
	(
		[](const camera* a, const camera* b) -> bool
		{
			return a->get_composite_index() < b->get_composite_index();
		}
	);

	// Process cameras in order
	for (const camera* camera: sorted_cameras)
	{
		// Skip inactive cameras
		if (!camera->is_active())
		{
			continue;
		}
		
		// Skip cameras with no compositors
		const compositor* compositor = camera->get_compositor();
		if (!compositor)
		{
			continue;
		}
		
		// Setup render context
		render_context context;
		context.camera = camera;
		context.camera_transform = camera->get_transform_tween().interpolate(alpha); 
		context.camera_forward = context.camera_transform.rotation * global_forward;
		context.camera_up = context.camera_transform.rotation * global_up;
		context.clip_near = camera->get_view_frustum().get_near(); ///< TODO: tween this
		context.scene = &scene;
		context.alpha = alpha;
		
		// Get camera culling volume
		context.camera_culling_volume = camera->get_culling_mask();
		if (!context.camera_culling_volume)
			context.camera_culling_volume = &camera->get_bounds();
		
		// Generate render operations for each visible scene object
		for (const scene_object_base* object: *objects)
		{
			// Skip inactive objects
			if (!object->is_active())
				continue;
			
			// Process object
			process_object(context, object);
		}
		
		// Pass render context to the camera's compositor
		compositor->composite(&context);
	}
}

void renderer::set_billboard_vao(vertex_array* vao)
{
	billboard_op.vertex_array = vao;
}

void renderer::process_object(render_context& context, const scene_object_base* object) const
{
	std::size_t type = object->get_object_type_id();
	
	if (type == model_instance::object_type_id)
		process_model_instance(context, static_cast<const model_instance*>(object));
	else if (type == billboard::object_type_id)		
		process_billboard(context, static_cast<const billboard*>(object));
	else if (type == lod_group::object_type_id)
		process_lod_group(context, static_cast<const lod_group*>(object));
}

void renderer::process_model_instance(render_context& context, const ::model_instance* model_instance) const
{
	const model* model = model_instance->get_model();
	if (!model)
		return;
	
	// Get object culling volume
	const bounding_volume<float>* object_culling_volume = model_instance->get_culling_mask();
	if (!object_culling_volume)
		object_culling_volume = &model_instance->get_bounds();
	
	// Perform view-frustum culling
	if (!context.camera_culling_volume->intersects(*object_culling_volume))
		return;
	
	const std::vector<material*>* instance_materials = model_instance->get_materials();
	const std::vector<model_group*>* groups = model->get_groups();

	for (model_group* group: *groups)
	{
		render_operation operation;

		// Determine operation material
		operation.material = group->get_material();
		if ((*instance_materials)[group->get_index()])
		{
			// Override model group material with the instance's material 
			operation.material = (*instance_materials)[group->get_index()];
		}

		operation.pose = model_instance->get_pose();
		operation.vertex_array = model->get_vertex_array();
		operation.drawing_mode = group->get_drawing_mode();
		operation.start_index = group->get_start_index();
		operation.index_count = group->get_index_count();
		operation.transform = math::matrix_cast(model_instance->get_transform_tween().interpolate(context.alpha));
		operation.depth = context.clip_near.signed_distance(math::resize<3>(operation.transform[3]));
		operation.instance_count = model_instance->get_instance_count();

		context.operations.push_back(operation);
	}
}

void renderer::process_billboard(render_context& context, const ::billboard* billboard) const
{
	// Get object culling volume
	const bounding_volume<float>* object_culling_volume = billboard->get_culling_mask();
	if (!object_culling_volume)
		object_culling_volume = &billboard->get_bounds();
	
	// Perform view-frustum culling
	if (!context.camera_culling_volume->intersects(*object_culling_volume))
		return;
	
	math::transform<float> billboard_transform = billboard->get_transform_tween().interpolate(context.alpha);
	billboard_op.material = billboard->get_material();
	billboard_op.depth = context.clip_near.signed_distance(math::resize<3>(billboard_transform.translation));
	
	// Align billboard
	if (billboard->get_billboard_type() == billboard_type::spherical)
	{
		billboard_transform.rotation = math::normalize(math::look_rotation(context.camera_forward, context.camera_up) * billboard_transform.rotation);
	}
	else if (billboard->get_billboard_type() == billboard_type::cylindrical)
	{
		const float3& alignment_axis = billboard->get_alignment_axis();
		float3 look = math::normalize(project_on_plane(billboard_transform.translation - context.camera_transform.translation, {0.0f, 0.0f, 0.0f}, alignment_axis));
		float3 right = math::normalize(math::cross(alignment_axis, look));
		look = math::cross(right, alignment_axis);
		float3 up = math::cross(look, right);
		billboard_transform.rotation = math::normalize(math::look_rotation(look, up) * billboard_transform.rotation);
	}
	
	billboard_op.transform = math::matrix_cast(billboard_transform);
	
	context.operations.push_back(billboard_op);
}

void renderer::process_lod_group(render_context& context, const ::lod_group* lod_group) const
{
	// Select level of detail
	std::size_t level = lod_group->select_lod(*context.camera);
	
	// Process all objects in the group with the selected level of detail
	const std::list<scene_object_base*>& objects = lod_group->get_objects(level);
	for (const scene_object_base* object: objects)
	{
		process_object(context, object);
	}
}
