/*
 * Copyright (C) 2020  Christopher J. Howard
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
#include "geometry/plane.hpp"
#include "geometry/bounding-volume.hpp"
#include <vmq/vmq.hpp>
#include <list>

class camera;
class scene;

using namespace vmq::types;
using vmq::transform;

struct render_context
{
	const camera* camera;
	transform<float> camera_transform;
	float3 camera_forward;
	float3 camera_up;
	const bounding_volume<float>* camera_culling_volume;
	plane<float> clip_near;
	
	const scene* scene;
	std::list<render_operation> operations;
	float alpha;
};

#endif // ANTKEEPER_RENDER_CONTEXT_HPP

