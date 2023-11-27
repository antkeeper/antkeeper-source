/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

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
