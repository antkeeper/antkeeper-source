// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_SKELETAL_ANIMATION_HPP
#define ANTKEEPER_ANIMATION_SKELETAL_ANIMATION_HPP

class animation_sequence;
class skeleton;

/**
 * Binds an animation sequence to a skeleton.
 *
 * @note Animation context must be a pointer to a skeleton_pose.
 */
void bind_skeletal_animation(animation_sequence& sequence, const ::skeleton& skeleton);

#endif // ANTKEEPER_ANIMATION_SKELETAL_ANIMATION_HPP
