/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_ORBIT_CAM_HPP
#define ANTKEEPER_ORBIT_CAM_HPP

#include "camera-rig.hpp"
#include "animation/spring.hpp"
#include <array>

/**
 * Rig which orbits around a focal point.
 */
class orbit_cam: public camera_rig
{
public:
	orbit_cam();
	virtual ~orbit_cam();
	virtual void update(float dt);
	
	/// @param direction Specifies the movement direction and speed scale on the XZ plane
	void move(const float3& translation);
	void pan(float angle);
	void tilt(float angle);
	void zoom(float factor);
	
	void reset_springs();
	
	void set_aspect_ratio(float ratio);
	
	void set_focal_point(const float3& point);
	void set_azimuth(float angle);
	void set_elevation(float angle);
	void set_zoom(float factor);
	
	void set_target_focal_point(const float3& point);
	void set_target_azimuth(float angle);
	void set_target_elevation(float angle);
	void set_target_zoom(float factor);
	
	void set_azimuth_limits(const std::array<float, 2>& limits);
	void set_elevation_limits(const std::array<float, 2>& limits);
	void set_focal_distance_limits(const std::array<float, 2>& limits);
	void set_fov_limits(const std::array<float, 2>& limits);
	void set_clip_near_limits(const std::array<float, 2>& limits);
	void set_clip_far_limits(const std::array<float, 2>& limits);
	
	void set_focal_point_oscillation(float frequency);
	void set_azimuth_oscillation(float frequency);
	void set_elevation_oscillation(float frequency);
	void set_zoom_oscillation(float frequency);
	
	const float3& get_focal_point() const;
	float get_azimuth() const;
	float get_elevation() const;
	float get_zoom() const;
	const quaternion_type& get_azimuth_rotation() const;
	const quaternion_type& get_elevation_rotation() const;
	
private:
	float aspect_ratio;

	numeric_spring<float3, float> focal_point_spring;
	numeric_spring<float, float> azimuth_spring;
	numeric_spring<float, float> elevation_spring;
	numeric_spring<float, float> zoom_spring;	
	
	std::array<float, 2> azimuth_limits;
	std::array<float, 2> elevation_limits;
	std::array<float, 2> focal_distance_limits;
	std::array<float, 2> fov_limits;
	std::array<float, 2> clip_near_limits;
	std::array<float, 2> clip_far_limits;
	
	quaternion_type azimuth_rotation;
	quaternion_type elevation_rotation;
};

inline const float3& orbit_cam::get_focal_point() const
{
	return focal_point_spring.x0;
}

inline float orbit_cam::get_azimuth() const
{
	return azimuth_spring.x0;
}

inline float orbit_cam::get_elevation() const
{
	return elevation_spring.x0;
}

inline float orbit_cam::get_zoom() const
{
	return zoom_spring.x0;
}


inline const typename camera_rig::quaternion_type& orbit_cam::get_azimuth_rotation() const
{
	return azimuth_rotation;
}

inline const typename camera_rig::quaternion_type& orbit_cam::get_elevation_rotation() const
{
	return elevation_rotation;
}
#endif // ANTKEEPER_ORBIT_CAM_HPP
