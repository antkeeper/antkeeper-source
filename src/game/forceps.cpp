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

#include "forceps.hpp"
#include "camera-rig.hpp"
#include <stdexcept>

Forceps::Forceps(const Model* model, Animator* animator):
	wasActive(false)
{
	// Allocate pose and initialize to bind pose
	pose = new Pose(model->getSkeleton());
	pose->reset();
	
	// Setup model instance
	modelInstance.setModel(model);
	modelInstance.setPose(pose);
	
	// Find pinch animation
	pinchClip = model->getSkeleton()->getAnimationClip("pinch");
	if (!pinchClip)
	{
		throw std::runtime_error("Forceps pinch animation clip not found.");
	}
	
	// Find release animation
	releaseClip = model->getSkeleton()->getAnimationClip("release");
	if (!releaseClip)
	{
		throw std::runtime_error("Forceps release animation clip not found.");
	}

	// Scale animation speed
	float pinchDuration = 0.1f;
	float releaseDuration = 0.05f;
	float pinchSpeed = std::get<1>(pinchClip->getTimeFrame()) / pinchDuration;
	float releaseSpeed = std::get<1>(releaseClip->getTimeFrame()) / releaseDuration;

	std::cout << std::get<1>(pinchClip->getTimeFrame()) << std::endl;
	std::cout << std::get<1>(releaseClip->getTimeFrame()) << std::endl;

	// Setup pinch animation callbacks
	pinchAnimation.setSpeed(pinchSpeed);
	pinchAnimation.setTimeFrame(pinchClip->getTimeFrame());
	pinchAnimation.setClip(pinchClip);
	pinchAnimation.setAnimateCallback
	(
		[this](std::size_t id, const Transform& transform)
		{
			this->pose->setRelativeTransform(id, transform);
		}
	);
	pinchAnimation.setEndCallback
	(
		[this]()
		{
			this->pinched = true;
		}
	);

	// Setup release animation callbacks
	releaseAnimation.setSpeed(releaseSpeed);
	releaseAnimation.setTimeFrame(releaseClip->getTimeFrame());
	releaseAnimation.setClip(releaseClip);
	releaseAnimation.setAnimateCallback
	(
		[this](std::size_t id, const Transform& transform)
		{
			this->pose->setRelativeTransform(id, transform);
		}
	);
	
	hoverDistance = 1.0f;
	
	// Setup timing
	float descentDuration = 0.125f;
	float ascentDuration = 0.125f;
	
	/*
	// Allocate tweener and and setup tweens
	tweener = new Tweener();
	descentTween = new Tween<float>(EaseFunction::OUT_CUBIC, 0.0f, descentDuration, hoverDistance, -hoverDistance);
	ascentTween = new Tween<float>(EaseFunction::IN_CUBIC, 0.0f, ascentDuration, 0.0f, hoverDistance);
	descentTween->setEndCallback(std::bind(&TweenBase::start, ascentTween));
	tweener->addTween(descentTween);
	tweener->addTween(ascentTween);
	*/
	
	// Setup initial state
	state = Forceps::State::RELEASED;
	for (std::size_t i = 0; i < pinchClip->getChannelCount(); ++i)
	{
		const AnimationChannel<Transform>* channel = pinchClip->getChannelByIndex(i);
		pose->setRelativeTransform(channel->getChannelID(), *std::get<1>(channel->getKeyframe(0)));
	}
	pose->concatenate();

	animator->addAnimation(&pinchAnimation);
	animator->addAnimation(&releaseAnimation);
}

Forceps::~Forceps()
{
	delete pose;
}

