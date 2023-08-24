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

#ifndef ANTKEEPER_GAME_AUTOFOCUS_COMPONENT_HPP
#define ANTKEEPER_GAME_AUTOFOCUS_COMPONENT_HPP

#include <engine/math/vector.hpp>
#include <engine/math/angles.hpp>
#include <engine/physics/spring.hpp>

/**
 * Modulates a camera's field of view and spring arm length.
 */
struct autofocus_component
{
	/// Height of the focal plane at maximum zoom.
	double near_focal_plane_height{1.0};
	
	/// Height of the focal plane at minimum zoom.
	double far_focal_plane_height{50.0};
	
	/// Horizontal FoV at maximum zoom.
	double near_hfov{math::radians(90.0)};
	
	/// Horizontal FoV at minimum zoom.
	double far_hfov{math::radians(45.0)};
	
	/// Zoom factor, on `[0, 1]`.
	double zoom{};
	
	/// Dimensions of the focal plane.
	math::dvec2 focal_plane_size;
	
	/// Horizontal FoV of the camera, in radians.
	double hfov{};
	
	/// Vertical FoV of the camera, in radians.
	double vfov{};
	
	/// Position of the focal point, relative to the subject.
	math::dvec3 focal_point_offset{};
};

#endif // ANTKEEPER_GAME_AUTOFOCUS_COMPONENT_HPP
