/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_RENDERER_HPP
#define ANTKEEPER_RENDERER_HPP

#include "renderer/operation.hpp"
#include "renderer/context.hpp"
#include "renderer/queue.hpp"
#include "gl/vertex-array.hpp"

namespace scene
{
	class collection;
	class object_base;
	class model_instance;
	class billboard;
	class lod_group;
	class text;
}

/*
# Pipeline

1. A scene containing meshes, lights, and cameras is passed to renderer::render().
2. Each camera is processed in order of priority.
3. Scene objects are tested for visibility against camera's view frustum.
4. List of visible scene objects are passed to the camera's compositor.
5. Compositor passes the visible scene objects to each render pass
6. Render pass sorts scene objects according to its own rules, then rasterizes to its render target.
*/

/**
 *
 */
class renderer
{
public:
	renderer();
	
	/**
	 * Renders a collection of scene objects.
	 *
	 * @param t Current time, in seconds.
	 * @param dt Timestep, in seconds.
	 * @param alpha Subframe interpolation factor.
	 * @parma collection Collection of scene objects to render.
	 */
	void render(float t, float dt, float alpha, const scene::collection& collection) const;
	
	/**
	 * Sets the VAO to be used when generating render operations for billboards.
	 */
	void set_billboard_vao(gl::vertex_array* vao);
	
private:
	void process_object(const render::context& ctx, render::queue& queue, const scene::object_base* object) const;
	void process_model_instance(const render::context& ctx, render::queue& queue, const scene::model_instance* model_instance) const;
	void process_billboard(const render::context& ctx, render::queue& queue, const scene::billboard* billboard) const;
	void process_lod_group(const render::context& ctx, render::queue& queue, const scene::lod_group* lod_group) const;
	void process_text(const render::context& ctx, render::queue& queue, const scene::text* text) const;

	mutable render::operation billboard_op;
};

#endif // ANTKEEPER_RENDERER_HPP
