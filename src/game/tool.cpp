#include "tool.hpp"
#include "ant.hpp"
#include "colony.hpp"
#include "../camera-controller.hpp"
#include <iostream>
#include <list>

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
	
	// Allocate tweener and and setup tweens
	tweener = new Tweener();
	descentTween = new Tween<float>(EaseFunction::OUT_CUBIC, 0.0f, 0.0667f, 1.0f, -1.0f);
	ascentTween = new Tween<float>(EaseFunction::IN_CUBIC, 0.0f, 0.0667f, 0.0f, 1.0f);
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
	// Update tweener
	tweener->update(dt);
	
	// Determine distance from pick point
	float forcepsDistance = descentTween->getStartValue();
	if (!ascentTween->isStopped())
	{
		forcepsDistance = ascentTween->getTweenValue();
	}
	else if (!descentTween->isStopped())
	{
		forcepsDistance = descentTween->getTweenValue();
	}
	
	Quaternion rotation = glm::angleAxis(cameraController->getAzimuth(), Vector3(0, 1, 0)) *
		glm::angleAxis(glm::radians(15.0f), Vector3(0, 0, -1));
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
		if (suspendedAnt != nullptr)
		{
			// Suspend ant
			suspendedAnt->suspend(modelInstance.getTranslation());
		}
	}
	else if (state == Forceps::State::PINCHING)
	{
		// Perform pinch animation
		pinchAnimation->animate(pose, animationTime);
		pose->concatenate();
		
		// If pinch animation is finished
		if (animationTime >= pinchAnimation->getEndTime())
		{
			// If an ant was targeted
			if (targetedAnt != nullptr)
			{
				// Suspend targeted ant
				suspendedAnt = targetedAnt;
				suspendedAnt->setState(Ant::State::SUSPENDED);
				suspendedAnt->suspend(modelInstance.getTranslation());
				targetedAnt = nullptr;
			}
			
			// Change to pinched state
			state = Forceps::State::PINCHED;
		}
	}
	
	// Increment animation time
	//animationTime += dt;
	animationTime += 2.5f;
}

void Forceps::setColony(Colony* colony)
{
	this->colony = colony;
}

void Forceps::setCameraController(const SurfaceCameraController* cameraController)
{
	this->cameraController = cameraController;
}

void Forceps::setPick(const Vector3& pick)
{
	this->pick = pick;
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
	}
	
	// Start descent tweener
	descentTween->start();
}

void Forceps::release()
{
	// Change state to releasing
	state = Forceps::State::RELEASING;
	animationTime = 0.0f;
	
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
