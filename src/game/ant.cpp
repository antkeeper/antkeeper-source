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

#include "ant.hpp"
#include "colony.hpp"
#include "pheromone-matrix.hpp"
#include <cmath>

float FRAMES_PER_SECOND = 60;
float TIMESTEP = 1.0f / FRAMES_PER_SECOND;
float ANT_LENGTH = 0.5f; // 0.5 cm, head to abdomen (not including legs / antennae)
float ANT_COLLISION_RADIUS = ANT_LENGTH * 1.25f;
float RECEPTOR_RADIUS = 0.4f;
float RECEPTOR_SEPARATION = 0.882f;
float RECEPTOR_DISTANCE = 0.588f;
float MOUTH_DISTANCE = 0.2646f;
float BITE_RADIUS = 0.0294f;
float FOOD_PARTICLE_RADIUS = 0.1176f;
float MAX_RECEPTOR_NOISE = 0.05f; // essentially an epsilon
float MAX_EXCITEMENT = 1.0f;
float MAX_PHEROMONE_TURNING_ANGLE = glm::radians(8.5f);
float MIN_WALK_TIME = 0.5f; // seconds
float MAX_WALK_TIME = 8.0f; // seconds
float MIN_REST_TIME = 0.15f;
float MAX_REST_TIME = 0.7f;
float MIN_CHEW_TIME = 0.25f;
float MAX_CHEW_TIME = 0.5f;
float DEEXCITEMENT_FACTOR = 0.999f; // This should probably always be less than the evaporation factor
float CALM_FACTOR = 0.995f;
float MAX_WALK_FORCE = 1.5;
float MAX_PANIC_FORCE = 0.1029f;
float MAX_WALK_SPEED = 3.0f; // cm/s
float MAX_PANIC_SPEED = 8.82f; // cm/s
float PANIC_RADIUS = 7.35f;
float WANDER_CIRCLE_DISTANCE = 0.441f;
float WANDER_CIRCLE_RADIUS = 0.0294f;
float MAX_WANDER_ANGLE = 0.15f;

inline float fwrap(float angle, float limit)
{
    return angle - std::floor(angle / limit) * limit;
}

Ant::Ant(Colony* colony):
	colony(colony),
	state(Ant::State::IDLE),
	transform(Transform::getIdentity()),
	pose(nullptr)
{
	pose = new Pose(colony->getAntModel()->getSkeleton());
	pose->reset();
	pose->concatenate();
	
	modelInstance.setModel(colony->getAntModel());
	modelInstance.setPose(pose);
	
	animationTime = frand(0.0f, 60.0f);
	
	velocity = Vector3(0);
	acceleration = Vector3(0);
	wanderDirection = getForward();
	excitement = MAX_EXCITEMENT;
}

Ant::~Ant()
{
	delete pose;
}

void Ant::animate()
{
	colony->getTripodGaitAnimation()->animate(pose, animationTime);
	pose->concatenate();
	animationTime = fwrap(animationTime + 4.0f, colony->getTripodGaitAnimation()->getEndTime());
}

void Ant::suspend(const Vector3& suspensionPoint, const Quaternion& suspensionRotation)
{
	transform.translation = suspensionPoint;
	transform.rotation = suspensionRotation;
	modelInstance.setTransform(transform);
}

void Ant::move(const Vector3& velocity)
{
	std::vector<Navmesh::Step> traversal;
	Navmesh::traverse(getNavmeshTriangle(), getBarycentricPosition(), velocity, &traversal);
	
	if (!traversal.empty())
	{
		const Navmesh::Step& step = traversal.back();
		
		if (step.start != step.end)
		{
			if (step.triangle != getNavmeshTriangle())
			{
				Quaternion alignment = glm::rotation(getNavmeshTriangle()->normal, step.triangle->normal);
				Vector3 newForward = glm::normalize(project_on_plane(alignment * getForward(), Vector3(0.0f), step.triangle->normal));
				
				setOrientation(newForward, step.triangle->normal);
			}
		}
		
		setPosition(step.triangle, step.end);
	}
}

