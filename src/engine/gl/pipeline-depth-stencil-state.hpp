// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
