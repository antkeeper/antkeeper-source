// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/gl/viewport.hpp>
#include <engine/gl/scissor-region.hpp>
#include <vector>

namespace engine::gl
{
	/// Pipeline viewport state.
	struct pipeline_viewport_state
	{
		/// Active viewports.
		std::vector<viewport> viewports;

		/// Active scissor regions.
		std::vector<scissor_region> scissors;
	};
}
