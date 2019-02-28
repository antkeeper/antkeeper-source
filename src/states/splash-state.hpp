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

#ifndef SPLASH_STATE_HPP
#define SPLASH_STATE_HPP

#include "game-state.hpp"

#include <emergent/emergent.hpp>
using namespace Emergent;

/**
 * Displays the splash screen.
 */
class SplashState:
	public GameState,
	public EventHandler<KeyPressedEvent>,
	public EventHandler<MouseButtonPressedEvent>,
	public EventHandler<GamepadButtonPressedEvent>
{
public:
	SplashState(Game* game);
	virtual ~SplashState();
	virtual void enter();
	virtual void execute();
	virtual void exit();

private:
	virtual void handleEvent(const KeyPressedEvent& event);
	virtual void handleEvent(const MouseButtonPressedEvent& event);
	virtual void handleEvent(const GamepadButtonPressedEvent& event);
	void skip();

	Animation<float> fadeAnimation;
	AnimationClip<float> fadeInClip;
	AnimationClip<float> fadeOutClip;
};

#endif // SPLASH_STATE_HPP
