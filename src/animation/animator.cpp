/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "animator.hpp"
#include "animation/animation.hpp"
#include <stdexcept>

animator::animator():
	animating(0)
{}

void animator::animate(double dt)
{
	// Advance animations
	++animating;
	for (animation_base* animation: animations)
	{
		animation->advance(dt);
	}
	--animating;
}

void animator::add_animation(animation_base* animation)
{
	if (animating)
		throw std::runtime_error("Attempting to add animation to animator while animating");
	
	animations.emplace(animation);
}

void animator::remove_animation(animation_base* animation)
{
	if (animating)
		throw std::runtime_error("Attempting to remove animation from animator while animating");
		
	auto it = animations.find(animation);
	if (it != animations.end())
	{
		animations.erase(it);
	}
}

void animator::remove_animations()
{
	if (animating)
		throw std::runtime_error("Attempting to remove animations from animator while animating");
	
	animations.clear();
}
