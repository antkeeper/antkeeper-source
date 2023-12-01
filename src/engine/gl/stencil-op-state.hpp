// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_STENCIL_OP_STATE_HPP
#define ANTKEEPER_GL_STENCIL_OP_STATE_HPP

#include <engine/gl/compare-op.hpp>
#include <engine/gl/stencil-op.hpp>
#include <cstdint>

namespace gl {

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
	std::uint32_t compare_mask{0xffffffff};
	
	/// Bits of the unsigned integer stencil values updated by the stencil test in the stencil framebuffer attachment.
	std::uint32_t write_mask{0xffffffff};
	
	/// Stencil reference value that is used in the unsigned stencil comparison.
	std::uint32_t reference{0};
};

} // namespace gl

#endif // ANTKEEPER_GL_STENCIL_OP_STATE_HPP
