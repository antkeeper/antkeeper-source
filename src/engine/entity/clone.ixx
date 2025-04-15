// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.entity.clone;
import engine.entity.id;
import engine.entity.registry;

export namespace engine::entity
{
	/// Clones all the components of an entity.
	/// @param registry Entity registry.
	/// @param source Source entity ID.
	/// @param destination Destination entity ID.
	void clone(entity::registry& registry, entity::id source, entity::id destination);
}
