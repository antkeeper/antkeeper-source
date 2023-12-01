// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
