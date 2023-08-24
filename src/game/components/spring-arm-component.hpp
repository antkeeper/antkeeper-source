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

#ifndef ANTKEEPER_GAME_SPRING_ARM_COMPONENT_HPP
#define ANTKEEPER_GAME_SPRING_ARM_COMPONENT_HPP

#include <engine/entity/id.hpp>
#include <engine/math/vector.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/math/angles.hpp>
#include <engine/physics/spring.hpp>

/**
 * Attaches a camera to an entity using springs.
 */
struct spring_arm_component
{
	/// ID of the entity to which the spring arm is attached.
	entity::id parent_eid{entt::null};
	
	/// Height of the view frustum at the near clipping distance.
	double near_focal_plane_height{1.0};
	
	/// Local up rotation quaternion.
	math::dquat up_rotation{math::dquat::identity()};
	
	/// Attached camera rotation.
	math::dquat camera_rotation{math::dquat::identity()};
	
	/// Pitch, yaw, and roll velocities, in radians per second.
	math::dvec3 angular_velocities{};
	
	/// Minimum pitch, yaw, and roll angles, in radians.
	math::dvec3 min_angles{-math::half_pi<double>, -std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()};
	
	/// Maximum pitch, yaw, and roll angles, in radians.
	math::dvec3 max_angles{math::half_pi<double>, std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};
	
	physics::numeric_spring<math::dvec3, double> focal_point_spring;
	
	/// Pitch, yaw, and roll angles spring.
	physics::numeric_spring<math::dvec3, double> angles_spring;
	
	/// Arm length spring.
	// physics::numeric_spring<double, double> arm_length_spring;
	
	
	
	
	/// Height of the view frustum at the far clipping distance.
	double far_focal_plane_height{50.0};
	
	/// Horizontal FoV at maximum zoom.
	double near_hfov{math::radians(90.0)};
	
	/// Horizontal FoV at minimum zoom.
	double far_hfov{math::radians(45.0)};
	
	/// Zoom factor, on `[0, 1]`.
	double zoom{};
	
	/// Horizontal FoV of the camera, in radians.
	double hfov{};
	
	/// Vertical FoV of the camera, in radians.
	double vfov{};
	
	/// Position of the focal point, relative to the subject.
	math::dvec3 focal_point_offset{};
	
	/// Distance to the focal plane.
	double focal_distance{};
	
	/// Width of the view frustum at the focal distance.
	double focal_plane_width{};
	
	/// Height of the view frustum at the focal distance.
	double focal_plane_height{};
};

#endif // ANTKEEPER_GAME_SPRING_ARM_COMPONENT_HPP
