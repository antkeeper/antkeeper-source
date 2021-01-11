/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_SCENE_DIRECTIONAL_LIGHT_HPP
#define ANTKEEPER_SCENE_DIRECTIONAL_LIGHT_HPP

#include "scene/light.hpp"
#include "utility/fundamental-types.hpp"

namespace scene {

class directional_light: public light
{
public:
	directional_light();
	
	/// Returns light_type::directional
	virtual light_type get_light_type() const;

	const float3& get_direction() const;

	const tween<float3>& get_direction_tween() const;

	/// @copydoc object_base::update_tweens();
	virtual void update_tweens();

private:
	virtual void transformed();

	tween<float3> direction;
};

inline light_type directional_light::get_light_type() const
{
	return light_type::directional;
}

inline const float3& directional_light::get_direction() const
{
	return direction[1];
}

inline const tween<float3>& directional_light::get_direction_tween() const
{
	return direction;
}

} // namespace scene

#endif // ANTKEEPER_SCENE_DIRECTIONAL_LIGHT_HPP

