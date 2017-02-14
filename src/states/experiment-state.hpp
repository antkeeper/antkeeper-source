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

#ifndef EXPERIMENT_STATE_HPP
#define EXPERIMENT_STATE_HPP

#include "../application-state.hpp"
#include "../input.hpp"
#include "../debug.hpp"
#include "../nest.hpp"
#include "../terrain.hpp"

#include <emergent/emergent.hpp>
using namespace Emergent;

class ExperimentState: public ApplicationState, public WindowObserver, public MouseButtonObserver
{
public:
	ExperimentState(Application* application);
	virtual ~ExperimentState();
	
	virtual void enter();
	virtual void execute();
	virtual void exit();
	
	virtual void windowClosed();
	virtual void windowResized(int width, int height);
	virtual void mouseButtonPressed(int button, int x, int y);
	virtual void mouseButtonReleased(int button, int x, int y);
	
	void generateNest();
	
private:
	Timer timer;	
	Nest nest;
	Terrain terrain;
	bool dragging;
	bool oldDragging;
	Vector3 dragStart;
	Vector3 dragEnd;
};

#endif // EXPERIMENT_STATE_HPP