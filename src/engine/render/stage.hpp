// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RENDER_STAGE_HPP
#define ANTKEEPER_RENDER_STAGE_HPP

#include <engine/render/context.hpp>

namespace render {

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

} // namespace render

#endif // ANTKEEPER_RENDER_STAGE_HPP
