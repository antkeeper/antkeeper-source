/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANTKEEPER_GAME_TERRAIN_SYSTEM_HPP
#define ANTKEEPER_GAME_TERRAIN_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"
#include "game/components/terrain-component.hpp"
#include <engine/entity/id.hpp>
#include <engine/gl/image.hpp>
#include <engine/math/transform.hpp>
#include <engine/math/vector.hpp>
#include <engine/render/model.hpp>
#include <engine/render/material.hpp>
#include <engine/geom/brep/brep-mesh.hpp>
#include <memory>

/**
 * Generates terrain patches and performs terrain patch LOD selection.
 */
class terrain_system: public updatable_system
{
public:
	explicit terrain_system(entity::registry& registry);
	~terrain_system();
	
	virtual void update(float t, float dt);
	
	/**
	 * Generates terrain entities from a heightmap.
	 *
	 * @param heightmap Heightmap from which the terrain should be generated.
	 * @param subdivisions Number of heightmap subdivisions on the x- and z-axes. Determines the number of terrain entities generated.
	 * @param transform Translation, rotation, and scale of the terrain.
	 * @param material Terrain material.
	 *
	 * @return Entity ID of the generated terrain grid.
	 *
	 * @except std::invalid_argument Failed to generate terrain from null heightmap.
	 * @except std::runtime_error Heightmap size less than 2x2.
	 * @except std::runtime_error Heightmap subdivision failed.
	 */
	entity::id generate(std::shared_ptr<gl::image_2d> heightmap, const math::uvec2& subdivisions, const math::transform<float>& transform, std::shared_ptr<render::material> material);

private:
	[[nodiscard]] std::unique_ptr<render::model> generate_terrain_model(const geom::brep_mesh& mesh, std::shared_ptr<render::material> material, const math::uvec2& quad_dimensions) const;
};

#endif // ANTKEEPER_GAME_TERRAIN_SYSTEM_HPP
