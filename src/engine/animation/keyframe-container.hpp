// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_KEYFRAME_CONTAINER_HPP
#define ANTKEEPER_ANIMATION_KEYFRAME_CONTAINER_HPP

#include <engine/animation/keyframe.hpp>
#include <engine/animation/keyframe-time-comparator.hpp>
#include <set>

/** Set of keyframes, sorted by time. */
using keyframe_container = std::set<keyframe, keyframe_time_comparator>;

#endif // ANTKEEPER_ANIMATION_KEYFRAME_CONTAINER_HPP
