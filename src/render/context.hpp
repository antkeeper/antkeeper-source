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

#ifndef ANTKEEPER_RENDER_CONTEXT_HPP
#define ANTKEEPER_RENDER_CONTEXT_HPP

#include "geom/plane.hpp"
#include "geom/bounding-volume.hpp"
#include "utility/fundamental-types.hpp"
#include "math/transform-operators.hpp"
#include <list>

namespace scene
{
	class camera;
	class collection;
}

namespace render {

/**
 * Context of a renderer.
 */
struct context
{
	/// Pointer to the camera.
	const scene::camera* camera;
	
	/// Camera transform.
	math::transform<float> camera_transform;
	
	/// Camera forward vector
	float3 camera_forward;
	
	/// Camera up vector.
	float3 camera_up;
	
	/// Camera culling volume.
	const geom::bounding_volume<float>* camera_culling_volume;
	
	/// Near clipping plane of the camera.
	geom::plane<float> clip_near;
	
	/// Camera view matrix.
	float4x4 view;
	
	/// Camera projection matrix.
	float4x4 projection;
	
	/// Camera view projection matrix.
	float4x4 view_projection;
	
	/// Camera exposure normalization factor.
	float exposure;
	
	/// Collection of scene objects being rendered.
	const scene::collection* collection;
	
	/// Current time, in seconds.
	float t;
	
	/// Timestep, in seconds.
	float dt;
	
	/// Subframe interpolation factor.
	float alpha;
};

} // namespace render

#endif // ANTKEEPER_RENDER_CONTEXT_HPP
