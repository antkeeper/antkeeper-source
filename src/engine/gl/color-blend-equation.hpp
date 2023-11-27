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
