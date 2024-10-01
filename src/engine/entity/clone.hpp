// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ENTITY_CLONE_HPP
#define ANTKEEPER_ENTITY_CLONE_HPP

#include <engine/entity/registry.hpp>
#include <engine/entity/id.hpp>

namespace entity {

/// Clones all the components of an entity.
/// @param registry Entity registry.
/// @param source Source entity ID.
/// @param destination Destination entity ID.
void clone(entity::registry& registry, entity::id source, entity::id destination);

} // namespace entity

#endif // ANTKEEPER_ENTITY_CLONE_HPP
