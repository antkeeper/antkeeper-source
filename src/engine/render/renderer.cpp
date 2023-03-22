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
#include <engine/scene/static-mesh.hpp>
#include <engine/scene/billboard.hpp>
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
	culling_stage = std::make_unique<render::culling_stage>();
	queue_stage = std::make_unique<render::queue_stage>();
}

void renderer::render(float t, float dt, float alpha, const scene::collection& collection)
{
	// Init render context
	ctx.collection = &collection;
	ctx.t = t;
	ctx.dt = dt;
	ctx.alpha = alpha;
	
	// Get list of cameras to be sorted
	const auto& cameras = collection.get_objects(scene::camera::object_type_id);
	
	// Process cameras in order
	for (scene::object_base* camera_object: cameras)
	{
		scene::camera& camera = static_cast<scene::camera&>(*camera_object);
		
		// Skip cameras with no compositors
		compositor* compositor = camera.get_compositor();
		if (!compositor)
		{
			continue;
		}
		
		// Update render context camera
		ctx.camera = &camera;
		
		// Clear render queues
		ctx.objects.clear();
		ctx.operations.clear();
		
		// Execute culling stage
		culling_stage->execute(ctx);
		
		// Execute queue stage
		queue_stage->execute(ctx);
		
		// Pass render context to the camera's compositor
		compositor->composite(ctx);
	}
}

} // namespace render
