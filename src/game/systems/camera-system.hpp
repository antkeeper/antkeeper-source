// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_CAMERA_SYSTEM_HPP
#define ANTKEEPER_GAME_CAMERA_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"
#include <engine/math/vector.hpp>

class camera_system: public updatable_system
{
public:
	explicit camera_system(entity::registry& registry);
	~camera_system() override = default;
	void update(float t, float dt) override;
	void interpolate(float alpha);
	
	void set_viewport(const math::fvec4& viewport);

private:
	math::dvec4 m_viewport{};
	double m_aspect_ratio{};
	double m_fixed_update_time{};
	double m_fixed_timestep{};
	double m_variable_update_time{};
};

#endif // ANTKEEPER_GAME_CAMERA_SYSTEM_HPP
