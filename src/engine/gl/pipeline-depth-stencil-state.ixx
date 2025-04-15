// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.pipeline_depth_stencil_state;
export import engine.gl.compare_op;
export import engine.gl.stencil_op_state;

export namespace engine::gl
{
	/// Pipeline depth/stencil state.
	struct pipeline_depth_stencil_state
	{
		/// `true` if depth testing is enabled, `false` otherwise.
		bool depth_test_enabled{true};

		/// `true` if depth writes are enabled when depth testing is enabled, `false` otherwise.
		/// @note Depth writes are always disabled when depth testing is disabled.
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
}
