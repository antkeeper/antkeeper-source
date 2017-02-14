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

#ifndef CAMERA_CONTROLLER_HPP
#define CAMERA_CONTROLLER_HPP

#include <emergent/emergent.hpp>

using namespace Emergent;

class CameraController
{
public:
	CameraController();
	virtual void update(float dt) = 0;
	
	void setCamera(Camera* camera);
	
	const Camera* getCamera() const;
	Camera* getCamera();

protected:
	Camera* camera;
};

inline void CameraController::setCamera(Camera* camera)
{
	this->camera = camera;
}

inline const Camera* CameraController::getCamera() const
{
	return camera;
}

inline Camera* CameraController::getCamera()
{
	return camera;
}

class SurfaceCameraController: public CameraController
{
public:
	SurfaceCameraController();
	virtual ~SurfaceCameraController();
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
	const Vector3& getTranslation() const;
	const glm::quat& getRotation() const;
	const Vector3& getTargetTranslation() const;
	const glm::quat& getTargetRotation() const;
	
private:
	Vector3 focalPoint;
	float focalDistance;
	float elevation;
	float azimuth;
	
	Vector3 targetFocalPoint;
	float targetFocalDistance;
	float targetElevation;
	float targetAzimuth;
	
	glm::quat elevationRotation;
	glm::quat azimuthRotation;
	glm::quat rotation;
	glm::quat targetElevationRotation;
	glm::quat targetAzimuthRotation;
	glm::quat targetRotation;
	Vector3 translation;
	Vector3 targetTranslation;
};

inline const Vector3& SurfaceCameraController::getFocalPoint() const
{
	return focalPoint;
}

inline float SurfaceCameraController::getFocalDistance() const
{
	return focalDistance;
}

inline float SurfaceCameraController::getElevation() const
{
	return elevation;
}

inline float SurfaceCameraController::getAzimuth() const
{
	return azimuth;
}

inline const Vector3& SurfaceCameraController::getTargetFocalPoint() const
{
	return targetFocalPoint;
}

inline float SurfaceCameraController::getTargetFocalDistance() const
{
	return targetFocalDistance;
}

inline float SurfaceCameraController::getTargetElevation() const
{
	return targetElevation;
}

inline float SurfaceCameraController::getTargetAzimuth() const
{
	return targetAzimuth;
}

inline const Vector3& SurfaceCameraController::getTranslation() const
{
	return translation;
}

inline const glm::quat& SurfaceCameraController::getRotation() const
{
	return rotation;
}

inline const Vector3& SurfaceCameraController::getTargetTranslation() const
{
	return targetTranslation;
}

inline const glm::quat& SurfaceCameraController::getTargetRotation() const
{
	return targetRotation;
}

/**
 * Note, when mouseover tunnel, highlight (in red?) the entire path from the end of the tunnel to the entrance of the nest.
 */
class TunnelCameraController: public CameraController
{
public:
	TunnelCameraController();
	virtual ~TunnelCameraController();
	
	virtual void update(float dt);
	
	void rotateCW(float scale);
	void rotateCCW(float scale);
	
	// Ascends the tunnel system in the direction of the entrance
	void ascend(float scale);
	
	// Descends along the current tunnel
	void descend(float scale);
};

#endif