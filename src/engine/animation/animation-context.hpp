// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_ANIMATION_CONTEXT_HPP
#define ANTKEEPER_ANIMATION_ANIMATION_CONTEXT_HPP

#include <entt/entt.hpp>

/// Context for animation track output functions.
struct animation_context
{
	/// Handle to the entity being animated.
	entt::handle handle;
};

#endif // ANTKEEPER_ANIMATION_ANIMATION_CONTEXT_HPP
