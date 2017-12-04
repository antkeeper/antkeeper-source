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

#include "agent.hpp"

Agent::Agent():
	navmeshTriangle(nullptr),
	barycentricPosition(0.0f),
	position(0.0f),
	forward(0, 0, -1),
	up(0, 1, 0),
	right(1, 0, 0),
	rotation(1, 0, 0, 0)
	//wanderDirection(0, 0, -1),
	//velocity(0.0f)
{}

/*
void Agent::applyForce(const Vector3& force)
{
	acceleration += force;
}

void Agent::updateVelocity()
{
	// Limit acceleration
	acceleration = limit(acceleration / mass, maxAcceleration);
	
	// Add acceleration to velocity and limit
	velocity = limit(velocity + acceleration, maxSpeed);
	
	// Reset acceleration to zero
	acceleration = Vector3(0.0f);
}

Vector3 Agent::wander(float dt)
{
	// Calculate center of wander circle
	Vector3 wanderCircleCenter = position + forward * wanderCircleDistance;
	
	// Calculate wander force
	Vector3 target = wanderCircleCenter + wanderDirection * wanderCircleRadius;
	
	// Rotate wander direction by a random displacement angle
	float displacement = frand(-wanderRate * 0.5f, wanderRate * 0.5f);
	wanderDirection = glm::normalize(glm::angleAxis(displacement, up) * wanderDirection);
	
	return seek(target);
}

Vector3 Agent::seek(const Vector3& target) const
{
	Vector3 desiredVelocity = glm::normalize(target - position) * maxSpeed;
	return desiredVelocity - velocity;
}

Vector3 Agent::flee(const Vector3& target) const
{
	Vector3 desiredVelocity = glm::normalize(position - target) * maxSpeed;
	return desiredVelocity - velocity;
}

Vector3 Agent::containment(const Vector3& probe) const
{
	std::vector<Navmesh::Step> traversal;
	Navmesh::traverse(navmeshTriangle, barycentricPosition, probe, &traversal);
	
	if (traversal.empty())
	{
		return Vector3(0.0f);
	}
	
	const Navmesh::Step& step = traversal.back();
	
	// If not on edge or on connected edge
	if (step.edge == nullptr || step.edge->symmetric != nullptr)
	{
		return Vector3(0.0f);
	}
	
	// Calculate difference between probe position and position on edge
	//Vector3 end = cartesian(step.end,
	//	step.triangle->edge->vertex->position,
	//	step.triangle->edge->next->vertex->position,
	//	step.triangle->edge->previous->vertex->position);
	
	//Vector3 difference = probe - end;
	
	//float depth = 0.0f;
	//if (nonzero(difference))
	//{
	//	depth = glm::length(difference);
	//}
	
	// Calculate edge normal
	const Vector3& a = step.edge->vertex->position;
	const Vector3& b = step.edge->next->vertex->position;
	Vector3 ab = glm::normalize(b - a);
	Vector3 edgeNormal = glm::cross(up, ab);
	
	// Calculate reflection vector of forward vector and edge normal
	//Vector3 reflection = glm::reflect(forward, edgeNormal);
	
	//Vector3 target = cartesian(step.end,
	//	step.triangle->edge->vertex->position,
	//	step.triangle->edge->next->vertex->position,
	//	step.triangle->edge->previous->vertex->position) + reflection * 0.1f;
	
	//std::cout << "reflection: " << reflection.x << ", " << reflection.y << ", " << reflection.z << std::endl;
	
	return edgeNormal;
}

Vector3 Agent::separation(const std::list<Agent*>& neighbors) const
{
	Vector3 force(0.0f);
	
	for (Agent* neighbor: neighbors)
	{
		Vector3 difference = position - neighbor->position;
		
		float distanceSquared = glm::dot(difference, difference);
		if (distanceSquared > 0.0f && distanceSquared < separationRadiusSquared)
		{			
			force += difference * (1.0f / distanceSquared);
		}
	}
	
	if (nonzero(force))
	{
		force = glm::normalize(force);
	}
	
	return force;
}
*/

void Agent::setPosition(Navmesh::Triangle* triangle, const Vector3& position)
{
	// Update navmesh triangle and position
	navmeshTriangle = triangle;
	barycentricPosition = position;
	
	// Convert navmesh-space barycentric position to world-space cartesian position
	const Vector3& a = triangle->edge->vertex->position;
	const Vector3& b = triangle->edge->next->vertex->position;
	const Vector3& c = triangle->edge->previous->vertex->position;
	this->position = cartesian(position, a, b, c);
}

void Agent::setOrientation(const Vector3& newForward, const Vector3& newUp)
{
	// Calculate alignment quaternion
	Quaternion alignment = glm::rotation(up, newUp);
	
	// Rebuild vector basis
	forward = newForward;
	right = glm::normalize(glm::cross(newUp, forward));
	up = glm::cross(forward, right);
	
	// Calculate rotation quaternion from vector basis
	rotation = glm::normalize(glm::quat_cast(Matrix3(right, up, forward)));
	
	// Align wander direction
	//wanderDirection = glm::normalize(project_on_plane(alignment * wanderDirection, Vector3(0.0f), up));
}

/*
void Agent::setMaxSpeed(float speed)
{
	maxSpeed = speed;
}

void Agent::setVelocity(const Vector3& velocity)
{
	this->velocity = velocity;
}

void Agent::setWanderCircleDistance(float distance)
{
	wanderCircleDistance = distance;
}

void Agent::setWanderCircleRadius(float radius)
{
	wanderCircleRadius = radius;
}

void Agent::setWanderRate(float angle)
{
	wanderRate = angle;
}

void Agent::setSeparationRadius(float radius)
{
	separationRadius = radius;
	separationRadiusSquared = separationRadius * separationRadius;
}
*/

/** EXAMPLE USAGE
Vector3 wanderForce = wander(dt) * wanderWeight;
Vector3 fleeForce = flee(mouse) * fleeWeight;
Vector3 steerForce = wanderForce + fleeForce;
steer(steerForce);
**/