void Ant::turn(float angle)
{
	// Rotate forward vector
	Vector3 newForward = glm::normalize(glm::angleAxis(angle, getUp()) * getForward());
	setOrientation(newForward, getUp());
}

void Ant::update(float dt)
{
	float probeLateralOffset = 0.1f;
	float probeForwardOffset = 0.3f;
	
	animate();
	
    // Calculate positions of receptors
	receptorL = getPosition() + getForward() * RECEPTOR_DISTANCE;
	receptorR = receptorL;
	receptorL -= getRight() * RECEPTOR_SEPARATION * 0.5f;
	receptorR += getRight() * RECEPTOR_SEPARATION * 0.5f;
	
	// Steering
	if (state == Ant::State::WANDER)
	{
		//setWanderCircleDistance(4.0f);
		//setWanderCircleRadius(0.3f);
		//setWanderRate(glm::radians(90.0f));
		//setSeparationRadius(0.5f);
		//setMaxSpeed(0.025f);
		
		// Calculate wander force
		Vector3 wanderForce = wander() * 1.5f;
		Vector3 followForce = follow() * 3.0f;
		
		// Setup containment probes
		//Vector3 leftProbe = getForward() * probeForwardOffset - getRight() * probeLateralOffset;
		//Vector3 rightProbe = getForward() * probeForwardOffset + getRight() * probeLateralOffset;
		
		// Calculate containment force
		//Vector3 containmentForce = containment(leftProbe) + containment(rightProbe);
		
		// Determine neighbors
		//float neighborhoodSize = 2.0f;
		//AABB neighborhoodAABB(getPosition() - Vector3(neighborhoodSize * 0.5f), getPosition() + Vector3(neighborhoodSize * 0.5f));
		//std::list<Agent*> neighbors;
		//colony->queryAnts(neighborhoodAABB, &neighbors);
				
		// Calculate separation force
		//Vector3 separationForce = separation(neighbors);
		
		applyForce(wanderForce);
		applyForce(followForce);
		
		float maxSpeed = MAX_WALK_SPEED * TIMESTEP;
		
		// Limit acceleration
		float accelerationMagnitudeSquared = glm::dot(acceleration, acceleration);
		if (accelerationMagnitudeSquared > MAX_WALK_FORCE * MAX_WALK_FORCE)
		{
			acceleration = glm::normalize(acceleration) * MAX_WALK_FORCE;
		}
		
		// Accelerate
		velocity += acceleration;
		
		// Limit speed
		float speedSquared = glm::dot(velocity, velocity);
		if (speedSquared > maxSpeed * maxSpeed)
		{
			velocity = glm::normalize(velocity) * maxSpeed;
		}
		
		Vector3 direction = glm::normalize(velocity);
		setOrientation(direction, getUp());
		
		// Deposit pheromones
		Vector2 position2D = Vector2(getPosition().x, getPosition().z);
		colony->getHomingMatrix()->deposit(position2D, excitement);
		excitement *= DEEXCITEMENT_FACTOR;
		
		// Move ant
		move(velocity);
	}
	else if (state == Ant::State::IDLE)
	{
		velocity = Vector3(0.0f);
		
		// Move ant
		move(velocity);
	}
	
	// Update transform
	if (state == Ant::State::WANDER || state == Ant::State::IDLE)
	{
		transform.translation = getPosition();
		transform.rotation = getRotation();
			
		// Update model instance
		modelInstance.setTransform(transform);
	}
	
	// Locomotion
	
	/*
	As the ant moves forward, legs in the stance phase are kept grounded via IK. If IK constraints are violated, the swinging legs are grounded
	and the grounded legs begin swinging.
	
	Two poses are loaded from the model file: midswing and touchdown.
	
	touchdown is the pose in which all legs are at the end of their swing phases and need to be grounded
	midswing is the pose in which all legs are at the highest point in their swing phases
	
	when a grounded leg enters the swing phases, its current pose is saved as the liftoff pose, then an animation is created using the liftoff pose, midswing pose, and touchdown pose.
	*/
}

