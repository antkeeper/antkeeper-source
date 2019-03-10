/*
 * Copyright (C) 2017-2019  Christopher J. Howard
 *
 * This file is part of Antkeeper Source Code.
 *
 * Antkeeper Source Code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper Source Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper Source Code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TERRAIN_PATCH_COMPONENT_HPP
#define TERRAIN_PATCH_COMPONENT_HPP

#include "../component.hpp"
#include "component-type.hpp"
#include <tuple>

class TerrainPatchComponent: public Component<ComponentType::TERRAIN_PATCH>
{
public:
	virtual ComponentBase* clone() const;

	// Number of terrain mesh subdivisions (at LOD 0)
	int subdivisions;
	
	// Position in integer terrain coordinates
	std::tuple<int, int> position;
};

#endif // TERRAIN_PATCH_COMPONENT_HPP

