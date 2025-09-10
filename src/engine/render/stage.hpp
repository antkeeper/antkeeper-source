// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/render/context.hpp>

namespace engine::render
{
	/// Abstract base class for a single stage in a render pipeline.
	class stage
	{
	public:
		/// Destructs a render stage.
		virtual ~stage() = default;
	
		/// Executes the render stage.
		/// @param ctx Render context.
		virtual void execute(render::context& ctx) = 0;
	};
}
