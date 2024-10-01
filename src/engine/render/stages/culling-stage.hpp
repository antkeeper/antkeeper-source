// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RENDER_CULLING_STAGE_HPP
#define ANTKEEPER_RENDER_CULLING_STAGE_HPP

#include <engine/render/stage.hpp>

namespace render {

/// Builds a set of scene objects visible to the current camera and stores it in the render context.
class culling_stage: public stage
{
public:
	/// Destructs a culling stage.
	~culling_stage() override = default;
	
	void execute(render::context& ctx) override;
};

} // namespace render

#endif // ANTKEEPER_RENDER_CULLING_STAGE_HPP
