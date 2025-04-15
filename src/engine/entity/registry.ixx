// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.entity.registry;
export import <entt/entt.hpp>;

export namespace engine::entity
{
	/// Component registry type.
	using registry = entt::registry;
}
