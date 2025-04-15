// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_LARVA_COMPONENT_HPP
#define ANTKEEPER_GAME_LARVA_COMPONENT_HPP

import engine.render.material_variable;
import engine.entity.id;

using namespace engine;

/// Larval development parameters.
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