void Forceps::update(float dt)
{
	// Determine distance from pick point
	float forcepsDistance = hoverDistance;
	
	Quaternion alignment = glm::angleAxis(orbitCam->getAzimuth(), Vector3(0, 1, 0));
	Quaternion tilt = glm::angleAxis(glm::radians(15.0f), Vector3(0, 0, -1));
	tilt = tilt * glm::angleAxis(glm::radians(-70.0f), tilt * Vector3(0, 1, 0));
	Quaternion rotation = glm::normalize(alignment * tilt);
	Vector3 translation = pick + rotation * Vector3(0, forcepsDistance, 0);
	
	// Set tool position
	modelInstance.setTranslation(translation);
	modelInstance.setRotation(rotation);


	pose->concatenate();

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


	/*
	
	if (state == Forceps::State::RELEASED)
	{
		
	}
	else if (state == Forceps::State::RELEASING)
	{
		// Perform release animation
		releaseAnimation->animate(pose, animationTime);
		pose->concatenate();
		
		// If release animation is finished
		if (animationTime >= releaseAnimation->getEndTime())
		{
			// Changed to released state
			state = Forceps::State::RELEASED;
		}
	}
	else if (state == Forceps::State::PINCHED)
	{
		if (!ascentTween->isStopped())
		{
			// Calculate interpolation factor
			float interpolationFactor = (ascentTween->getTweenValue() - ascentTween->getStartValue()) /  ascentTween->getDeltaValue();
			
			// Form tilt quaternion
			//Quaternion tilt = glm::angleAxis(glm::radians(15.0f), Vector3(0, 0, -1));
			tilt = glm::angleAxis(glm::radians(15.0f), Vector3(0, 0, -1));
			
			// Project camera forward onto XZ plane
			Vector3 cameraForwardXZ = orbitCam->getCamera()->getForward();
			cameraForwardXZ.y = 0.0f;
			cameraForwardXZ = glm::normalize(cameraForwardXZ);
			
			// Form alignment quaternion
			//Quaternion alignment = glm::rotation(Vector3(0, 0, -1), cameraForwardXZ);
			alignment = glm::angleAxis(orbitCam->getAzimuth(), Vector3(0, 1, 0));
			
			// Calculate target rotation at the top of the ascentTween
			rotationTop = glm::normalize(alignment * tilt);
			
			// Interpolate between bottom and top rotations
			Quaternion interpolatedRotation = glm::normalize(glm::slerp(rotationBottom, rotationTop, interpolationFactor));
			
			// Set target translation at the top of the ascent
			translationTop = pick + rotationTop * Vector3(0, hoverDistance, 0);
			
			// Interpolate between bottom and top translations
			Vector3 interpolatedTranslation = glm::lerp(translationBottom, translationTop, interpolationFactor);
			
			// Update model instance transform
			modelInstance.setTranslation(interpolatedTranslation);	
			modelInstance.setRotation(interpolatedRotation);
		}
		
		if (suspendedAnt != nullptr)
		{
			// Project forceps forward vector onto XZ plane
			Vector3 forward = glm::normalize(modelInstance.getRotation() * Vector3(0, 0, -1));
			forward.y = 0.0f;
			forward = glm::normalize(forward);

			// Calculate suspension quaternion
			Quaternion suspensionRotation = glm::normalize(glm::rotation(Vector3(0, 0, -1), ((flipRotation) ? -forward : forward)));
			
			// Suspend ant
			suspendedAnt->suspend(modelInstance.getTranslation(), suspensionRotation);
		}
	}
	else if (state == Forceps::State::PINCHING)
	{
		// Perform pinch animation
		pinchAnimation->animate(pose, animationTime);
		pose->concatenate();
		
		// Rotate to align forceps with ant
		if (targetedAnt != nullptr)
		{
			// Calculate interpolation factor
			float interpolationFactor = (descentTween->getTweenValue() - descentTween->getStartValue()) /  descentTween->getDeltaValue();
			
			// Set target translation at the bottom of the descent
			translationBottom = targetedAnt->getPosition();
			
			// Interpolate between top and bottom translations
			Vector3 interpolatedTranslation = glm::lerp(translationTop, translationBottom, interpolationFactor);
			
			// Project camera forward onto XZ plane
			Vector3 cameraForwardXZ = orbitCam->getCamera()->getForward();
			cameraForwardXZ.y = 0.0f;
			cameraForwardXZ = glm::normalize(cameraForwardXZ);
			
			// Form tilt quaternion
			tilt = glm::angleAxis(glm::radians(15.0f), -cameraForwardXZ);
			
			// Project ant forward onto XZ plane
			Vector3 antForwardXZ = targetedAnt->getForward();
			antForwardXZ.y = 0.0f;
			antForwardXZ = glm::normalize(antForwardXZ);
			
			// Form alignment quaternion
			alignment = glm::rotation(Vector3(0, 0, -1), (flipRotation) ? antForwardXZ : -antForwardXZ);
			
			// Calculate target rotation at the bottom of the descent
			rotationBottom = glm::normalize(tilt * alignment);
			
			// Interpolate between top and bottom rotations
			Quaternion interpolatedRotation = glm::normalize(glm::slerp(rotationTop, rotationBottom, interpolationFactor));
			
			// Update model instance transform
			modelInstance.setTranslation(interpolatedTranslation);
			modelInstance.setRotation(interpolatedRotation);
		}
		
		// If pinch animation is finished
		if (animationTime >= pinchAnimation->getEndTime() && descentTween->isStopped())
		{
			// If an ant was targeted
			if (targetedAnt != nullptr)
			{
				// Suspend targeted ant
				suspendedAnt = targetedAnt;
				suspendedAnt->setState(Ant::State::SUSPENDED);
				//suspendedAnt->suspend(modelInstance.getTranslation());
				targetedAnt = nullptr;
			}
			
			// Change to pinched state
			state = Forceps::State::PINCHED;
		}
	}
	*/
}

