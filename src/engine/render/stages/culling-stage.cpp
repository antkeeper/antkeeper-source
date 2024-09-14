// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/render/stages/culling-stage.hpp>
#include <engine/scene/camera.hpp>
#include <engine/scene/collection.hpp>

namespace render {

void culling_stage::execute(render::context& ctx)
{
	// Get all objects in the collection
	const auto& objects = ctx.collection->get_objects();
	
	// Get camera layer mask and view frustum
	const auto camera_layer_mask = ctx.camera->get_layer_mask();
	const auto& view_frustum = ctx.camera->get_view_frustum();
	
	// For each object in the scene collection
	for (const auto& object: objects)
	{
		// Ignore cameras
		if (object->get_object_type_id() == scene::camera::object_type_id)
		{
			continue;
		}
		
		// Cull object if it doesn't share any common layers with the camera
		if (!(object->get_layer_mask() & camera_layer_mask))
		{
			continue;
		}
		
		// Cull object if it's outside of the camera view frustum
		if (!view_frustum.intersects(object->get_bounds()))
		{
			continue;
		}
		
		// Insert object into set of visible objects
		ctx.objects.push_back(object);
	}
}

} // namespace render
