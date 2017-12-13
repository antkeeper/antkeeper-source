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

#include "camera-rig.hpp"
#include <algorithm>
#include <cmath>

CameraRig::CameraRig():
	camera(nullptr),
	translation(0.0f),
	rotation(1.0f, 0.0f, 0.0f, 0.0f),
	forward(0.0f, 0.0f, -1.0f),
	right(1.0f, 0.0f, 0.0f),
	up(0.0f, 1.0f, 0.0f)
{}

void CameraRig::attachCamera(Camera* camera)
{
	this->camera = camera;
	if (camera != nullptr)
	{
		camera->lookAt(translation, translation + forward, up);
	}
}

void CameraRig::detachCamera()
{
	camera = nullptr;
}

void CameraRig::setTranslation(const Vector3& translation)
{
	this->translation = translation;
}

void CameraRig::setRotation(const Quaternion& rotation)
{
	this->rotation = rotation;
	
	// Calculate orthonormal basis
	forward = glm::normalize(rotation * Vector3(0.0f, 0.0f, -1.0f));
	up = glm::normalize(rotation * Vector3(0.0f, 1.0f, 0.0f));
	right = glm::normalize(glm::cross(forward, up));
}

FreeCam::FreeCam():
	pitchRotation(1.0f, 0.0f, 0.0f, 0.0f),
	yawRotation(1.0f, 0.0f, 0.0f, 0.0f),
	pitch(0.0f),
	yaw(0.0f)
{}

FreeCam::~FreeCam()
{}

void FreeCam::update(float dt)
{
	if (getCamera() != nullptr)
	{
		getCamera()->lookAt(getTranslation(), getTranslation() + getForward(), getUp());
	}
}

void FreeCam::move(const Vector2& velocity)
{
	setTranslation(getTranslation() + getForward() * velocity.x + getRight() * velocity.y);
}

float wrapAngle(float x)
{
	x = std::fmod(x + glm::pi<float>(), glm::pi<float>() * 2.0f);
	if (x < 0.0f)
	{
		x += glm::pi<float>() * 2.0f;
	}
	
	return x - glm::pi<float>();
}

void FreeCam::rotate(float pan, float tilt)
{
	pitch = wrapAngle(pitch + tilt);
	yaw = wrapAngle(yaw + pan);
	
	pitch = std::min<float>(std::max<float>(glm::radians(-89.0f), pitch), glm::radians(89.0f));
	
	// Form quaternions from pan and tilt angles
	pitchRotation = glm::angleAxis(pitch,Vector3(1.0f, 0.0f, 0.0f));
	yawRotation = glm::angleAxis(yaw, Vector3(0.0f, 1.0f, 0.0f));
	
	// Rotate camera	
	setRotation(glm::normalize(yawRotation * pitchRotation));
}

OrbitCam::OrbitCam():
	elevationRotation(1, 0, 0, 0),
	azimuthRotation(1, 0, 0, 0),
	targetElevationRotation(1, 0, 0, 0),
	targetAzimuthRotation(1, 0, 0, 0),
	targetRotation(1, 0, 0, 0)
{}

OrbitCam::~OrbitCam()
{}

void OrbitCam::update(float dt)
{
	float interpolationFactor = 0.25f / (1.0 / 60.0f) * dt;

	// Calculate rotation and target rotation quaternions
	//rotation = azimuthRotation * elevationRotation;
	targetRotation = targetAzimuthRotation * targetElevationRotation;
	
	// Calculate target translation
	targetTranslation = targetFocalPoint + targetRotation * Vector3(0.0f, 0.0f, targetFocalDistance);
	
	// Interpolate rotation
	//rotation = glm::mix(rotation, targetRotation, interpolationFactor);
	
	// Interpolate angles
	setElevation(glm::mix(elevation, targetElevation, interpolationFactor));
	setAzimuth(glm::mix(azimuth, targetAzimuth, interpolationFactor));
	
	// Calculate rotation
	setRotation(azimuthRotation * elevationRotation);
	
	// Interpolate focal point and focal distance
	focalPoint = glm::mix(focalPoint, targetFocalPoint, interpolationFactor);
	focalDistance = glm::mix(focalDistance, targetFocalDistance, interpolationFactor);
	
	// Caluclate translation
	setTranslation(focalPoint + getRotation() * Vector3(0.0f, 0.0f, focalDistance));
	/*
	// Recalculate azimuth
	azimuthRotation = rotation;
	azimuthRotation.x = 0.0f;
	azimuthRotation.z = 0.0f;
	azimuthRotation = glm::normalize(azimuthRotation);
	azimuth = 2.0f * std::acos(azimuthRotation.w);
	
	// Recalculate elevation
	elevationRotation = rotation;
	elevationRotation.y = 0.0f;
	elevationRotation.z = 0.0f;
	elevationRotation = glm::normalize(elevationRotation);
	elevation = 2.0f * std::acos(elevationRotation.w);
	*/
	
	// Update camera
	if (getCamera() != nullptr)
	{
		getCamera()->lookAt(getTranslation(), getTranslation() + getForward(), getUp());
	}
}

void OrbitCam::move(Vector2 direction)
{
	targetFocalPoint += azimuthRotation * Vector3(direction.x, 0.0f, direction.y);
}

void OrbitCam::rotate(float angle)
{
	setTargetAzimuth(targetAzimuth + angle);
}

void OrbitCam::zoom(float distance)
{
	setTargetFocalDistance(targetFocalDistance - distance);
}

void OrbitCam::setFocalPoint(const Vector3& point)
{
	focalPoint = point;
}

void OrbitCam::setFocalDistance(float distance)
{
	focalDistance = distance;
}

void OrbitCam::setElevation(float angle)
{
	elevation = angle;
	elevationRotation = glm::angleAxis(elevation, Vector3(-1.0f, 0.0f, 0.0f));
}

void OrbitCam::setAzimuth(float angle)
{
	azimuth = angle;
	azimuthRotation = glm::angleAxis(azimuth, Vector3(0.0f, 1.0f, 0.0f));
}

void OrbitCam::setTargetFocalPoint(const Vector3& point)
{
	targetFocalPoint = point;
}

void OrbitCam::setTargetFocalDistance(float distance)
{
	targetFocalDistance = distance;
}

void OrbitCam::setTargetElevation(float angle)
{
	targetElevation = angle;
	targetElevationRotation = glm::angleAxis(targetElevation, Vector3(-1.0f, 0.0f, 0.0f));
}

void OrbitCam::setTargetAzimuth(float angle)
{
	targetAzimuth = angle;
	targetAzimuthRotation = glm::angleAxis(targetAzimuth, Vector3(0.0f, 1.0f, 0.0f));
}

ShadowCam::ShadowCam():
	light(nullptr)
{}

ShadowCam::~ShadowCam()
{}

void ShadowCam::setLight(const PunctualLight* light)
{
	this->light = light;
	
	if (light != nullptr)
	{
		
	}
}

void ShadowCam::update(float dt)
{
	
}