void Forceps::pinch()
{
	releaseAnimation.stop();
	pinchAnimation.rewind();
	pinchAnimation.play();

	/*
	// Change state to pinching
	state = Forceps::State::PINCHING;
	animationTime = 0.0f;
	
	if (colony != nullptr)
	{
		// Target nearest ant in pinching radius
		Sphere pinchingBounds = Sphere(pick, 0.35f);
		
		// Build a list of ants which intersect the pinching bounds
		std::list<Agent*> ants;
		colony->queryAnts(pinchingBounds, &ants);
		
		// Target ant closest to the center of the pinching bounds
		float closestDistance = std::numeric_limits<float>::infinity();
		for (Agent* agent: ants)
		{
			Ant* ant = static_cast<Ant*>(agent);
			
			Vector3 difference = ant->getPosition() - pinchingBounds.getCenter();
			float distanceSquared = glm::dot(difference, difference);
			if (distanceSquared < closestDistance)
			{
				closestDistance = distanceSquared;
				targetedAnt = ant;
			}
		}
		
		if (targetedAnt != nullptr)
		{
			// Start descent tweener
			descentTween->start();
			
			// Save translation & rotation
			translationTop = modelInstance.getTranslation();
			rotationTop = modelInstance.getRotation();
			
			// Project ant forward onto XZ plane
			Vector3 antForwardXZ = targetedAnt->getForward();
			antForwardXZ.y = 0.0f;
			antForwardXZ = glm::normalize(antForwardXZ);
			
			// Project camera forward onto XZ plane
			Vector3 cameraForwardXZ = orbitCam->getCamera()->getForward();
			cameraForwardXZ.y = 0.0f;
			cameraForwardXZ = glm::normalize(cameraForwardXZ);
			
			// Find angle between ant and camera on XZ plane
			float angle = std::acos(glm::dot(cameraForwardXZ, antForwardXZ));
			
			// Determine direction to rotate
			flipRotation = (angle > glm::radians(90.0f));
		}
	}
	*/
}

void Forceps::release()
{
	pinchAnimation.stop();
	releaseAnimation.rewind();
	releaseAnimation.play();

	/*
	// Change state to releasing
	state = Forceps::State::RELEASING;
	animationTime = 0.0f;
	targetedAnt = nullptr;
	
	if (suspendedAnt != nullptr)
	{
		Ray pickingRay;
		pickingRay.origin = pick + Vector3(0, 1, 0);
		pickingRay.direction = Vector3(0, -1, 0);
		
		const std::vector<Navmesh::Triangle*>* navmeshTriangles = navmesh->getTriangles();
		for (Navmesh::Triangle* triangle: *navmeshTriangles)
		{
			auto result = intersects(pickingRay, triangle);
			if (std::get<0>(result))
			{
				Vector3 barycentricPosition = Vector3(std::get<2>(result), std::get<3>(result), 1.0f - std::get<2>(result) - std::get<3>(result));
				suspendedAnt->setPosition(triangle, barycentricPosition);
				
				break;
			}
		}
		
		// Release suspended ant
		suspendedAnt->setState(Ant::State::WANDER);
		suspendedAnt = nullptr;
	}
	
	// Reset tweens
	descentTween->reset();
	descentTween->stop();
	ascentTween->reset();
	ascentTween->stop();
	*/
}

