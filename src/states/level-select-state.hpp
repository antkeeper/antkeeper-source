/*
 * Copyright (C) 2017  Christopher J. Howard
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

#ifndef LEVEL_SELECT_STATE_HPP
#define LEVEL_SELECT_STATE_HPP

#include "../application-state.hpp"

#include <emergent/emergent.hpp>
using namespace Emergent;

class LevelSelectState: public ApplicationState
{
public:
	LevelSelectState(Application* application);
	virtual ~LevelSelectState();
	
	virtual void enter();
	virtual void execute();
	virtual void exit();
	
private:
	float levelRotation;
};

#endif // LEVEL_SELECT_STATE_HPP
