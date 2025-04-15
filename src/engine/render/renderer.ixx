// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.render.renderer;
import engine.resources.resource_manager;
import engine.scene.collection;
import engine.render.context;
import engine.render.light_probe_stage;
import engine.render.cascaded_shadow_map_stage;
import engine.render.culling_stage;
import engine.render.queue_stage;
import engine.gl.pipeline;
import <memory>;

export namespace engine::render
{
	/// High-level renderer.
	class renderer
	{
	public:
		/// Constructs a renderer.
		/// @param pipeline Graphics pipeline.
		/// @param resource_manager Resource manager for loading shader templates.
		renderer(gl::pipeline& pipeline, resources::resource_manager& resource_manager);
	
		/// Renders a collection of scene objects.
		/// @param t Current time, in seconds.
		/// @param dt Timestep, in seconds.
		/// @param alpha Subframe interpolation factor.
		/// @param collection Collection of scene objects to render.
		void render(float t, float dt, float alpha, scene::collection& collection);
	
	private:
		render::context m_ctx;
		std::unique_ptr<render::light_probe_stage> m_light_probe_stage;
		std::unique_ptr<render::cascaded_shadow_map_stage> m_cascaded_shadow_map_stage;
		std::unique_ptr<render::culling_stage> m_culling_stage;
		std::unique_ptr<render::queue_stage> m_queue_stage;
	};
}
