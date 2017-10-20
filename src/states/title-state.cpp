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
#include "../application.hpp"
#include "../camera-controller.hpp"
#include "../ui/menu.hpp"
#include <iostream>
#include <SDL2/SDL.h>

TitleState::TitleState(Application* application):
	ApplicationState(application)
{}

TitleState::~TitleState()
{}

void TitleState::enter()
{
	application->backgroundLayer->addObject(&application->bgCamera);
	application->backgroundLayer->addObject(&application->bgBatch);
	
	application->inputManager->addWindowObserver(this);
	windowResized(application->resolution.x, application->resolution.y);
	
	application->camera.setPerspective(
		glm::radians(25.0f),
		application->resolution.x / application->resolution.y,
		0.1f,
		1000.0f);
	
	// Setup camera controller
	application->surfaceCam->setCamera(&application->camera);
	application->surfaceCam->setFocalPoint(Vector3(0.0f));
	application->surfaceCam->setFocalDistance(50.0f);
	application->surfaceCam->setElevation(glm::radians(-30.0f));
	application->surfaceCam->setAzimuth(glm::radians(180.0f));
	application->surfaceCam->setTargetFocalPoint(application->surfaceCam->getFocalPoint());
	application->surfaceCam->setTargetFocalDistance(application->surfaceCam->getFocalDistance());
	application->surfaceCam->setTargetElevation(application->surfaceCam->getElevation());
	application->surfaceCam->setTargetAzimuth(application->surfaceCam->getAzimuth());
	application->surfaceCam->update(0.0f);
	
	// Dim background
	application->darkenImage->setVisible(true);
	
	// Show title
	application->titleImage->setVisible(true);
	application->titleImage->setTintColor(Vector4(1.0f));

	// Open main menu
	application->openMenu(application->mainMenu);
	
	// Position options menu
	application->optionsMenu->getUIContainer()->setAnchor(Vector2(0.5f, 0.8f));
	application->controlsMenu->getUIContainer()->setAnchor(Vector2(0.5f, 0.8f));
	application->levelsMenu->getUIContainer()->setAnchor(Vector2(0.5f, 0.8f));
	
	// Setup fade-in
	application->blackoutImage->setVisible(true);
	application->fadeInTween->start();
}

void TitleState::execute()
{
	// Navigate menu
	if (application->activeMenu != nullptr)
	{
		MenuItem* selectedItem = application->activeMenu->getSelectedItem();
		
		if (application->menuDown.isTriggered() && !application->menuDown.wasTriggered())
		{
			if (selectedItem != nullptr)
			{
				if (selectedItem->getItemIndex() < application->activeMenu->getItemCount() - 1)
				{
					application->selectMenuItem(selectedItem->getItemIndex() + 1);
				}
				else
				{
					application->selectMenuItem(0);
				}
			}
			else
			{
				application->selectMenuItem(0);
			}
		}
		else if (application->menuUp.isTriggered() && !application->menuUp.wasTriggered())
		{
			if (selectedItem != nullptr)
			{
				if (selectedItem->getItemIndex() > 0)
				{
					application->selectMenuItem(selectedItem->getItemIndex() - 1);
				}
				else
				{
					application->selectMenuItem(application->activeMenu->getItemCount() - 1);
				}
			}
			else
			{
				application->selectMenuItem(application->activeMenu->getItemCount() - 1);
			}
		}
		
		if (application->menuLeft.isTriggered() && !application->menuLeft.wasTriggered())
		{
			application->decrementMenuItem();
		}
		else if (application->menuRight.isTriggered() && !application->menuRight.wasTriggered())
		{
			application->incrementMenuItem();
		}
		
		if (application->menuSelect.isTriggered() && !application->menuSelect.wasTriggered())
		{
			application->activateMenuItem();
		}
	}
	
	if (application->escape.isTriggered())
	{
		application->close(EXIT_SUCCESS);
	}
	
	
	// Set selector icon position
	/*
	float lineHeight = application->menuFont->getMetrics().getHeight();
	const UIContainer* container = application->menuContainers[application->currentMenuIndex];
	application->menuSelectorLabel->setTranslation(
		Vector2(container->getPosition().x - application->menuSelectorLabel->getDimensions().x * 1.5f,
			container->getPosition().y + lineHeight * 0.5f - application->menuSelectorLabel->getDimensions().y * 0.5f + lineHeight * application->selectedMenuItemIndex));
	*/
}

void TitleState::exit()
{
	// Remove input observers
	application->inputManager->removeWindowObserver(this);
	
	// Hide UI
	application->titleImage->setVisible(false);
	application->anyKeyLabel->setVisible(false);
	application->darkenImage->setVisible(false);

	
	// Remove clear scene
	application->backgroundLayer->removeObject(&application->bgCamera);
	application->backgroundLayer->removeObject(&application->bgBatch);
}

void TitleState::windowClosed()
{
	application->close(EXIT_SUCCESS);
}

void TitleState::windowResized(int width, int height)
{
	/*
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
	*/
}
