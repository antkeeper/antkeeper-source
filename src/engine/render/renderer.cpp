// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.render.renderer;
import engine.render.light_probe_stage;
import engine.render.cascaded_shadow_map_stage;
import engine.render.culling_stage;
import engine.render.queue_stage;
import engine.render.compositor;
import engine.scene.collection;
import engine.scene.camera;

namespace engine::render
{
	renderer::renderer(gl::pipeline& pipeline, resources::resource_manager& resource_manager)
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
}
