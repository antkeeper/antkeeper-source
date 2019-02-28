/*
 * Copyright (C) 2017-2019  Christopher J. Howard
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

#include "brush.hpp"
#include "camera-rig.hpp"
#include <stdexcept>

Brush::Brush(const Model* model, Animator* animator):
	wasActive(false)
{
	// Setup model instance
	modelInstance.setModel(model);
	
	pressedDistance = -0.25f;
	releasedDistance = 0.5f;
	pressDuration = 0.25f;
	releaseDuration = 0.175f;

	tipDistance = releasedDistance;
	lastTipDistance = tipDistance;
	painting = false;
	speed = 0.0f;
	mousePosition = Vector2(0.0f);
	lastMousePosition = mousePosition;
	screenDimensions = Vector2(1.0f);

	// Construct press animation clip
	AnimationChannel<float>* channel;
	pressClip.setInterpolator(easeOutCubic<float>);
	channel = pressClip.addChannel(0);
	channel->insertKeyframe(0.0f, 0.0f);
	channel->insertKeyframe(pressDuration, 1.0f);

	// Construct release animation clip
	releaseClip.setInterpolator(easeOutCubic<float>);
	channel = releaseClip.addChannel(0);
	channel->insertKeyframe(0.0f, 0.0f);
	channel->insertKeyframe(releaseDuration, 1.0f);

	// Setup press animation callbacks
	pressAnimation.setTimeFrame(pressClip.getTimeFrame());
	pressAnimation.setClip(&pressClip);
	pressAnimation.setAnimateCallback
	(
		[this](std::size_t id, float t)
		{
			this->tipDistance = lerp(lastTipDistance, pressedDistance, t);
		}
	);
	pressAnimation.setEndCallback
	(
		[this]()
		{
			this->painting = true;
		}
	);

	// Setup release animation callbacks
	releaseAnimation.setTimeFrame(releaseClip.getTimeFrame());
	releaseAnimation.setClip(&releaseClip);
	releaseAnimation.setAnimateCallback
	(
		[this](std::size_t id, float t)
		{
			this->tipDistance = lerp(lastTipDistance, releasedDistance, t);
		}
	);

	// Add animations to animator
	animator->addAnimation(&pressAnimation);
	animator->addAnimation(&releaseAnimation);
}

Brush::~Brush()
{}

void Brush::update(float dt)
{
	Vector2 screenCenter = Vector2(screenDimensions.x * 0.5f, screenDimensions.y * 0.5f);

	// Calculate mouse movement speed
	Vector2 mouseDifference = (mousePosition - lastMousePosition) / std::min<float>(screenDimensions.x, screenDimensions.y);
	Vector2 mouseDirection(0.0f);
	float mouseSpeed = glm::length2(mouseDifference);
	if (mouseSpeed != 0.0f)
	{
		mouseSpeed = std::sqrt(mouseSpeed);
		mouseDirection = mouseDifference * (1.0f / mouseSpeed);
	}

	this->lastMousePosition = this->mousePosition;

	Vector3 tiltDirection = Vector3(mouseDirection.x, 0.0f, mouseDirection.y);
	float tiltMagnitude = std::min<float>(0.5f, mouseSpeed * 10.0f);
	if (!tiltMagnitude)
	{
		tiltDirection = Vector3(0, 1, 0);
	}

	Vector2 tiltForce = mouseDirection * mouseSpeed;
	velocity += tiltForce;

	Quaternion tilt = glm::normalize(glm::slerp(Quaternion(1, 0, 0, 0), glm::normalize(glm::rotation(Vector3(0, 1, 0), tiltDirection)), tiltMagnitude * 0.0f));

	
	Quaternion alignment = glm::angleAxis(orbitCam->getAzimuth(), Vector3(0, 1, 0));
	Quaternion rotation = glm::normalize(alignment * tilt);
	Vector3 translation = pick + rotation * Vector3(0, tipDistance, 0);
	
	// Set tool position
	modelInstance.setTranslation(translation);
	modelInstance.setRotation(rotation);

	if (active && !wasActive)
	{
		modelInstance.resetTweens();
		modelInstance.setActive(true);
	}
	else if (!active && wasActive)
	{
		modelInstance.setActive(false);
	}

	wasActive = active;
}

void Brush::press()
{
	lastTipDistance = tipDistance;

	releaseAnimation.stop();
	pressAnimation.rewind();
	pressAnimation.play();
}

void Brush::release()
{
	lastTipDistance = tipDistance;

	pressAnimation.stop();
	releaseAnimation.rewind();
	releaseAnimation.play();
	painting = false;
}


void Brush::setTiltParams(const Vector2& mousePosition, const Vector2& screenDimensions)
{
	this->mousePosition = mousePosition;
	this->screenDimensions = screenDimensions;
}

