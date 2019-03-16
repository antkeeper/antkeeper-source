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

#ifndef SANDBOX_STATE_HPP
#define SANDBOX_STATE_HPP

#include "game-state.hpp"
#include <tuple>

#include <emergent/emergent.hpp>
using namespace Emergent;

class SandboxState:
	public GameState,
	public EventHandler<MouseMovedEvent>,
	public EventHandler<MouseButtonPressedEvent>,
	public EventHandler<MouseButtonReleasedEvent>
{
public:
	SandboxState(Game* game);
	virtual ~SandboxState();
	virtual void enter();
	virtual void execute();
	virtual void exit();

private:
	virtual void handleEvent(const MouseMovedEvent& event);
	virtual void handleEvent(const MouseButtonPressedEvent& event);
	virtual void handleEvent(const MouseButtonReleasedEvent& event);

	std::tuple<int, int> savedMousePosition;
	Vector2 selectorVector;
	std::tuple<int, int> dragMousePosition;
	Vector3 dragReferencePoint;
	Vector3 dragOffset;
	Plane dragPlane;
	float zoom;
	int toolIndex;
	bool noPick;
};

#endif // SANDBOX_STATE_HPP
