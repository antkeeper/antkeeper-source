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

#ifndef ANTKEEPER_GAME_LARVA_COMPONENT_HPP
#define ANTKEEPER_GAME_LARVA_COMPONENT_HPP

#include <engine/entity/id.hpp>
#include <engine/render/material-variable.hpp>

/**
 * Larval development parameters.
 */
struct larva_component
{
	/// Duration of the development period, in days.
	float development_period{};
	
	/// Current development phase, on `[0, 1]`.
	float development_phase{};
	
	/// Duration of the cocoon-spinning period, in days.
	float spinning_period{};
	
	/// Current phase of the cocoon-spinning, on `[0, 1]`.
	float spinning_phase{};
	
	/// ID of the cocoon entity.
	entity::id cocoon_eid{entt::null};
	
	/// Material variable associated with the cocoon-spinning phase.
	std::shared_ptr<render::matvar_float> spinning_phase_matvar;
};

#endif // ANTKEEPER_GAME_LARVA_COMPONENT_HPP
