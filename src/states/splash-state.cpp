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

#include "splash-state.hpp"
#include "../application.hpp"
#include "title-state.hpp"

SplashState::SplashState(Application* application):
	ApplicationState(application)
{}

SplashState::~SplashState()
{}

void SplashState::enter()
{
	application->splashBackgroundImage->setVisible(true);
	application->splashImage->setVisible(true);
	application->splashFadeInTween->start();
}

void SplashState::execute()
{
	// Listen for splash screen skip
	InputEvent event;
	application->inputManager->listen(&event);
	if (event.type != InputEvent::Type::NONE)
	{
		// Update control profile and input manager
		application->menuControlProfile->update();
		application->inputManager->update();
		
		// Check if application was closed
		if (application->inputManager->wasClosed() || application->escape.isTriggered())
		{
			application->close(EXIT_SUCCESS);
			return;
		}
		
		// Check if fullscreen was toggled
		else if (application->toggleFullscreen.isTriggered() && !application->toggleFullscreen.wasTriggered())
		{
			application->changeFullscreen();
		}
		else
		{
			// Clear screen
			glClear(GL_COLOR_BUFFER_BIT);
			SDL_GL_SwapWindow(application->window);
			
			// Stop splash tweens
			application->splashFadeInTween->stop();
			application->splashHangTween->stop();
			application->splashFadeOutTween->stop();
			
			// Change to title state
			application->changeState(application->titleState);
			return;
		}
	}
}

void SplashState::exit()
{
	// Hide splash screen
	application->splashBackgroundImage->setVisible(false);
	application->splashImage->setVisible(false);
}
