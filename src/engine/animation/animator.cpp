// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/animator.hpp>
#include <engine/animation/animation.hpp>
#include <stdexcept>

animator::animator():
	animating(0)
{}

void animator::animate(float dt)
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
