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

#ifndef AGENT_HPP
#define AGENT_HPP

#include "navmesh.hpp"

#include <vector>

#include <emergent/emergent.hpp>
using namespace Emergent;

class Obstacle;


/*************88

Ant is an agent.

Ant combines steering behaviors with different weights.

I.E.

seek pheromones * 0.5
separation * 0.1
alignment * 0.1
cohesion * 0.1
followWall * 0.2

*/

/**
 * An agent which navigates on a navmesh.
 */
class Agent
{
public:
	Agent();
	
	/**
	 * Adds a force to the agent's acceleration vector.
	 *
	 * @param force Acceleration force
	 */
	void applyForce(const Vector3& force);
	
	/**
	 * Calculates velocity based on current acceleration vector, then resets acceleration to zero.
	 */
	void updateVelocity();
	
	/**
	 * Calculates steering force for the wander behavior.
	 */
	Vector3 wander(float dt);
	
	/**
	 * Calculates steering force for the seek behavior.
	 */
	Vector3 seek(const Vector3& target) const;
	
	/**
	 * Calculates steering force for the flee behavior.
	 */
	Vector3 flee(const Vector3& target) const;
	
	Vector3 containment(const Vector3& probe) const;
	
	Vector3 separation(const std::list<Agent*>& neighbors) const;
	
	Vector3 forage(const Vector3& leftProbe, const Vector3& rightProbe);
	
	
	void setMaxSpeed(float speed);
	void setVelocity(const Vector3& velocity);
	void setMaxAcceleration(float acceleration);
	void setMass(float mass);
	void setWanderCircleDistance(float distance);
	void setWanderCircleRadius(float radius);
	void setWanderRate(float angle);
	void setSeparationRadius(float radius);
	
	/**
	 * Sets the position of the agent on a navmesh.
	 *
	 * @param triangle Navmesh triangle on which the agent resides
	 * @param position Barycentric position on the specified triangle
	 */
	void setPosition(Navmesh::Triangle* triangle, const Vector3& position);
	
	/**
	 * Sets the orientation of the agent. This effectively updates the agent's vector basis and rotation quaternion.
	 *
	 * @param forward Normalized forward vector
	 * @param up Normalized up vector
	 */
	void setOrientation(const Vector3& newForward, const Vector3& newUp);
	
	/*
	Vector3 followWall();
	// or
	Vector3 followEdge();
	Vector3 avoidObstacle(const Obstacle* obstacle);
	
	Vector3 alignment(const std::vector<const Agent*>* neighbors);
	Vector3 cohesion(const std::vector<const Agent*>* neighbors);
	*/
	
	const Navmesh::Triangle* getNavmeshTriangle() const;
	Navmesh::Triangle* getNavmeshTriangle();
	const Vector3& getBarycentricPosition() const;
	const Vector3& getPosition() const;
	const Vector3& getForward() const;
	const Vector3& getUp() const;
	const Vector3& getRight() const;
	const Quaternion& getRotation() const;
	
	const Vector3& getVelocity() const;

private:
	Navmesh::Triangle* navmeshTriangle;
	Vector3 barycentricPosition;
	Vector3 position;
	Vector3 forward;
	Vector3 up;
	Vector3 right;
	Quaternion rotation;
	
	// Limits
	float maxSpeed;
	float maxAcceleration;
	
	// Steering forces
	float mass;
	Vector3 acceleration;
	Vector3 velocity;

	// Wander variables
	float wanderCircleDistance;
	float wanderCircleRadius;
	float wanderRate;
	Vector3 wanderDirection;
	float separationRadius;
	float separationRadiusSquared;
};

inline const Navmesh::Triangle* Agent::getNavmeshTriangle() const
{
	return navmeshTriangle;
}

inline Navmesh::Triangle* Agent::getNavmeshTriangle()
{
	return navmeshTriangle;
}

inline const Vector3& Agent::getBarycentricPosition() const
{
	return barycentricPosition;
}

inline const Vector3& Agent::getPosition() const
{
	return position;
}

inline const Vector3& Agent::getForward() const
{
	return forward;
}

inline const Vector3& Agent::getUp() const
{
	return up;
}

inline const Vector3& Agent::getRight() const
{
	return right;
}

inline const Quaternion& Agent::getRotation() const
{
	return rotation;
}

inline const Vector3& Agent::getVelocity() const
{
	return velocity;
}

#endif // AGENT_HPP