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

#ifndef ANT_HPP
#define ANT_HPP

#include <vector>
#include "navmesh.hpp"
#include "agent.hpp"

#include <emergent/emergent.hpp>
using namespace Emergent;

class Colony;
class Gait;

/**
 * An individual ant which belongs to a colony.
 */
class Ant: public Agent
{
public:
	/**
	 * Named constants corresponding to leg indices.
	 *
	 *      \_/
	 * L1 --| |-- R1
	 * L2 --| |-- R2
	 * L3 --|_|-- R3
	 */
	enum class LegIndex
	{
		L1,
		L2,
		L3,
		R1,
		R2,
		R3
	};
	
	enum class State
	{
		IDLE,
		WANDER,
		DEAD,
		SUSPENDED
	};
	
	/**
	 * Creates an instance of Ant.
	 */
	Ant(Colony* colony);
	~Ant();
	
	void animate();
	
	void suspend(const Vector3& suspensionPoint, const Quaternion& suspensionRotation);
	
	void move(const Vector3& velocity);
	
	void turn(float angle);
	
	void update(float dt);
	
	void setState(Ant::State state);
	
	const Colony* getColony() const;
	Colony* getColony();
	const Transform& getTransform() const;
	
	const ModelInstance* getModelInstance() const;
	ModelInstance* getModelInstance();
	
private:
	Vector3 forage(const Vector3& leftReceptor, const Vector3& rightReceptor);
	
	/**
	 * Calculates the surface normal averaged between the surface normals at each of the ant's grounded feet.
	 */
	Vector3 calculateAverageSurfaceNormal() const;
	
	void updateTransform();
	
	Colony* colony;
	Ant::State state;
	float animationTime;
	
	Transform transform;
	ModelInstance modelInstance;
	Pose* pose;
};

inline const Colony* Ant::getColony() const
{
	return colony;
}

inline Colony* Ant::getColony()
{
	return colony;
}

inline const Transform& Ant::getTransform() const
{
	return transform;
}

inline const ModelInstance* Ant::getModelInstance() const
{
	return &modelInstance;
}

inline ModelInstance* Ant::getModelInstance()
{
	return &modelInstance;
}

#endif // ANT_HPP