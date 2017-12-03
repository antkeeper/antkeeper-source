#include "tool.hpp"
#include "ant.hpp"
#include "colony.hpp"
#include "../camera-controller.hpp"
#include <iostream>
#include <list>

Tool::Tool():
	active(false),
	pick(0.0f),
	cameraController(nullptr)
{
	modelInstance.setActive(active);
}

Tool::~Tool()
{}

void Tool::setActive(bool active)
{
	this->active = active;
	if (!active)
	{
		modelInstance.setActive(active);
	}
}

void Tool::setCameraController(const SurfaceCameraController* cameraController)
{
	this->cameraController = cameraController;
}

Forceps::Forceps(const Model* model)
{
	// Allocate pose and initialize to bind pose
	pose = new Pose(model->getSkeleton());
	pose->reset();
	pose->concatenate();
	
	// Setup model instance
	modelInstance.setModel(model);
	modelInstance.setPose(pose);
	
	// Find pinch animation
	pinchAnimation = model->getSkeleton()->getAnimation("pinch");
	if (!pinchAnimation)
	{
		std::cerr << "Forceps pinch animation not found" << std::endl;
	}
	
	// Find release animation
	releaseAnimation = model->getSkeleton()->getAnimation("release");
	if (!releaseAnimation)
	{
		std::cerr << "Forceps release animation not found" << std::endl;
	}
	
	hoverDistance = 1.0f;
	
	// Setup timing
	float descentDuration = 0.125f;
	float ascentDuration = 0.125f;
	float descentFrameCount = descentDuration / (1.0f / 60.0f);
	animationTimeStep = pinchAnimation->getEndTime() / descentFrameCount;
	
	// Allocate tweener and and setup tweens
	tweener = new Tweener();
	descentTween = new Tween<float>(EaseFunction::OUT_CUBIC, 0.0f, descentDuration, hoverDistance, -hoverDistance);
	ascentTween = new Tween<float>(EaseFunction::IN_CUBIC, 0.0f, ascentDuration, 0.0f, hoverDistance);
	descentTween->setEndCallback(std::bind(&TweenBase::start, ascentTween));
	tweener->addTween(descentTween);
	tweener->addTween(ascentTween);
	
	// Setup initial state
	state = Forceps::State::RELEASED;
	animationTime = 0.0f;
	colony = nullptr;
	targetedAnt = nullptr;
	suspendedAnt = nullptr;
	cameraController = nullptr;
	pick = Vector3(0.0f);
}

Forceps::~Forceps()
{
	delete pose;
	delete descentTween;
	delete ascentTween;
	delete tweener;
}

void Forceps::update(float dt)
{
	modelInstance.setActive(active);
	
	// Update tweener
	tweener->update(dt);
	
	// Determine distance from pick point
	float forcepsDistance = hoverDistance;
	if (!ascentTween->isStopped())
	{
		forcepsDistance = ascentTween->getTweenValue();
	}
	else if (!descentTween->isStopped())
	{
		forcepsDistance = descentTween->getTweenValue();
	}
		
	Quaternion alignment = glm::angleAxis(cameraController->getAzimuth(), Vector3(0, 1, 0));
	Quaternion tilt = glm::angleAxis(glm::radians(15.0f), Vector3(0, 0, -1));
	Quaternion rotation = glm::normalize(alignment * tilt);
	Vector3 translation = pick + rotation * Vector3(0, forcepsDistance, 0);
	
	// Set tool position
	modelInstance.setTranslation(translation);
	modelInstance.setRotation(rotation);
	
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
			Vector3 cameraForwardXZ = cameraController->getCamera()->getForward();
			cameraForwardXZ.y = 0.0f;
			cameraForwardXZ = glm::normalize(cameraForwardXZ);
			
			// Form alignment quaternion
			//Quaternion alignment = glm::rotation(Vector3(0, 0, -1), cameraForwardXZ);
			alignment = glm::angleAxis(cameraController->getAzimuth(), Vector3(0, 1, 0));
			
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
			Vector3 cameraForwardXZ = cameraController->getCamera()->getForward();
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
	
	// Increment animation time
	animationTime += animationTimeStep;
}

