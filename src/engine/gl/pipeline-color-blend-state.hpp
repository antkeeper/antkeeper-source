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

#ifndef ANTKEEPER_GL_PIPELINE_COLOR_BLEND_STATE_HPP
#define ANTKEEPER_GL_PIPELINE_COLOR_BLEND_STATE_HPP

#include <engine/gl/logic-op.hpp>
#include <engine/gl/color-blend-equation.hpp>
#include <array>
#include <cstdlib>

namespace gl {

/// Pipeline color blend state.
struct pipeline_color_blend_state
{
	/// Controls whether to apply logical operations.
	bool logic_op_enabled{false};
	
	/// Selects which logical operation to apply.
	gl::logic_op logic_op{gl::logic_op::bitwise_copy};
	
	/// Controls whether blending is enabled for the corresponding color attachment.
	bool blend_enabled{false};
	
	/// Color blend factors and operations.
	gl::color_blend_equation color_blend_equation
	{
		blend_factor::one,
		blend_factor::zero,
		blend_op::add,
		blend_factor::one,
		blend_factor::zero,
		blend_op::add
	};
	
	/// Bitmask indicating which of the RGBA components are enabled for writing.
	std::uint8_t color_write_mask{0b1111};
	
	/// RGBA components of the blend constant that are used in blending, depending on the blend factor.
	std::array<float, 4> blend_constants{0.0f, 0.0f, 0.0f, 0.0f};
};

} // namespace gl

#endif // ANTKEEPER_GL_PIPELINE_COLOR_BLEND_STATE_HPP
