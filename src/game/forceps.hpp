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

#ifndef FORCEPS_HPP
#define FORCEPS_HPP

#include "tool.hpp"

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
	Forceps(const Model* model, Animator* animator);
	
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
	 * Returns the current state of the forceps.
	 */
	Forceps::State getState() const;

private:
	Forceps::State state;
	Pose* pose;
	const AnimationClip<Transform>* pinchClip;
	const AnimationClip<Transform>* releaseClip;
	Animation<Transform> pinchAnimation;
	Animation<Transform> releaseAnimation;
	float hoverDistance;
	Vector3 translationBottom;
	Vector3 translationTop;
	Quaternion rotationTop;
	Quaternion rotationBottom;
	bool flipRotation;
	bool pinched;
	bool wasActive;
};

inline Forceps::State Forceps::getState() const
{
	return state;
}

#endif // FORCEPS_HPP
