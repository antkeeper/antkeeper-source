// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_SCENE_COMPONENT_HPP
#define ANTKEEPER_GAME_SCENE_COMPONENT_HPP

#include <engine/scene/object.hpp>
#include <cstdint>
#include <memory>

struct scene_component
{
	std::shared_ptr<scene::object_base> object;
	std::uint8_t layer_mask{0b00000001};
};

#endif // ANTKEEPER_GAME_SCENE_COMPONENT_HPP
