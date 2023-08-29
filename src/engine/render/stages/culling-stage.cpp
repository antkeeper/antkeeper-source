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
