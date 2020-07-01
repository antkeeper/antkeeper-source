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

#ifndef ANTKEEPER_CAMERA_RIG_HPP
#define ANTKEEPER_CAMERA_RIG_HPP

#include <vmq/vmq.hpp>

using namespace vmq::types;

class camera;

/**
 * Abstract base class for camera rigs which control the movement of cameras.
 */
class camera_rig
{
public:
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
	void attach(::camera* camera);
	
	/**
	 * Detaches a camera from the rig.
	 */
	void detach();
	
	/**
	 * Sets the translation of the camera rig.
	 */
	void set_translation(const float3& translation);
	
	/**
	 * Sets the rotation of the camera rig.
	 */
	void set_rotation(const vmq::quaternion<float>& rotation);
	
	/**
	 * Returns the attached camera.
	 */
	const ::camera* get_camera() const;
	
	/// @copydoc camera_rig::get_camera() const
	::camera* get_camera();
	
	const float3& get_translation() const;
	const vmq::quaternion<float>& get_rotation() const;
	const float3& get_forward() const;
	const float3& get_right() const;
	const float3& get_up() const;

private:
	camera* camera;
	float3 translation;
	vmq::quaternion<float> rotation;
	float3 forward;
	float3 right;
	float3 up;
};

inline const camera* camera_rig::get_camera() const
{
	return camera;
}

inline camera* camera_rig::get_camera()
{
	return camera;
}

inline const float3& camera_rig::get_translation() const
{
	return translation;
}

inline const vmq::quaternion<float>& camera_rig::get_rotation() const
{
	return rotation;
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

