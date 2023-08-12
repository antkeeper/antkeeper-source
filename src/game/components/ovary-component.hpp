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

#ifndef ANTKEEPER_GAME_OVARY_COMPONENT_HPP
#define ANTKEEPER_GAME_OVARY_COMPONENT_HPP

#include <engine/animation/bone.hpp>
#include <engine/geom/primitives/line-segment.hpp>
#include <engine/entity/id.hpp>
#include <cstdint>
#include <memory>

/**
 * 
 */
struct ovary_component
{
	/// Maximum number of concurrent eggs.
	std::uint16_t egg_capacity{};
	
	/// Number of fully-developed eggs.
	std::uint16_t egg_count{};
	
	/// Duration required to produce an egg, in seconds.
	float egg_production_duration{};
	
	/// Elapsed time the current egg has been in production, in seconds.
	float elapsed_egg_production_time{};
	
	/// Duration required to lay an egg, in seconds.
	float oviposition_duration{};
	
	/// Elapsed time the current egg has been traveling down the common oviduct, in seconds.
	float elapsed_oviposition_time{};
	
	/// `true` if currently ovipositing an egg, `false` otherwise.
	bool ovipositing{};
	
	/// Bone of the ovipositor.
	bone_index_type ovipositor_bone{};
	
	/// Path along which eggs travel while being oviposited, relative to the ovipositor bone.
	geom::line_segment<float, 3> oviposition_path{};
	
	/// Entity ID of the egg currently in the ovipositor (if any).
	entity::id ovipositor_egg_eid{entt::null};
};

#endif // ANTKEEPER_GAME_OVARY_COMPONENT_HPP
