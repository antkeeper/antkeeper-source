// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.stencil_op_state;
export import engine.gl.stencil_op;
export import engine.gl.compare_op;
import engine.utility.sized_types;

export namespace engine::gl
{
	/// Stencil operation state.
	struct stencil_op_state
	{
		/// Action performed on samples that fail the stencil test.
		stencil_op fail_op{stencil_op::keep};

		/// Action performed on samples that pass both the depth and stencil tests.
		stencil_op pass_op{stencil_op::keep};

		/// Action performed on samples that pass the stencil test and fail the depth test.
		stencil_op depth_fail_op{stencil_op::keep};

		/// Comparison operator used in the stencil test.
		gl::compare_op compare_op{gl::compare_op::always};

		/// Bits of the unsigned integer stencil values participating in the stencil test.
		u32 compare_mask{0xffffffff};

		/// Bits of the unsigned integer stencil values updated by the stencil test in the stencil framebuffer attachment.
		u32 write_mask{0xffffffff};

		/// Stencil reference value that is used in the unsigned stencil comparison.
		u32 reference{0};
	};
}
