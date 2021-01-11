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

#ifndef ANTKEEPER_CAMERA_RIG_HPP
#define ANTKEEPER_CAMERA_RIG_HPP

#include "math/quaternion-type.hpp"
#include "math/transform-type.hpp"
#include "scene/camera.hpp"
#include "utility/fundamental-types.hpp"


/**
 * Abstract base class for camera rigs which control the movement of cameras.
 */
class camera_rig
{
public:
	typedef math::quaternion<float> quaternion_type;
	typedef math::transform<float> transform_type;
	
	camera_rig();
	
	/**
	 * Updates the rig.
	 *
	 * @param dt Delta time.
	 */
	virtual void update(float dt) = 0;
	
	/**
	 * Attaches a camera to the rig.
	 *
	 * @param camera Camera to attach.
	 */
	void attach(scene::camera* camera);
	
	/**
	 * Detaches the camera from the rig.
	 */
	void detach();
	

	
	/**
	 * Returns the attached camera.
	 */
	const scene::camera* get_camera() const;
	
	const float3& get_translation() const;
	const quaternion_type& get_rotation() const;
	const float3& get_forward() const;
	const float3& get_right() const;
	const float3& get_up() const;

protected:
	/**
	 * Updates the transform of the camera
	 */
	void update_transform(const transform_type& transform);
	
	void update_projection(float fov, float aspect_ratio, float clip_near, float clip_far);
	void update_projection(float clip_left, float clip_right, float clip_bottom, float clip_top, float clip_near, float clip_far);

private:
	scene::camera* camera;
	transform_type transform;
	float3 forward;
	float3 right;
	float3 up;
};

inline const scene::camera* camera_rig::get_camera() const
{
	return camera;
}

inline const float3& camera_rig::get_translation() const
{
	return transform.translation;
}

inline const typename camera_rig::quaternion_type& camera_rig::get_rotation() const
{
	return transform.rotation;
}

inline const float3& camera_rig::get_forward() const
{
	return forward;
}

inline const float3& camera_rig::get_right() const
{
	return right;
}

inline const float3& camera_rig::get_up() const
{
	return up;
}

#endif // ANTKEEPER_CAMERA_RIG_HPP

