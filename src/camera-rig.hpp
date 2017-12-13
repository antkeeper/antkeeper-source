/*
 * Copyright (C) 2017  Christopher J. Howard
 *
 * This file is part of Antkeeper Source Code.
 *
 * Antkeeper Source Code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper Source Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper Source Code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CAMERA_RIG_HPP
#define CAMERA_RIG_HPP

#include <emergent/emergent.hpp>

using namespace Emergent;

/**
 * Abstract base class for camera rigs which control the movement of cameras.
 */
class CameraRig
{
public:
	CameraRig();
	
	/**
	 * Updates the rig.
	 */
	virtual void update(float dt) = 0;
	
	/**
	 * Attaches a camera to the rig.
	 *
	 * @param camera Camera to attach.
	 */
	void attachCamera(Camera* camera);
	
	/**
	 * Detaches a camera from the rig.
	 */
	void detachCamera();
	
	/**
	 * Sets the translation of the camera rig.
	 */
	void setTranslation(const Vector3& translation);
	
	/**
	 * Sets the rotation of the camera rig.
	 */
	void setRotation(const Quaternion& rotation);
	
	/**
	 * Returns the attached camera.
	 */
	const Camera* getCamera() const;
	
	/// @copydoc CameraRig::getCamera() const
	Camera* getCamera();
	
	const Vector3& getTranslation() const;
	const Quaternion& getRotation() const;
	const Vector3& getForward() const;
	const Vector3& getRight() const;
	const Vector3& getUp() const;

private:
	Camera* camera;
	Vector3 translation;
	Quaternion rotation;
	Vector3 forward;
	Vector3 right;
	Vector3 up;
};

inline const Camera* CameraRig::getCamera() const
{
	return camera;
}

inline Camera* CameraRig::getCamera()
{
	return camera;
}

inline const Vector3& CameraRig::getTranslation() const
{
	return translation;
}

inline const Quaternion& CameraRig::getRotation() const
{
	return rotation;
}

inline const Vector3& CameraRig::getForward() const
{
	return forward;
}

inline const Vector3& CameraRig::getRight() const
{
	return right;
}

inline const Vector3& CameraRig::getUp() const
{
	return up;
}

/**
 * Rig which can freely move around the scene.
 */
class FreeCam: public CameraRig
{
public:
	FreeCam();
	virtual ~FreeCam();
	
	/**
	 * Moves the camera.
	 *
	 * @param velocity Specifies the movement velocity on the local forward-right plane.
	 */
	void move(const Vector2& velocity);
	
	/**
	 * Rotates the camera.
	 *
	 * @param pan Specifies the angle (in radians) to rotate around the global Y-axis.
	 * @param tilt Specifies the angle (in radians) to rotate around the local X-axis.
	 */
	void rotate(float pan, float tilt);
	
	virtual void update(float dt);
	
private:
	Quaternion pitchRotation;
	Quaternion yawRotation;
	float pitch;
	float yaw;
};

/**
 * Rig which orbits around a focal point.
 */
class OrbitCam: public CameraRig
{
public:
	OrbitCam();
	virtual ~OrbitCam();
	virtual void update(float dt);
	
	/// @param direction Specifies the movement direction and speed scale on the XZ plane
	void move(Vector2 direction);
	void rotate(float angle);
	void zoom(float distance);
	
	void setFocalPoint(const Vector3& point);
	void setFocalDistance(float distance);
	void setElevation(float angle);
	void setAzimuth(float angle);
	void setTargetFocalPoint(const Vector3& point);
	void setTargetFocalDistance(float distance);
	void setTargetElevation(float angle);
	void setTargetAzimuth(float angle);
	
	const Vector3& getFocalPoint() const;
	float getFocalDistance() const;
	float getElevation() const;
	float getAzimuth() const;
	const Vector3& getTargetFocalPoint() const;
	float getTargetFocalDistance() const;
	float getTargetElevation() const;
	float getTargetAzimuth() const;
	const Vector3& getTargetTranslation() const;
	const Quaternion& getTargetRotation() const;
	
private:
	Vector3 focalPoint;
	float focalDistance;
	float elevation;
	float azimuth;
	
	Vector3 targetFocalPoint;
	float targetFocalDistance;
	float targetElevation;
	float targetAzimuth;
	
	Quaternion elevationRotation;
	Quaternion azimuthRotation;
	Quaternion targetElevationRotation;
	Quaternion targetAzimuthRotation;
	Quaternion targetRotation;
	Vector3 targetTranslation;
};

inline const Vector3& OrbitCam::getFocalPoint() const
{
	return focalPoint;
}

inline float OrbitCam::getFocalDistance() const
{
	return focalDistance;
}

inline float OrbitCam::getElevation() const
{
	return elevation;
}

inline float OrbitCam::getAzimuth() const
{
	return azimuth;
}

inline const Vector3& OrbitCam::getTargetFocalPoint() const
{
	return targetFocalPoint;
}

inline float OrbitCam::getTargetFocalDistance() const
{
	return targetFocalDistance;
}

inline float OrbitCam::getTargetElevation() const
{
	return targetElevation;
}

inline float OrbitCam::getTargetAzimuth() const
{
	return targetAzimuth;
}

inline const Vector3& OrbitCam::getTargetTranslation() const
{
	return targetTranslation;
}

inline const Quaternion& OrbitCam::getTargetRotation() const
{
	return targetRotation;
}

/**
 * Rig which aligns a camera with a light. Used for rendering shadow maps.
 */
class ShadowCam
{
public:
	ShadowCam();
	virtual ~ShadowCam();
	
	void setLight(const PunctualLight* light);
	
	void update(float dt);
	
private:
	const PunctualLight* light;
};

#endif // CAMERA_RIG_HPP