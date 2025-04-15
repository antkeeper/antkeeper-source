// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.pipeline_viewport_state;
export import engine.gl.viewport;
export import engine.gl.scissor_region;
export import <vector>;

export namespace engine::gl
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
