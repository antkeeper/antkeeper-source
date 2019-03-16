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

#include "tool.hpp"

Tool::Tool():
	active(false),
	pick(0.0f),
	orbitCam(nullptr)
{
	modelInstance.setActive(active);
	modelInstance.setCullingEnabled(false);
}

Tool::~Tool()
{}

void Tool::setActive(bool active)
{
	this->active = active;
	if (!active)
	{
		modelInstance.setActive(false);
	}
}

void Tool::setOrbitCam(const OrbitCam* orbitCam)
{
	this->orbitCam = orbitCam;
}

