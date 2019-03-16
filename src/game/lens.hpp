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

#ifndef LENS_HPP
#define LENS_HPP

#include "tool.hpp"

class ParticleSystem;

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
	Lens(const Model* model, Animator* animator);
	
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
	

	void focus();
	void unfocus();
	
	void setSunDirection(const Vector3& direction);
	void setParticleSystem(ParticleSystem* particleSystem);
	
	/**
	 * Returns the spotlight.
	 */
	const Spotlight* getSpotlight() const;
	Spotlight* getSpotlight();
	
private:
	Spotlight spotlight;
	float focusedDistance;
	float unfocusedDistance;
	float focusDuration;
	float unfocusDuration;
	float lensDistance;
	float lastDistance;
	bool focused;
	Vector3 sunDirection;
	Animation<float> focusAnimation;
	Animation<float> unfocusAnimation;
	AnimationClip<float> focusClip;
	AnimationClip<float> unfocusClip;
	bool wasActive;
	ParticleSystem* particleSystem;
};

inline const Spotlight* Lens::getSpotlight() const
{
	return &spotlight;
}

inline Spotlight* Lens::getSpotlight()
{
	return &spotlight;
}

#endif // LENS_HPP
