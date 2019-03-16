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

#ifndef TOOL_HPP
#define TOOL_HPP

#include <emergent/emergent.hpp>
using namespace Emergent;

class OrbitCam;

/**
 * Abstract base class for tools. Tools are the only way for the user to interact with the world.
 */
class Tool
{
public:
	/**
	 * Creates an instance of Tool.
	 */
	Tool();
	
	/**
	 * Destroys an instance of Tool.
	 */
	virtual ~Tool();
	
	/**
	 * Updates the tool.
	 *
	 * @param dt Game timestep.
	 */
	virtual void update(float dt) = 0;
	
	/**
	 * Activates or deactivates the tool.
	 */
	void setActive(bool active);
	
	/**
	 * Sets the picking position.
	 *
	 * @param pick Picking position
	 */
	void setPick(const Vector3& pick);
	
	/**
	 * Sets the camera.
	 *
	 * @param camera Pointer to the camera.
	 */
	void setOrbitCam(const OrbitCam* orbitCam);
	
	bool isActive() const;

	const Vector3& getPick() const;
	
	/**
	 * Returns the model instance.
	 */
	const ModelInstance* getModelInstance() const;
	ModelInstance* getModelInstance();
	
protected:
	ModelInstance modelInstance;
	bool active;
	Vector3 pick;
	const OrbitCam* orbitCam;
};

inline bool Tool::isActive() const
{
	return active;
}

inline void Tool::setPick(const Vector3& pick)
{
	this->pick = pick;
}

inline const Vector3& Tool::getPick() const
{
	return pick;
}

inline const ModelInstance* Tool::getModelInstance() const
{
	return &modelInstance;
}

inline ModelInstance* Tool::getModelInstance()
{
	return &modelInstance;
}

#endif // TOOL_HPP
