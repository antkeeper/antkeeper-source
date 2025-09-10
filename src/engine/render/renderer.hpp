// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/resources/resource-manager.hpp>
#include <engine/scene/collection.hpp>
#include <engine/render/context.hpp>
#include <engine/render/stages/light-probe-stage.hpp>
#include <engine/render/stages/cascaded-shadow-map-stage.hpp>
#include <engine/render/stages/culling-stage.hpp>
#include <engine/render/stages/queue-stage.hpp>
#include <engine/gl/pipeline.hpp>
#include <memory>

namespace engine::render
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
