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

#ifndef ANTKEEPER_SCENE_AMBIENT_LIGHT_HPP
#define ANTKEEPER_SCENE_AMBIENT_LIGHT_HPP

#include <engine/scene/light.hpp>

namespace scene {

class ambient_light: public light
{
public:
	/// Returns light_type::ambient
	virtual light_type get_light_type() const;
};

inline light_type ambient_light::get_light_type() const
{
	return light_type::ambient;
}

} // namespace scene

#endif // ANTKEEPER_SCENE_AMBIENT_LIGHT_HPP

