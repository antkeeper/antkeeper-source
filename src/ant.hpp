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

#include <emergent/emergent.hpp>
using namespace Emergent;

class Gait;

class Ant
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
	
private:
	Transform transform;
	ModelInstance modelInstance;
	Pose* skeletonPose;
};

class Colony
{
public:
	Ant* spawn();
	
	const Model* getAntModel() const;
	Model* getAntModel();
	
	const Skeleton* getAntSkeleton() const;
	Skeleton* getSkeleton();

private:
	// Rendering
	Model* antModel;
	Skeleton* antSkeleton;
	
	// Locomotion
	float walkSpeed;
	float turnSpeed;
	Gait* tripodGait;
	Gait* rippleGait;
	Gait* slowWaveGait;
	
	
	std::vector<Ant*> ants;
};

#endif // ANT_HPP