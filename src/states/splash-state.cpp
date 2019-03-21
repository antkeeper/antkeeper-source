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

#include "game.hpp"
#include "ui/ui.hpp"

void Game::enterSplashState()
{
	// Show splash screen
	splashBackgroundImage->setVisible(true);
	splashImage->setVisible(true);
	splashImage->setTintColor({1, 1, 1, 0});
	splashBackgroundImage->setTintColor({0, 0, 0, 1});
	splashImage->resetTweens();
	splashBackgroundImage->resetTweens();
	uiRootElement->update();
	
	// Add splash animations to animator
	animator.addAnimation(&splashFadeInAnimation);
	animator.addAnimation(&splashFadeOutAnimation);

	// Play splash fade-in animation
	splashFadeInAnimation.rewind();
	splashFadeInAnimation.play();
}

void Game::exitSplashState()
{
	// Hide splash screen 
	splashImage->setVisible(false);
	splashBackgroundImage->setVisible(false);
	uiRootElement->update();

	// Remove splash animations from animator
	animator.removeAnimation(&splashFadeInAnimation);
	animator.removeAnimation(&splashFadeOutAnimation);
}
