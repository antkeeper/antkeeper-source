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

#ifndef ANTKEEPER_RENDER_RENDERER_HPP
#define ANTKEEPER_RENDER_RENDERER_HPP

#include <engine/render/context.hpp>
#include <engine/render/stages/culling-stage.hpp>
#include <engine/render/stages/queue-stage.hpp>
#include <engine/scene/collection.hpp>
#include <memory>

namespace render {

/**
 *
 */
class renderer
{
public:
	/**
	 * Constructs a renderer.
	 */
	renderer();
	
	/**
	 * Renders a collection of scene objects.
	 *
	 * @param t Current time, in seconds.
	 * @param dt Timestep, in seconds.
	 * @param alpha Subframe interpolation factor.
	 * @param collection Collection of scene objects to render.
	 */
	void render(float t, float dt, float alpha, const scene::collection& collection);
	
private:
	render::context ctx;
	
	std::unique_ptr<render::culling_stage> culling_stage;
	std::unique_ptr<render::queue_stage> queue_stage;
};

} // namespace render

#endif // ANTKEEPER_RENDER_RENDERER_HPP
