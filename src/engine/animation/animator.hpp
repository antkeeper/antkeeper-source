// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATOR_HPP
#define ANTKEEPER_ANIMATOR_HPP

#include <list>
#include <unordered_set>

class animation_base;

/**
 * Progresses a set of animations.
 */
class animator
{
public:
	/// Constructs an animator.
	animator();
	
	/**
	 * Progresses all active animations by @p dt.
	 *
	 * @param dt Delta time by which the animations will be progressed.
	 */
	void animate(float dt);
	
	/**
	 * Adds an animation to the animator.
	 *
	 * @param animation Animation to add.
	 */
	void add_animation(animation_base* animation);
	
	/**
	 * Removes an animation from the animator.
	 *
	 * @param animation Animation to remove.
	 */
	void remove_animation(animation_base* animation);
	
	/// Removes all animations from the animator.
	void remove_animations();
	
private:
	std::unordered_set<animation_base*> animations;
	std::list<animation_base*> animations_to_add;
	std::list<animation_base*> animations_to_remove;
	int animating;
};

#endif // ANTKEEPER_ANIMATOR_HPP
