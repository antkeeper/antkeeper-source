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
