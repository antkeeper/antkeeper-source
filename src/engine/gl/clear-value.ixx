// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.clear_value;
import engine.utility.sized_types;
export import <array>;

export namespace engine::gl
{
	/// Clear value.
	struct clear_value
	{
		/// Color clear values.
		std::array<float, 4> color{};

		/// Depth clear value.
		float depth{};

		/// Stencil clear value.
		u32 stencil{};
	};
}
