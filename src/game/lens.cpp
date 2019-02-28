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

#include "lens.hpp"
#include "camera-rig.hpp"
#include "../entity/systems/particle-system.hpp"

Lens::Lens(const Model* model, Animator* animator):
	wasActive(false),
	particleSystem(nullptr)
{
	// Setup model instance
	modelInstance.setModel(model);
	modelInstance.setCullingEnabled(false);
	
	// Setup spotlight
	spotlight.setColor(Vector3(1.0f));
	spotlight.setIntensity(1000.0f);
	spotlight.setAttenuation(Vector3(1, 0, 1));
	spotlight.setCutoff(std::cos(glm::radians(45.0f)));
	spotlight.setExponent(150.0f);
	spotlight.setActive(false);

	unfocusedDistance = 18.0f;
	unfocusDuration = 0.25f;
	focusedDistance = 12.0f;
	focusDuration = 0.75f;

	lensDistance = unfocusedDistance;
	focused = false;
	lastDistance = unfocusedDistance;
	sunDirection = Vector3(0, -1, 0);

	// Construct focus animation clip
	AnimationChannel<float>* channel;
	focusClip.setInterpolator(easeOutCubic<float>);
	channel = focusClip.addChannel(0);
	channel->insertKeyframe(0.0f, 0.0f);
	channel->insertKeyframe(focusDuration, 1.0f);

	// Construct unfocus animation clip
	unfocusClip.setInterpolator(easeOutCubic<float>);
	channel = unfocusClip.addChannel(0);
	channel->insertKeyframe(0.0f, 0.0f);
	channel->insertKeyframe(unfocusDuration, 1.0f);

	// Setup focus animation callbacks
	focusAnimation.setTimeFrame(focusClip.getTimeFrame());
	focusAnimation.setClip(&focusClip);
	focusAnimation.setAnimateCallback
	(
		[this](std::size_t id, float distance)
		{
			this->lensDistance = lerp(lastDistance, focusedDistance, distance);
		}
	);
	focusAnimation.setEndCallback
	(
		[this]()
		{
			this->focused = true;
		}
	);

	// Setup unfocus animation callbacks
	unfocusAnimation.setTimeFrame(unfocusClip.getTimeFrame());
	unfocusAnimation.setClip(&unfocusClip);
	unfocusAnimation.setAnimateCallback
	(
		[this](std::size_t id, float distance)
		{
			this->lensDistance = lerp(lastDistance, unfocusedDistance, distance);
		}
	);

	// Add animations to animator
	animator->addAnimation(&focusAnimation);
	animator->addAnimation(&unfocusAnimation);
}

Lens::~Lens()
{}

void Lens::update(float dt)
{
	//Quaternion alignment = glm::rotation(Vector3(0, 1, 0), -sunDirection) * glm::angleAxis(glm::radians(90.0f), Vector3(0, 1, 0));
	Quaternion alignment = glm::rotation(Vector3(0, 1, 0), -sunDirection) * glm::angleAxis(orbitCam->getAzimuth() + glm::radians(90.0f), Vector3(0, 1, 0));
	Quaternion rotation = glm::normalize(alignment);
	Vector3 translation = pick + sunDirection * -lensDistance;
	
	modelInstance.setTranslation(translation);
	modelInstance.setRotation(rotation);
	
	float t = (1.0 - (lensDistance - focusedDistance) / (unfocusedDistance - focusedDistance));

	float unfocusedIntensity = 250.0f * 2.0f;
	float focusedIntensity = 1000.0f * 2.0f;
	float intensity = easeInExpo(unfocusedIntensity, focusedIntensity, t);

	float unfocusedExponent = 100.0f;
	float focusedExponent = 5000.0f;
	float exponent = easeInExpo(unfocusedExponent, focusedExponent, t);

	spotlight.setIntensity(intensity);
	spotlight.setExponent(exponent);
	spotlight.setTranslation(pick + sunDirection * -lensDistance);
	spotlight.setDirection(sunDirection);

	if (active && !wasActive)
	{
		modelInstance.resetTweens();
		spotlight.resetTweens();
		modelInstance.setActive(true);
		spotlight.setActive(true);
	}
	else if (!active && wasActive)
	{
		modelInstance.setActive(false);
		spotlight.setActive(false);
	}

	if (active && focused)
	{
		int count = 10;
		for (int i = 0; i < count; ++i)
			particleSystem->emit(pick);
	}

	wasActive = active;
}

void Lens::focus()
{
	lastDistance = lensDistance;

	unfocusAnimation.stop();
	focusAnimation.rewind();
	focusAnimation.play();
}

void Lens::unfocus()
{
	lastDistance = lensDistance;

	focusAnimation.stop();
	unfocusAnimation.rewind();
	unfocusAnimation.play();
	focused = false;
}

void Lens::setSunDirection(const Vector3& direction)
{
	sunDirection = direction;
}

void Lens::setParticleSystem(ParticleSystem* particleSystem)
{
	this->particleSystem = particleSystem;
}

