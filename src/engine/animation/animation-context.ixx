// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.animation.animation_context;
import <entt/entt.hpp>;

export namespace engine::animation
{
	/// Context for animation track output functions.
	struct animation_context
	{
		/// Handle to the entity being animated.
		entt::handle handle;
	};
}
