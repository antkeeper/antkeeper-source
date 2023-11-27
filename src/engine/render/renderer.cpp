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
#include <engine/scene/light-probe.hpp>
#include <engine/scene/text.hpp>
#include <engine/render/model.hpp>
#include <engine/math/matrix.hpp>
#include <engine/geom/projection.hpp>
#include <engine/config.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/math/numbers.hpp>
#include <functional>
#include <set>

namespace render {

renderer::renderer(gl::pipeline& pipeline, ::resource_manager& resource_manager)
{
	m_light_probe_stage = std::make_unique<render::light_probe_stage>(pipeline, resource_manager);
	m_cascaded_shadow_map_stage = std::make_unique<render::cascaded_shadow_map_stage>(pipeline, resource_manager);
	m_culling_stage = std::make_unique<render::culling_stage>();
	m_queue_stage = std::make_unique<render::queue_stage>();
}

void renderer::render(float t, float dt, float alpha, scene::collection& collection)
{
	// Init render context
	m_ctx.collection = &collection;
	m_ctx.t = t;
	m_ctx.dt = dt;
	m_ctx.alpha = alpha;
	
	// Execute light probe stage
	m_light_probe_stage->execute(m_ctx);
	
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
		m_ctx.camera = &camera;
		
		// Clear render queues
		m_ctx.objects.clear();
		m_ctx.operations.clear();
		
		// Execute cascaded shadow map stage
		m_cascaded_shadow_map_stage->execute(m_ctx);
		
		// Execute culling stage
		m_culling_stage->execute(m_ctx);
		
		// Execute queue stage
		m_queue_stage->execute(m_ctx);
		
		// Pass render context to the camera's compositor
		compositor->composite(m_ctx);
	}
}

} // namespace render
