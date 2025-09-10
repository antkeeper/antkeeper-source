// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_UTILITY_TERRAIN_HPP
#define ANTKEEPER_GAME_UTILITY_TERRAIN_HPP

#include <engine/gl/image.hpp>
#include <engine/render/material.hpp>
#include <engine/math/vector.hpp>
#include <engine/math/transform.hpp>
#include <engine/entity/id.hpp>
#include <engine/entity/registry.hpp>
#include <memory>

using namespace engine;

/// Generates terrain entities from a heightmap.
/// @param heightmap Heightmap from which the terrain should be generated.
/// @param subdivisions Number of heightmap subdivisions on the x- and z-axes. Determines the number of terrain entities generated.
/// @param transform Translation, rotation, and scale of the terrain.
/// @param material Terrain material.
/// @return Entity ID of the generated terrain grid.
/// @exception std::invalid_argument Failed to generate terrain from null heightmap.
/// @exception std::runtime_error Heightmap size less than 2x2.
/// @exception std::runtime_error Heightmap subdivision failed.
entity::id generate_terrain(entity::registry& registry, std::shared_ptr<gl::image_2d> heightmap, const math::uvec2& subdivisions, const math::transform<float>& transform, std::shared_ptr<render::material> material);

#endif // ANTKEEPER_GAME_UTILITY_TERRAIN_HPP
