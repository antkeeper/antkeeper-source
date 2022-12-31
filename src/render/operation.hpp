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

#ifndef ANTKEEPER_RENDER_OPERATION_HPP
#define ANTKEEPER_RENDER_OPERATION_HPP

#include "utility/fundamental-types.hpp"
#include "animation/pose.hpp"
#include "gl/vertex-array.hpp"
#include "gl/drawing-mode.hpp"
#include <cstddef>

namespace render {

class material;

/**
 * Encapsulates an atomic render operation.
 */
struct operation
{
	const float4x4* skinning_palette;
	std::size_t bone_count;
	const material* material;
	const gl::vertex_array* vertex_array;
	gl::drawing_mode drawing_mode;
	std::size_t start_index;
	std::size_t index_count;
	float4x4 transform;
	float depth;
	std::size_t instance_count;
};

} // namespace render

#endif // ANTKEEPER_RENDER_OPERATION_HPP
