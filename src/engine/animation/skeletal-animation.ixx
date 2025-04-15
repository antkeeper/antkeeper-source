// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.animation.skeletal_animation;
import engine.animation.skeleton;
import engine.animation.animation_sequence;

export namespace engine::animation
{
	/// Binds an animation sequence to a skeleton.
	/// @param sequence Animation sequence to bind.
	/// @param skeleton Skeleton to which the sequence should be bound.
	void bind_skeletal_animation(animation_sequence& sequence, const animation::skeleton& skeleton);
}
