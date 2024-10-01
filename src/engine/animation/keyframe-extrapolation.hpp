// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_KEYFRAME_EXTRAPOLATION_HPP
#define ANTKEEPER_ANIMATION_KEYFRAME_EXTRAPOLATION_HPP

#include <engine/animation/keyframe-container.hpp>

/// Extrapolates keyframes by clamping to the values of the first or last keyframes.
/// @param keyframes Keyframes.
/// @param time Extrapolation time.
/// @return Extrapolated value.
/// @exception std::runtime_error Failed clamp extrapolation of keyframes: no keyframes provided.
[[nodiscard]] float extrapolate_keyframes_clamp(const keyframe_container& keyframes, float time);

#endif // ANTKEEPER_ANIMATION_KEYFRAME_EXTRAPOLATION_HPP
