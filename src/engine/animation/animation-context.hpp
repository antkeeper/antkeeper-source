// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <entt/entt.hpp>

namespace engine::animation
{
	/// Context for animation track output functions.
	struct animation_context
	{
		/// Handle to the entity being animated.
		entt::handle handle;
	};
}
