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

#include <engine/render/operation.hpp>
#include <engine/render/context.hpp>
#include <engine/render/queue.hpp>
#include <engine/render/stage/culling-stage.hpp>
#include <engine/gl/vertex-array.hpp>

namespace scene
{
	class collection;
	class object_base;
	class model_instance;
	class billboard;
	class lod_group;
	class text;
}

namespace render {

/**
 *
 */
class renderer
{
public:
	renderer();
	~renderer();
	
	/**
	 * Renders a collection of scene objects.
	 *
	 * @param t Current time, in seconds.
	 * @param dt Timestep, in seconds.
	 * @param alpha Subframe interpolation factor.
	 * @param collection Collection of scene objects to render.
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
	float4x4* skinning_palette;
	
	render::culling_stage* culling_stage;
};

} // namespace render

#endif // ANTKEEPER_RENDER_RENDERER_HPP
