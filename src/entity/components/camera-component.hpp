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

#ifndef CAMERA_COMPONENT_HPP
#define CAMERA_COMPONENT_HPP

#include "../component.hpp"
#include "component-type.hpp"

#include <emergent/emergent.hpp>
using namespace Emergent;

class CameraComponent: public Component<ComponentType::CAMERA>
{
public:
	virtual ComponentBase* clone() const;
	
	Camera* camera;
};

#endif // CAMERA_COMPONENT_HPP

