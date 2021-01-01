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

#ifndef ANTKEEPER_RENDER_OPERATION_HPP
#define ANTKEEPER_RENDER_OPERATION_HPP

#include "utility/fundamental-types.hpp"
#include <cstdlib>

class pose;
class material;
class vertex_array;
enum class drawing_mode;

/**
 * Describes a render operation with a single mesh and single material.
 */
struct render_operation
{
	const pose* pose;
	const material* material;
	const vertex_array* vertex_array;
	drawing_mode drawing_mode;
	std::size_t start_index;
	std::size_t index_count;
	float4x4 transform;
	float depth;
	std::size_t instance_count;
};

#endif // ANTKEEPER_RENDER_OPERATION_HPP

