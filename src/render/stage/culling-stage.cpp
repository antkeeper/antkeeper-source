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

#include "render/stage/culling-stage.hpp"
#include "scene/camera.hpp"
#include "scene/collection.hpp"
#include <algorithm>
#include <execution>
#include <mutex>

namespace render {

void culling_stage::execute(render::context& ctx)
{
	// Get list of all objects in the collection
	const std::list<scene::object_base*>& objects = *(ctx.collection->get_objects());
	
	// Get camera culling volume
	ctx.camera_culling_volume = ctx.camera->get_culling_mask();
	if (!ctx.camera_culling_volume)
		ctx.camera_culling_volume = &ctx.camera->get_world_bounds();
	
	// Clear set of visible objects
	ctx.visible_objects.clear();
	
	// Construct mutex to guard set of visible objects
	std::mutex mutex;
	
	// For each object in the scene collection
	std::for_each
	(
		std::execution::par_unseq,
		std::begin(objects),
		std::end(objects),
		[&](scene::object_base* object)
		{
			// Ignore inactive objects and cameras
			if (!object->is_active() || object->get_object_type_id() == scene::camera::object_type_id)
				return;
			
			// Cull object if it doesn't share any common layers with the camera
			//if (!(object->get_layer_mask() & camera_layer_mask))
			//	return;
			
			// Get object culling volume
			const geom::bounding_volume<float>* object_culling_volume = object->get_culling_mask();
			if (!object_culling_volume)
				object_culling_volume = &object->get_world_bounds();
			
			// Cull object if it's outside of the camera culling volume
			if (!ctx.camera_culling_volume->intersects(*object_culling_volume))
				return;
			
			// Insert object into set of visible objects
			std::lock_guard<std::mutex> guard(mutex);
			ctx.visible_objects.push_back(object);
		}
	);
}

} // namespace render
