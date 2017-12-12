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

#ifndef TOOL_HPP
#define TOOL_HPP

#include "../ui/tween.hpp"

#include <emergent/emergent.hpp>
using namespace Emergent;

class Ant;
class Colony;
class Navmesh;
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
	 * @param dt Application timestep.
	 */
	virtual void update(float dt) = 0;
	
	/**
	 * Activates or deactivates the tool.
	 */
	virtual void setActive(bool active);
	
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

inline const ModelInstance* Tool::getModelInstance() const
{
	return &modelInstance;
}

inline ModelInstance* Tool::getModelInstance()
{
	return &modelInstance;
}

/**
 * The forceps tool can pick up ants and place them anywhere in the world.
 */
class Forceps: public Tool
{
public:
	enum class State
	{
		RELEASED,
		RELEASING,
		PINCHED,
		PINCHING
	};
	
	/**
	 * Creates an instance of Forceps.
	 *
	 * @param model Forceps model
	 */
	Forceps(const Model* model);
	
	/**
	 * Destroys an instance of Forceps.
	 */
	~Forceps();
	
	/**
	 * Updates the forceps.
	 *
	 * @param dt Application timestep.
	 */
	virtual void update(float dt);
	
	/**
	 * Pinches the forceps.
	 */
	void pinch();
	
	/**
	 * Releases the forceps.
	 */
	void release();
	
	/**
	 * Associates a colony with this forceps.
	 *
	 * @param colony Colony with which to associate.
	 */
	void setColony(Colony* colony);
	
	void setNavmesh(Navmesh* navmesh);
	
	/**
	 * Returns the current state of the forceps.
	 */
	Forceps::State getState() const;
	
	/**
	 * Returns the suspended ant, if any.
	 */
	Ant* getSuspendedAnt() const;

private:
	Forceps::State state;
	Pose* pose;
	const Animation* pinchAnimation;
	const Animation* releaseAnimation;
	float animationTime;
	float animationTimeStep;
	float hoverDistance;
	Tweener* tweener;
	Tween<float>* descentTween;
	Tween<float>* ascentTween;
	Vector3 translationBottom;
	Vector3 translationTop;
	Quaternion rotationTop;
	Quaternion rotationBottom;
	bool flipRotation;
	Colony* colony;
	Ant* targetedAnt;
	Ant* suspendedAnt;
	Navmesh* navmesh;
};

inline Forceps::State Forceps::getState() const
{
	return state;
}

inline Ant* Forceps::getSuspendedAnt() const
{
	return suspendedAnt;
}

/**
 * The lens tool can be used to burn ants.
 *
 * @see https://taylorpetrick.com/blog/post/dispersion-opengl
 * @see https://taylorpetrick.com/portfolio/webgl/lense
 */
class Lens: public Tool
{
public:
	/**
	 * Creates an instance of Lens.
	 *
	 * @param model Lens model
	 */
	Lens(const Model* model);
	
	/**
	 * Destroys an instance of Lens.
	 */
	~Lens();
	
	/**
	 * Updates the lens.
	 *
	 * @param dt Application timestep.
	 */
	virtual void update(float dt);
	
	/**
	 * Activates or deactivates the lens.
	 */
	virtual void setActive(bool active);
	
	void focus();
	void unfocus();
	
	/**
	 * Associates a colony with this lens.
	 *
	 * @param colony Colony with which to associate.
	 */
	void setColony(Colony* colony);
	
	void setSunDirection(const Vector3& direction);
	
	/**
	 * Returns the spotlight.
	 */
	const Spotlight* getSpotlight() const;
	Spotlight* getSpotlight();
	
private:
	Spotlight spotlight;
	float unfocusedDistance;
	float focusedDistance;
	bool focused;
	Vector3 sunDirection;
	Tweener* tweener;
	Tween<float>* descentTween;
	Tween<float>* ascentTween;
	Colony* colony;
};

inline const Spotlight* Lens::getSpotlight() const
{
	return &spotlight;
}

inline Spotlight* Lens::getSpotlight()
{
	return &spotlight;
}

/**
 * The brush tool can paint pheromones on the terrain.
 */
class Brush: public Tool
{
public:
	Brush(const Model* model);
	~Brush();
	
	virtual void update(float dt);
	
	void press();
	void release();
	
	/**
	 * Associates a colony with this brush.
	 *
	 * @param colony Colony with which to associate.
	 */
	void setColony(Colony* colony);
	
private:
	void paint(const Vector2& position, float radius);
	
	Pose* pose;
	float hoverDistance;
	bool descended;
	Vector3 oldPick;
	
	Tweener* tweener;
	Tween<float>* descentTween;
	Tween<float>* ascentTween;
	float tiltAngle;
	float targetTiltAngle;
	Vector3 tiltAxis;
	Vector3 targetTiltAxis;
	Colony* colony;
};

#endif // TOOL_HPP
