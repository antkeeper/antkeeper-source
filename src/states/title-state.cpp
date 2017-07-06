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

#include "title-state.hpp"
#include "main-menu-state.hpp"
#include "../application.hpp"
#include "../camera-controller.hpp"
#include <iostream>
#include <SDL.h>

const float blankDuration = 0.0f;
const float fadeInDuration = 0.5f;
const float hangDuration = 1.0f;
const float fadeOutDuration = 0.5f;
const float titleDelay = 2.0f;
const float copyrightDelay = 3.0f;
const float pressAnyKeyDelay = 5.0f;

TitleState::TitleState(Application* application):
	ApplicationState(application)
{}

TitleState::~TitleState()
{}

void TitleState::enter()
{	
	// Setup screen fade-in transition
	fadeIn = false;
	fadeOut = false;
	

	application->backgroundLayer->addObject(&application->bgCamera);
	application->backgroundLayer->addObject(&application->bgBatch);
	
	// Title ant hill
	application->defaultLayer->addObject(&application->antHillModelInstance);
	
	application->inputManager->addWindowObserver(this);
	windowResized(application->width, application->height);
	
	// Setup camera controller
	application->surfaceCam->setCamera(&application->camera);
	application->surfaceCam->setFocalPoint(Vector3(0.0f));
	application->surfaceCam->setFocalDistance(50.0f);
	application->surfaceCam->setElevation(glm::radians(0.0f));
	application->surfaceCam->setAzimuth(glm::radians(0.0f));
	application->surfaceCam->setTargetFocalPoint(application->surfaceCam->getFocalPoint());
	application->surfaceCam->setTargetFocalDistance(application->surfaceCam->getFocalDistance());
	application->surfaceCam->setTargetElevation(application->surfaceCam->getElevation());
	application->surfaceCam->setTargetAzimuth(application->surfaceCam->getAzimuth());
	application->surfaceCam->update(0.0f);
	
	// Setup fade-in
	application->blackoutImage->setVisible(true);
	application->fadeInTween->start();
	
	// Start timer
	stateTime = 0.0f;
	substate = 0;
}

void TitleState::execute()
{
	// Add dt to state time
	stateTime += application->dt;
	
	if (substate == 0 || substate == 1)
	{
		if (stateTime >= titleDelay && !application->titleImage->isVisible())
		{
			application->titleImage->setVisible(true);
			application->titleFadeInTween->start();
			application->titleScreenInfoContainer->setVisible(true);
			application->copyrightFadeInTween->start();
		}
		
		if (stateTime >= pressAnyKeyDelay && !application->anyKeyLabel->isVisible())
		{
			application->anyKeyLabel->setVisible(true);
			application->anyKeyFadeInTween->start();
		}
	}
	
	if (substate == 0 && stateTime >= titleDelay && application->titleFadeInTween->isStopped())
	{
		substate = 1;
	}
	
	// Listen for fade-in skip and "press any key"
	if (substate < 2)
	{
		InputEvent event;
		application->inputManager->listen(&event);
		
		if (event.type != InputEvent::Type::NONE)
		{
			application->menuControlProfile->update();
			application->inputManager->update();
						
			// Check if application was closed
			if (application->escape.isTriggered())
			{
				application->close(EXIT_SUCCESS);
				return;
			}
			// Check if fullscreen was toggled
			else if (application->toggleFullscreen.isTriggered() && !application->toggleFullscreen.wasTriggered())
			{
				application->changeFullscreen();
			}
			else if (!application->menuCancel.isTriggered())
			{
				if (substate == 0)
				{
					// Remove fade-in
					substate = 1;
					application->fadeInTween->stop();
					application->blackoutImage->setTintColor(Vector4(0.0f));
					application->blackoutImage->setVisible(false);
					application->titleFadeInTween->stop();
					application->titleImage->setVisible(true);
					application->titleImage->setTintColor(Vector4(1.0f));
					application->anyKeyFadeInTween->start();
					application->anyKeyLabel->setVisible(true);
					application->copyrightFadeInTween->stop();
					application->titleScreenInfoContainer->setVisible(true);
					application->titleScreenInfoContainer->setTintColor(Vector4(0.0f, 0.0f, 0.0f, 0.5f));
				}
				else if (substate == 1)
				{
					// Enter main menu
					substate = 2;
					application->titleFadeInTween->stop();
					application->titleFadeOutTween->start();
					application->anyKeyFadeInTween->stop();
					application->anyKeyFadeOutTween->stop();
					application->anyKeyLabel->setVisible(false);
					application->copyrightFadeInTween->stop();
					application->copyrightFadeOutTween->start();
					
					application->antHillZoomInTween->start();
					
					application->blackoutImage->setVisible(true);
					application->antHillFadeOutTween->start();
				}
			}
		}
	}
	
	application->surfaceCam->update(application->dt);
}

void TitleState::exit()
{
	application->titleImage->setVisible(false);
	application->anyKeyLabel->setVisible(false);
	application->titleScreenInfoContainer->setVisible(false);
	
	// Remove objects from scene
	application->defaultLayer->removeObject(&application->antHillModelInstance);
	application->backgroundLayer->removeObject(&application->bgCamera);
	application->backgroundLayer->removeObject(&application->bgBatch);
	
	application->inputManager->removeWindowObserver(this);
}

void TitleState::windowClosed()
{
	application->close(EXIT_SUCCESS);
}

void TitleState::windowResized(int width, int height)
{
	// Update application dimensions
	application->width = width;
	application->height = height;
	if (application->fullscreen)
	{
		application->fullscreenWidth = width;
		application->fullscreenHeight = height;
	}
	else
	{
		application->windowedWidth = width;
		application->windowedHeight = height;
	}
	
	// Setup default render target
	application->defaultRenderTarget.width = application->width;
	application->defaultRenderTarget.height = application->height;
	
	// Resize UI
	application->resizeUI();
	
	// 3D camera
	application->camera.setPerspective(
		glm::radians(25.0f),
		(float)application->width / (float)application->height,
		0.1f,
		1000.0f);
}
