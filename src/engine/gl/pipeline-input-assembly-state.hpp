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

#ifndef ANTKEEPER_GL_PIPELINE_INPUT_ASSEMBLY_STATE_HPP
#define ANTKEEPER_GL_PIPELINE_INPUT_ASSEMBLY_STATE_HPP

#include <engine/gl/primitive-topology.hpp>
#include <cstdlib>

namespace gl {

/// Pipeline input assembly state.
struct pipeline_input_assembly_state
{
	/// Primitive topology.
	primitive_topology topology{primitive_topology::triangle_list};
	
	/// Controls whether a special vertex index value is treated as restarting the assembly of primitives.
	bool primitive_restart_enabled{false};
};

} // namespace gl

#endif // ANTKEEPER_GL_PIPELINE_INPUT_ASSEMBLY_STATE_HPP
