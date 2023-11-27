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

#ifndef ANTKEEPER_GL_PIPELINE_DEPTH_STENCIL_STATE_HPP
#define ANTKEEPER_GL_PIPELINE_DEPTH_STENCIL_STATE_HPP

#include <engine/gl/compare-op.hpp>
#include <engine/gl/stencil-op-state.hpp>
#include <cstdlib>

namespace gl {

/// Pipeline depth/stencil state.
struct pipeline_depth_stencil_state
{
	/// `true` if depth testing is enabled, `false` otherwise.
	bool depth_test_enabled{true};
	
	/**
	 * `true` if depth writes are enabled when depth testing is enabled, `false` otherwise.
	 *
	 * @note Depth writes are always disabled when depth testing is disabled.
	 */
	bool depth_write_enabled{true};
	
	/// Comparison operator to use in the depth comparison step of the depth test.
	compare_op depth_compare_op{compare_op::less};
	
	/// `true` if stencil testing is enabled, `false` otherwise.
	bool stencil_test_enabled{false};
	
	/// Stencil testing parameters for front faces.
	stencil_op_state stencil_front;
	
	/// Stencil testing parameters for back faces.
	stencil_op_state stencil_back;
};

} // namespace gl

#endif // ANTKEEPER_GL_PIPELINE_DEPTH_STENCIL_STATE_HPP
