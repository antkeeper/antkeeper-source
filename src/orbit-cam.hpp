/*
 * Copyright (C) 2020  Christopher J. Howard
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
	void move(const float2& direction);
	void rotate(float angle);
	void tilt(float angle);
	void zoom(float distance);
	
	void set_focal_point(const float3& point);
	void set_focal_distance(float distance);
	void set_elevation(float angle);
	void set_azimuth(float angle);
	void set_target_focal_point(const float3& point);
	void set_target_focal_distance(float distance);
	void set_target_elevation(float angle);
	void set_target_azimuth(float angle);
	
	const float3& get_focal_point() const;
	float get_focal_distance() const;
	float get_elevation() const;
	float get_azimuth() const;
	const float3& get_target_focal_point() const;
	float get_target_focal_distance() const;
	float get_target_elevation() const;
	float get_target_azimuth() const;
	const float3& get_target_translation() const;
	const quaternion_type& get_target_rotation() const;
	
private:
	float3 focal_point;
	float focal_distance;
	float elevation;
	float azimuth;
	
	float3 target_focal_point;
	float target_focal_distance;
	float target_elevation;
	float target_azimuth;
	
	quaternion_type elevation_rotation;
	quaternion_type azimuth_rotation;
	quaternion_type target_elevation_rotation;
	quaternion_type target_azimuth_rotation;
	quaternion_type target_rotation;
	float3 target_translation;
};

inline const float3& orbit_cam::get_focal_point() const
{
	return focal_point;
}

inline float orbit_cam::get_focal_distance() const
{
	return focal_distance;
}

inline float orbit_cam::get_elevation() const
{
	return elevation;
}

inline float orbit_cam::get_azimuth() const
{
	return azimuth;
}

inline const float3& orbit_cam::get_target_focal_point() const
{
	return target_focal_point;
}

inline float orbit_cam::get_target_focal_distance() const
{
	return target_focal_distance;
}

inline float orbit_cam::get_target_elevation() const
{
	return target_elevation;
}

inline float orbit_cam::get_target_azimuth() const
{
	return target_azimuth;
}

inline const float3& orbit_cam::get_target_translation() const
{
	return target_translation;
}

inline const typename camera_rig::quaternion_type& orbit_cam::get_target_rotation() const
{
	return target_rotation;
}

#endif // ANTKEEPER_ORBIT_CAM_HPP
