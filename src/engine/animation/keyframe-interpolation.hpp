// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_KEYFRAME_INTERPOLATION_HPP
#define ANTKEEPER_ANIMATION_KEYFRAME_INTERPOLATION_HPP

struct keyframe;

/// Interpolates between two keyframes using linear interpolation.
/// @param a First keyframe.
/// @param b Second keyframe.
/// @param time Interpolation time.
/// @return Interpolated value.
[[nodiscard]] float interpolate_keyframes_linear(const keyframe& a, const keyframe& b, float time) noexcept;

/// Interpolates between two keyframes using constant interpolation.
/// @param a First keyframe.
/// @param b Second keyframe.
/// @param time Interpolation time.
/// @return Value of @p a.
[[nodiscard]] float interpolate_keyframes_constant(const keyframe& a, const keyframe& b, float time) noexcept;

#endif // ANTKEEPER_ANIMATION_KEYFRAME_INTERPOLATION_HPP
