// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
#include <engine/math/constants.hpp>
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
