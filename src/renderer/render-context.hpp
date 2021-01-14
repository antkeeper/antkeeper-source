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

#include "renderer/render-operation.hpp"
#include "geom/plane.hpp"
#include "geom/bounding-volume.hpp"
#include "utility/fundamental-types.hpp"
#include "scene/camera.hpp"
#include "scene/collection.hpp"
#include <list>

struct render_context
{
	const scene::camera* camera;
	math::transform<float> camera_transform;
	float3 camera_forward;
	float3 camera_up;
	const geom::bounding_volume<float>* camera_culling_volume;
	geom::plane<float> clip_near;
	
	const scene::collection* collection;
	std::list<render_operation> operations;
	float alpha;
};

#endif // ANTKEEPER_RENDER_CONTEXT_HPP

