// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/gl/logic-op.hpp>
#include <engine/gl/color-blend-equation.hpp>
#include <engine/utility/sized-types.hpp>
#include <array>

namespace engine::gl
{
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
		u8 color_write_mask{0b1111};

		/// RGBA components of the blend constant that are used in blending, depending on the blend factor.
		std::array<float, 4> blend_constants{0.0f, 0.0f, 0.0f, 0.0f};
	};
}
