// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_OVARY_COMPONENT_HPP
#define ANTKEEPER_GAME_OVARY_COMPONENT_HPP

import engine.geom.primitives.line_segment;
import engine.entity.id;
import engine.animation.bone;
import engine.utility.sized_types;
import <memory>;

using namespace engine;

struct ovary_component
{
	/// Maximum number of concurrent eggs.
	u16 egg_capacity{};
	
	/// Number of fully-developed eggs.
	u16 egg_count{};
	
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
	usize ovipositor_bone{};
	
	/// Path along which eggs travel while being oviposited, relative to the ovipositor bone.
	geom::line_segment<float, 3> oviposition_path{};
	
	/// Entity ID of the egg currently in the ovipositor (if any).
	entity::id ovipositor_egg_eid{entt::null};
};

#endif // ANTKEEPER_GAME_OVARY_COMPONENT_HPP