void Forceps::setColony(Colony* colony)
{
	this->colony = colony;
}

void Forceps::pinch()
{
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
			Vector3 cameraForwardXZ = cameraController->getCamera()->getForward();
			cameraForwardXZ.y = 0.0f;
			cameraForwardXZ = glm::normalize(cameraForwardXZ);
			
			// Find angle between ant and camera on XZ plane
			float angle = std::acos(glm::dot(cameraForwardXZ, antForwardXZ));
			
			// Determine direction to rotate
			flipRotation = (angle > glm::radians(90.0f));
		}
	}
}

void Forceps::release()
{
	// Change state to releasing
	state = Forceps::State::RELEASING;
	animationTime = 0.0f;
	targetedAnt = nullptr;
	
	if (suspendedAnt != nullptr)
	{
		/*
		auto result = intersects(pickingRay, pickTriangle);
		if (std::get<0>(result))
		{
			Vector3 barycentricPosition = Vector3(std::get<2>(result), std::get<3>(result), 1.0f - std::get<2>(result) - std::get<3>(result));
			pickAnt->setPosition(pickTriangle, barycentricPosition);
		}
		*/
		
		// Release suspended ant
		suspendedAnt->setState(Ant::State::WANDER);
		suspendedAnt = nullptr;
	}
	
	// Reset tweens
	descentTween->reset();
	descentTween->stop();
	ascentTween->reset();
	ascentTween->stop();
}

Lens::Lens(const Model* model)
{
	// Setup model instance
	modelInstance.setModel(model);
	
	unfocusedDistance = 15.0f;
	focusedDistance = 12.0f;
	focused = false;
	sunDirection = Vector3(0, -1, 0);
	
	// Setup timing
	float descentDuration = 0.75f;
	float ascentDuration = 0.25f;
	
	// Allocate tweener and and setup tweens
	tweener = new Tweener();
	descentTween = new Tween<float>(EaseFunction::OUT_CUBIC, 0.0f, descentDuration, unfocusedDistance, focusedDistance - unfocusedDistance);
	ascentTween = new Tween<float>(EaseFunction::OUT_CUBIC, 0.0f, ascentDuration, focusedDistance, unfocusedDistance - focusedDistance);
	descentTween->setEndCallback
	(
		[this](float t)
		{
			focused = true;
		}
	);
	tweener->addTween(descentTween);
	tweener->addTween(ascentTween);
}

Lens::~Lens()
{
	delete descentTween;
	delete ascentTween;
	delete tweener;
}

void Lens::update(float dt)
{
	/*
	// Rotate to face camera
	hoverDistance = 30.0f;
	Vector3 direction = glm::normalize(cameraController->getCamera()->getTranslation() - pick);
	//direction = cameraController->getCamera()->getForward();
	float distance = glm::distance(pick, cameraController->getCamera()->getTranslation());
	
	Quaternion alignment = glm::angleAxis(cameraController->getAzimuth() + glm::radians(90.0f), Vector3(0, 1, 0));
	Quaternion tilt = glm::rotation(Vector3(0, 1, 0), -direction);
	Quaternion rotation = glm::normalize(tilt * alignment);
	
	Vector3 translation = pick + rotation * Vector3(0, -distance + hoverDistance, 0);
	
	modelInstance.setTranslation(translation);
	modelInstance.setRotation(rotation);
	*/
	modelInstance.setActive(active);
	
	// Update tweener
	tweener->update(dt);
	
	float lensDistance = (focused) ? focusedDistance : unfocusedDistance;
	if (!ascentTween->isStopped())
	{
		lensDistance = ascentTween->getTweenValue();
	}
	else if (!descentTween->isStopped())
	{
		lensDistance = descentTween->getTweenValue();
	}
	
	//Quaternion alignment = glm::angleAxis(cameraController->getAzimuth() + glm::radians(90.0f), Vector3(0, 1, 0));
	Quaternion alignment = glm::rotation(Vector3(0, 1, 0), -sunDirection) * glm::angleAxis(glm::radians(90.0f), Vector3(0, 1, 0));
	Quaternion rotation = glm::normalize(alignment);
	Vector3 translation = pick + sunDirection * -lensDistance;
	
	modelInstance.setTranslation(translation);
	modelInstance.setRotation(rotation);
}