/*
Vector3 Ant::forage(const Vector3& leftReceptor, const Vector3& rightReceptor)
{
	float leftSignal = 0.0f;
	float rightSignal = 0.0f;
	
	// Detect pheromones with left receptor
	std::list<Pheromone*> leftPheromones;
	colony->getPheromoneOctree()->query(AABB(leftReceptor, leftReceptor), &leftPheromones);
	for (Pheromone* pheromone: leftPheromones)
	{
		Vector3 difference = pheromone->getPosition() - rightReceptor;
		
		float distanceSquared = glm::dot(difference, difference);
		if (distanceSquared <= pheromone->getRadiusSquared())
		{
			// Calculate attenuated pheromone strength using inverse-square law
			float strength = pheromone->getStrength() / ((distanceSquared == 0.0f) ? 1.0f : distanceSquared);
			leftSignal += strength;
		}
	}
	
	// Detect pheromones with right receptor
	std::list<Pheromone*> rightPheromones;
	colony->getPheromoneOctree()->query(AABB(rightReceptor, rightReceptor), &rightPheromones);
	for (Pheromone* pheromone: rightPheromones)
	{
		Vector3 difference = pheromone->getPosition() - rightReceptor;
		
		float distanceSquared = glm::dot(difference, difference);
		if (distanceSquared <= pheromone->getRadiusSquared())
		{
			// Calculate attenuated pheromone strength using inverse-square law
			float strength = pheromone->getStrength() / ((distanceSquared == 0.0f) ? 1.0f : distanceSquared);
			rightSignal += strength;
		}
	}
	
	// Add noise
	const float maxNoise = 0.1f;
	leftSignal += frand(0.0f, maxNoise);
	rightSignal += frand(0.0f, maxNoise);
	
    if (leftSignal + rightSignal > 0.0f)
    {
		const float maxPheromoneTurningAngle = 0.1f;
		
		// Use Weber's law (Perna et al.) to calculate turning angle based on pheromone signals
		float turningAngle = maxPheromoneTurningAngle * ((leftSignal - rightSignal) / (leftSignal + rightSignal));
    }
	
	return Vector3(0.0f);
}
*/

void Ant::setState(Ant::State state)
{
	this->state = state;
}

Vector3 Ant::seek(const Vector3& target)
{
	Vector3 steer(0.0f);
	Vector3 difference = target - getPosition();
	
	float distanceSquared = glm::dot(difference, difference);
	if (distanceSquared > 0.0f)
	{
		float maxForce = MAX_WALK_FORCE;
		
		steer = glm::normalize(difference) * maxForce - velocity;
	}
	
	return steer;
}

Vector3 Ant::flee(const Vector3& target)
{
	return -seek(target);
}

Vector3 Ant::wander()
{
	// Determine center of wander circle
	Vector3 center = getPosition() + getForward() * WANDER_CIRCLE_DISTANCE;
	
	// Calculate wander target
	Vector3 target = center + wanderDirection * WANDER_CIRCLE_RADIUS;
	
	// Rotate wander direction by a random displacement angle
	float displacement = frand(-MAX_WANDER_ANGLE, MAX_WANDER_ANGLE);
	wanderDirection = glm::normalize(glm::angleAxis(displacement, getUp()) * wanderDirection);
	
	return seek(target);
}

Vector3 Ant::follow()
{
	const PheromoneMatrix* pheromoneMatrix = colony->getRecruitmentMatrix();
	
	Vector2 receptorL2D = Vector2(receptorL.x, receptorL.z);
	Vector2 receptorR2D = Vector2(receptorR.x, receptorR.z);

	float signalL = pheromoneMatrix->query(receptorL2D, RECEPTOR_RADIUS);
	signalL += frand(0.0f, MAX_RECEPTOR_NOISE);

	float signalR = pheromoneMatrix->query(receptorR2D, RECEPTOR_RADIUS);
	signalR += frand(0.0f, MAX_RECEPTOR_NOISE);
	
	if (signalL + signalR > 0.0f)
	{
		float angle = -MAX_PHEROMONE_TURNING_ANGLE * ((signalL - signalR) / (signalL + signalR));
		
		Vector3 steer = glm::normalize(glm::angleAxis(angle, getUp()) * getForward());
		return steer;
	}

	return Vector3(0.0f);
}

void Ant::applyForce(const Vector3& force)
{
	acceleration += force;
}
