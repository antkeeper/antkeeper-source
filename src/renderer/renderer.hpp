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

#include "render-operation.hpp"

struct render_context;
class vertex_array;

namespace scene
{
	class collection;
	class object_base;
	class model_instance;
	class billboard;
	class lod_group;
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
	 * @param alpha Subframe interpolation factor.
	 * @parma collection Collection of scene objects to render.
	 */
	void render(float alpha, const scene::collection& collection) const;
	
	/**
	 * Sets the VAO to be used when generating render operations for billboards.
	 */
	void set_billboard_vao(vertex_array* vao);
	
private:
	void process_object(render_context& context, const scene::object_base* object) const;
	void process_model_instance(render_context& context, const scene::model_instance* model_instance) const;
	void process_billboard(render_context& context, const scene::billboard* billboard) const;
	void process_lod_group(render_context& context, const scene::lod_group* lod_group) const;

	mutable render_operation billboard_op;
};

#endif // ANTKEEPER_RENDERER_HPP