void Lens::focus()
{
	ascentTween->stop();
	descentTween->reset();
	descentTween->start();
}

void Lens::unfocus()
{
	descentTween->stop();
	focused = false;
	ascentTween->reset();
	ascentTween->start();
}

void Lens::setSunDirection(const Vector3& direction)
{
	sunDirection = direction;
}

Brush::Brush(const Model* model)
{
	// Allocate pose and initialize to bind pose
	pose = new Pose(model->getSkeleton());
	pose->reset();
	pose->concatenate();
	
	// Setup model instance
	modelInstance.setModel(model);
	modelInstance.setPose(pose);
	
	hoverDistance = 0.5f;
	
	// Setup timing
	float descentDuration = 0.1f;
	float ascentDuration = 0.1f;
	
	// Allocate tweener and and setup tweens
	tweener = new Tweener();
	descentTween = new Tween<float>(EaseFunction::OUT_CUBIC, 0.0f, descentDuration, hoverDistance, -hoverDistance);
	ascentTween = new Tween<float>(EaseFunction::OUT_CUBIC, 0.0f, ascentDuration, 0.0f, hoverDistance);
	descentTween->setEndCallback
	(
		[this](float t)
		{
			descended = true;
		}
	);
	tweener->addTween(descentTween);
	tweener->addTween(ascentTween);
	descended = false;
	
	oldPick = pick;
	tiltAngle = 0.0f;
	targetTiltAngle = 0.0f;
	tiltAxis = Vector3(1.0f, 0.0f, 0.0f);
	targetTiltAxis = tiltAxis;
}

Brush::~Brush()
{
	delete pose;
	delete descentTween;
	delete ascentTween;
	delete tweener;
}

void Brush::update(float dt)
{
	modelInstance.setActive(active);
	
	// Update tweener
	tweener->update(dt);
	
	float brushDistance = (descended) ? 0.0f : hoverDistance;
	if (!ascentTween->isStopped())
	{
		brushDistance = ascentTween->getTweenValue();
	}
	else if (!descentTween->isStopped())
	{
		brushDistance = descentTween->getTweenValue();
	}
	
	targetTiltAngle = 0.0f;
	if (descended)
	{
		Vector3 difference = pick - oldPick;
		float distanceSquared = glm::dot(difference, difference);
		
		if (distanceSquared > 0.005f)
		{
			float maxDistance = 0.25f;
			float maxTiltAngle = glm::radians(45.0f);
			float distance = std::sqrt(distanceSquared);
			float tiltFactor = std::min<float>(maxDistance, distance) / maxDistance;
			
			targetTiltAngle = maxTiltAngle * tiltFactor;
			targetTiltAxis = glm::normalize(Vector3(difference.z, 0.0f, -difference.x));
		}
	}
	
	float angleInterpolationFactor = 0.1f / (1.0 / 60.0f) * dt;
	float axisInterpolationFactor = 0.2f / (1.0 / 60.0f) * dt;
	tiltAngle = glm::mix(tiltAngle, targetTiltAngle, angleInterpolationFactor);
	tiltAxis = glm::mix(tiltAxis, targetTiltAxis, axisInterpolationFactor);
	
	Quaternion tilt = glm::angleAxis(tiltAngle, tiltAxis);
	
	Quaternion alignment = glm::angleAxis(cameraController->getAzimuth(), Vector3(0, 1, 0));
	Quaternion rotation = glm::normalize(tilt);
	Vector3 translation = pick + Vector3(0, brushDistance, 0);
	
	modelInstance.setTranslation(translation);
	modelInstance.setRotation(rotation);
	
	oldPick = pick;
}

void Brush::press()
{
	ascentTween->stop();
	descentTween->reset();
	descentTween->start();
	
}

void Brush::release()
{
	descentTween->stop();
	descended = false;
	ascentTween->reset();
	ascentTween->start();
}
