// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/render/stages/culling-stage.hpp>
#include <engine/scene/camera.hpp>
#include <engine/scene/collection.hpp>
#include <algorithm>
#include <execution>
#include <mutex>

namespace render {

void culling_stage::execute(render::context& ctx)
{
	// Get all objects in the collection
	const auto& objects = ctx.collection->get_objects();
	
	// Get camera layer mask and view frustum
	const auto camera_layer_mask = ctx.camera->get_layer_mask();
	const auto& view_frustum = ctx.camera->get_view_frustum();
	
	// Construct mutex to guard set of visible objects
	std::mutex mutex;
	
	// For each object in the scene collection
	std::for_each
	(
		std::execution::par,
		std::begin(objects),
		std::end(objects),
		[&](scene::object_base* object)
		{
			// Ignore cameras
			if (object->get_object_type_id() == scene::camera::object_type_id)
			{
				return;
			}
			
			// Cull object if it doesn't share any common layers with the camera
			if (!(object->get_layer_mask() & camera_layer_mask))
			{
				return;
			}
			
			// Cull object if it's outside of the camera view frustum
			if (!view_frustum.intersects(object->get_bounds()))
			{
				return;
			}
			
			// Insert object into set of visible objects
			std::lock_guard<std::mutex> guard(mutex);
			ctx.objects.push_back(object);
		}
	);
}

} // namespace render
