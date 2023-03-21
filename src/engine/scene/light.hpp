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

#ifndef ANTKEEPER_SCENE_LIGHT_HPP
#define ANTKEEPER_SCENE_LIGHT_HPP

#include <engine/scene/object.hpp>
#include <engine/scene/light-type.hpp>

namespace scene {

/**
 * Abstract base class for light objects.
 */
class light: public object<light>
{
public:
	/// Returns an enumeration denoting the light object type.
	[[nodiscard]] virtual light_type get_light_type() const noexcept = 0;
	
	inline const aabb_type& get_bounds() const noexcept override
	{
		return m_bounds;
	}

private:
	virtual void transformed();
	aabb_type m_bounds{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
};

} // namespace scene

#endif // ANTKEEPER_SCENE_LIGHT_HPP
