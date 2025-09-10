// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/utility/sized-types.hpp>

namespace engine::gl
{
	/// Enumerates all supported shader stages.
	enum class shader_stage: u8
	{
		/// Vertex shader stage.
		vertex,

		/// Fragment shader stage.
		fragment,

		/// Geometry shader stage.
		geometry
	};
}
