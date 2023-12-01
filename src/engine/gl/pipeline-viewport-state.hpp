// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_PIPELINE_VIEWPORT_STATE_HPP
#define ANTKEEPER_GL_PIPELINE_VIEWPORT_STATE_HPP

#include <engine/gl/viewport.hpp>
#include <engine/gl/scissor-region.hpp>
#include <vector>

namespace gl {

/// Pipeline viewport state.
struct pipeline_viewport_state
{
	/// Active viewports.
	std::vector<viewport> viewports;
	
	/// Active scissor regions.
	std::vector<scissor_region> scissors;
};

} // namespace gl

#endif // ANTKEEPER_GL_PIPELINE_VIEWPORT_STATE_HPP
