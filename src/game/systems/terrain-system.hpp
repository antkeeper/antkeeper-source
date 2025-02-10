// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_TERRAIN_SYSTEM_HPP
#define ANTKEEPER_GAME_TERRAIN_SYSTEM_HPP

#include "game/systems/fixed-update-system.hpp"
#include "game/components/terrain-component.hpp"
#include <engine/entity/id.hpp>
#include <engine/gl/image.hpp>
#include <engine/math/transform.hpp>
#include <engine/math/vector.hpp>
#include <engine/render/model.hpp>
#include <engine/render/material.hpp>
#include <engine/geom/brep/brep-mesh.hpp>
#include <memory>

/// Generates terrain patches and performs terrain patch LOD selection.
class terrain_system: public fixed_update_system
{
public:
	~terrain_system() override = default;
	void fixed_update(entity::registry& registry, float t, float dt) override;
};

#endif // ANTKEEPER_GAME_TERRAIN_SYSTEM_HPP
