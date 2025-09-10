// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_SCENE_OBJECT_COMPONENT_HPP
#define ANTKEEPER_GAME_SCENE_OBJECT_COMPONENT_HPP

#include <engine/scene/object.hpp>
#include <engine/utility/sized-types.hpp>
#include <memory>

using namespace engine;

/// Attaches a scene object, such as a mesh or light, to an entity.
struct scene_object_component
{
	/// Pointer to the scene object.
	std::shared_ptr<scene::object_base> object;

	/// Mask of layers in which the object is visible.
	u8 layer_mask{0b00000001};
};

#endif // ANTKEEPER_GAME_SCENE_OBJECT_COMPONENT_HPP
