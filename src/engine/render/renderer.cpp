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

#include <engine/render/renderer.hpp>
#include <engine/render/context.hpp>
#include <engine/render/compositor.hpp>
#include <engine/scene/collection.hpp>
#include <engine/scene/camera.hpp>
#include <engine/scene/model-instance.hpp>
#include <engine/scene/billboard.hpp>
#include <engine/scene/lod-group.hpp>
#include <engine/scene/text.hpp>
#include <engine/render/model.hpp>
#include <engine/gl/drawing-mode.hpp>
#include <engine/math/matrix.hpp>
#include <engine/geom/projection.hpp>
#include <engine/config.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/math/numbers.hpp>
#include <functional>
#include <set>

namespace render {

renderer::renderer()
{
	// Setup billboard render operation
	billboard_op.bone_count = 0;
	billboard_op.skinning_palette = nullptr;
	billboard_op.drawing_mode = gl::drawing_mode::triangles;
	billboard_op.vertex_array = nullptr;
	billboard_op.start_index = 0;
	billboard_op.index_count = 6;
	billboard_op.instance_count = 0;
	
	// Allocate skinning palette
	skinning_palette = new float4x4[MATERIAL_PASS_MAX_BONE_COUNT];
	
	// Construct culling stage
	culling_stage = new render::culling_stage();
}

renderer::~renderer()
{
	delete[] skinning_palette;
	delete culling_stage;
}

void renderer::render(float t, float dt, float alpha, const scene::collection& collection) const
{
	// Get list of all objects in the collection
	const std::list<scene::object_base*>* objects = collection.get_objects();
	
	// Build list of cameras to be sorted
	const std::list<scene::object_base*>* cameras = collection.get_objects(scene::camera::object_type_id);
	std::list<scene::camera*> sorted_cameras;
	for (scene::object_base* object: *cameras)
	{
		sorted_cameras.push_back(static_cast<scene::camera*>(object));
	}
	
	// Sort cameras according to their respective compositing indices
	sorted_cameras.sort
	(
		[](const scene::camera* a, const scene::camera* b) -> bool
		{
			return a->get_composite_index() < b->get_composite_index();
		}
	);
	
	// Init render context
	render::context ctx;
	ctx.collection = &collection;
	ctx.t = t;
	ctx.dt = dt;
	ctx.alpha = alpha;

	// Process cameras in order
	for (const scene::camera* camera: sorted_cameras)
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
		
		// Update render context with camera parameters
		ctx.camera = camera;
		ctx.camera_transform = camera->get_transform_tween().interpolate(alpha); 
		ctx.camera_forward = ctx.camera_transform.rotation * config::global_forward;
		ctx.camera_up = ctx.camera_transform.rotation * config::global_up;
		ctx.clip_near = camera->get_view_frustum().get_near(); ///< @TODO: tween this
		ctx.view = camera->get_view_tween().interpolate(alpha);
		ctx.projection = camera->get_projection_tween().interpolate(alpha);
		ctx.view_projection = ctx.projection * ctx.view;
		ctx.exposure = std::exp2(-camera->get_exposure_tween().interpolate(alpha));
		
		// Execute culling stage
		culling_stage->execute(ctx);
		
		// Create render queue
		render::queue queue;
		
		// Queue render operations for each visible scene object
		for (const scene::object_base* object: ctx.visible_objects)
		{
			// Process object
			process_object(ctx, queue, object);
		}
		
		// Pass render context to the camera's compositor
		compositor->composite(ctx, queue);
	}
}

void renderer::set_billboard_vao(gl::vertex_array* vao)
{
	billboard_op.vertex_array = vao;
}

void renderer::process_object(const render::context& ctx, render::queue& queue, const scene::object_base* object) const
{
	std::size_t type = object->get_object_type_id();
	
	if (type == scene::model_instance::object_type_id)
		process_model_instance(ctx, queue, static_cast<const scene::model_instance*>(object));
	else if (type == scene::billboard::object_type_id)		
		process_billboard(ctx, queue, static_cast<const scene::billboard*>(object));
	else if (type == scene::lod_group::object_type_id)
		process_lod_group(ctx, queue, static_cast<const scene::lod_group*>(object));
	else if (type == scene::text::object_type_id)
		process_text(ctx, queue, static_cast<const scene::text*>(object));
}

void renderer::process_model_instance(const render::context& ctx, render::queue& queue, const scene::model_instance* model_instance) const
{
	const model* model = model_instance->get_model();
	if (!model)
		return;
	
	const std::vector<material*>* instance_materials = model_instance->get_materials();
	const std::vector<model_group*>* groups = model->get_groups();

	render::operation operation;
	operation.transform = math::matrix_cast(model_instance->get_transform_tween().interpolate(ctx.alpha));
	operation.depth = ctx.clip_near.signed_distance(float3(operation.transform[3]));
	operation.vertex_array = model->get_vertex_array();
	operation.instance_count = model_instance->get_instance_count();
	
	// Skinning parameters
	operation.bone_count = model_instance->get_pose().size();
	if (operation.bone_count)
	{
		operation.skinning_palette = skinning_palette;
		::matrix_palette(model->get_skeleton().inverse_bind_pose, model_instance->get_pose(), skinning_palette);
	}
	else
	{
		operation.skinning_palette = nullptr;
	}
	
	for (model_group* group: *groups)
	{
		// Determine operation material
		operation.material = group->get_material();
		if ((*instance_materials)[group->get_index()])
		{
			// Override model group material with the instance's material 
			operation.material = (*instance_materials)[group->get_index()];
		}
		
		operation.drawing_mode = group->get_drawing_mode();
		operation.start_index = group->get_start_index();
		operation.index_count = group->get_index_count();
		
		queue.push_back(operation);
	}
}

void renderer::process_billboard(const render::context& ctx, render::queue& queue, const scene::billboard* billboard) const
{
	math::transform<float> billboard_transform = billboard->get_transform_tween().interpolate(ctx.alpha);
	billboard_op.material = billboard->get_material();
	billboard_op.depth = ctx.clip_near.signed_distance(float3(billboard_transform.translation));
	
	// Align billboard
	if (billboard->get_billboard_type() == scene::billboard_type::spherical)
	{
		billboard_transform.rotation = math::normalize(math::look_rotation(ctx.camera_forward, ctx.camera_up) * billboard_transform.rotation);
	}
	else if (billboard->get_billboard_type() == scene::billboard_type::cylindrical)
	{
		const float3& alignment_axis = billboard->get_alignment_axis();
		float3 look = math::normalize(geom::project_on_plane(billboard_transform.translation - ctx.camera_transform.translation, {0.0f, 0.0f, 0.0f}, alignment_axis));
		float3 right = math::normalize(math::cross(alignment_axis, look));
		look = math::cross(right, alignment_axis);
		float3 up = math::cross(look, right);
		billboard_transform.rotation = math::normalize(math::look_rotation(look, up) * billboard_transform.rotation);
	}
	
	billboard_op.transform = math::matrix_cast(billboard_transform);
	
	queue.push_back(billboard_op);
}

void renderer::process_lod_group(const render::context& ctx, render::queue& queue, const scene::lod_group* lod_group) const
{
	// Select level of detail
	std::size_t level = lod_group->select_lod(*ctx.camera);
	
	// Process all objects in the group with the selected level of detail
	const std::list<scene::object_base*>& objects = lod_group->get_objects(level);
	for (const scene::object_base* object: objects)
	{
		process_object(ctx, queue, object);
	}
}

void renderer::process_text(const render::context& ctx, render::queue& queue, const scene::text* text) const
{
	text->render(ctx, queue);
}

} // namespace render
