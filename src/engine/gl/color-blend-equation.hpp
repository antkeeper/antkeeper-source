// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_COLOR_BLEND_EQUATION_HPP
#define ANTKEEPER_GL_COLOR_BLEND_EQUATION_HPP

#include <engine/gl/blend-factor.hpp>
#include <engine/gl/blend-op.hpp>

namespace gl {

/// Color blend factors and operations.
struct color_blend_equation
{
	/// Selects which blend factor is used to determine the RGB source factors.
	blend_factor src_color_blend_factor;
	
	/// Selects which blend factor is used to determine the RGB destination factors.
	blend_factor dst_color_blend_factor;
	
	/// Selects which blend operation is used to calculate the RGB values to write to the color attachment.
	blend_op color_blend_op;
	
	/// Selects which blend factor is used to determine the alpha source factor.
	blend_factor src_alpha_blend_factor;
	
	/// Selects which blend factor is used to determine the alpha destination factor.
	blend_factor dst_alpha_blend_factor;
	
	/// Selects which blend operation is used to calculate the alpha values to write to the color attachment.
	blend_op alpha_blend_op;
};

} // namespace gl

#endif // ANTKEEPER_GL_COLOR_BLEND_EQUATION_HPP
