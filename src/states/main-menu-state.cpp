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

#include "main-menu-state.hpp"
#include "../application.hpp"

MainMenuState::MainMenuState(Application* application):
	ApplicationState(application)
{}

MainMenuState::~MainMenuState()
{}

void MainMenuState::enter()
{
	// Open main menu
	application->enterMenu(0);					
	application->menuSelectorLabel->setVisible(true);
	
	// Start fade-in
	application->fadeInTween->start();
}

void MainMenuState::execute()
{
	// Navigate menu
	if (application->menuDown.isTriggered() && !application->menuDown.wasTriggered())
	{
		if (application->selectedMenuItemIndex < application->currentMenu->getItemCount() - 1)
		{
			application->selectMenuItem(application->selectedMenuItemIndex + 1);
		}
		else
		{
			application->selectMenuItem(0);
		}
	}
	else if (application->menuUp.isTriggered() && !application->menuUp.wasTriggered())
	{
		if (application->selectedMenuItemIndex > 0)
		{
			application->selectMenuItem(application->selectedMenuItemIndex - 1);
		}
		else
		{
			application->selectMenuItem(application->currentMenu->getItemCount() - 1);
		}
	}
	
	if (application->menuSelect.isTriggered() && !application->menuSelect.wasTriggered())
	{
		application->activateMenuItem(application->selectedMenuItemIndex);
	}
	else if (application->menuCancel.isTriggered() && !application->menuCancel.wasTriggered())
	{
		
	}
	
	float lineHeight = application->menuFont->getMetrics().getHeight();
	const UIContainer* container = application->menuContainers[application->currentMenuIndex];
	application->menuSelectorLabel->setTranslation(
		Vector2(container->getPosition().x - application->menuSelectorLabel->getDimensions().x * 1.5f,
			container->getPosition().y + lineHeight * 0.5f - application->menuSelectorLabel->getDimensions().y * 0.5f + lineHeight * application->selectedMenuItemIndex));
}

void MainMenuState::exit()
{
}

void MainMenuState::mouseButtonPressed(int button, int x, int y)
{
}

void MainMenuState::mouseButtonReleased(int button, int x, int y)
{
}
