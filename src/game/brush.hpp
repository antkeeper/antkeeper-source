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

#ifndef BRUSH_HPP
#define BRUSH_HPP

#include "tool.hpp"

class Brush: public Tool
{
public:
	Brush(const Model* model, Animator* animator);
	~Brush();
	
	/**
	 * Updates the brush.
	 *
	 * @param dt Game timestep.
	 */
	virtual void update(float dt);
	
	void press();
	void release();

	void setTiltParams(const Vector2& mousePosition, const Vector2& screenDimensions);

private:
	float pressedDistance;
	float releasedDistance;
	float tipDistance;
	float lastTipDistance;
	bool painting;

	float pressDuration;
	float releaseDuration;

	Animation<float> pressAnimation;
	Animation<float> releaseAnimation;
	AnimationClip<float> pressClip;
	AnimationClip<float> releaseClip;

	Vector2 mousePosition;
	Vector2 lastMousePosition;
	Vector2 screenDimensions;
	float speed;

	Vector2 velocity;
	bool wasActive;
};

#endif // BRUSH_HPP
