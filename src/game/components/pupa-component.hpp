// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_PUPA_COMPONENT_HPP
#define ANTKEEPER_GAME_PUPA_COMPONENT_HPP

import engine.render.material_variable;
import engine.entity.id;

using namespace engine;

struct pupa_component
{
	/// Duration of the development period, in days.
	float development_period{};
	
	/// Current development phase, on `[0, 1]`.
	float development_phase{};
	
	/// ID of the cocoon entity.
	entity::id cocoon_eid{entt::null};
	
	/// Material variable associated with the cocoon decay phase.
	std::shared_ptr<render::matvar_float> decay_phase_matvar;
};

#endif // ANTKEEPER_GAME_PUPA_COMPONENT_HPP
