// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.color_blend_equation;
export import engine.gl.blend_factor;
export import engine.gl.blend_op;

export namespace engine::gl
{
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
}
